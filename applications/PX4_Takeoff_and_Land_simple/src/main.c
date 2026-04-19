/*
 * ============================================================
 * PX4 Takeoff-and-Land — Simple sequential implementation
 * ============================================================
 *
 * Direct C translation of the Python pymavlink script.
 * No RTOS scheduler — just sequential calls with k_msleep()
 * replacing time.sleep(), and a UART ISR ring buffer replacing
 * Python's blocking socket reads.
 *
 * Hardware connection:
 *   STM32F4-Discovery USART3  (PB10 = TX,  PB11 = RX)
 *   → PX4 flight controller TELEM1 port
 *   Baud: 57600 · 8-N-1  (set PX4 param SER_TEL1_BAUD = 57600)
 *   Wiring: PB10 → TELEM_RX,  PB11 → TELEM_TX,  GND → GND
 *
 * Execution flow  (mirrors the Python script):
 *   1. Send heartbeats until PX4 responds          (wait_heartbeat)
 *   2. set_mode_auto()  + sleep 5 s
 *   3. arm()            + sleep 5 s
 *   4. takeoff(10 m)    + sleep 5 s  + wait for COMMAND_ACK
 *   5. Hover 5 s
 *   6. land()           + sleep 5 s  + wait for COMMAND_ACK
 *   7. disarm()
 * ============================================================
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>

/* ─────────────────────────────────────────────────────────────
 * UART  (USART3 — see app.overlay)
 * ───────────────────────────────────────────────────────────── */

#define MAVLINK_UART_NODE  DT_NODELABEL(usart3)

static const struct device *uart_dev;

/* Power-of-2 ring buffer filled by the ISR, drained by poll_uart(). */
#define RX_BUF_SIZE  256u
#define RX_BUF_MASK  (RX_BUF_SIZE - 1u)

static uint8_t  rx_buf[RX_BUF_SIZE];
static uint32_t rx_head;   /* written by ISR      */
static uint32_t rx_tail;   /* written by main     */

static void uart_isr(const struct device *dev, void *ud)
{
	ARG_UNUSED(ud);
	while (uart_irq_update(dev) && uart_irq_rx_ready(dev)) {
		uint8_t c;
		if (uart_fifo_read(dev, &c, 1) == 1) {
			rx_buf[rx_head & RX_BUF_MASK] = c;
			rx_head++;
		}
	}
}

static bool rx_get(uint8_t *b)
{
	if (rx_tail == rx_head) {
		return false;
	}
	*b = rx_buf[rx_tail & RX_BUF_MASK];
	rx_tail++;
	return true;
}

static void uart_send(const uint8_t *data, uint8_t len)
{
	for (uint8_t i = 0; i < len; i++) {
		uart_poll_out(uart_dev, data[i]);
	}
}

/* ─────────────────────────────────────────────────────────────
 * MAVLink v1 — encoder
 * ───────────────────────────────────────────────────────────── */

static void crc_accum(uint16_t *crc, uint8_t x)
{
	uint8_t t = x ^ (uint8_t)(*crc & 0xFF);
	t ^= t << 4;
	*crc = (*crc >> 8) ^ ((uint16_t)t << 8) ^ ((uint16_t)t << 3) ^ ((uint16_t)t >> 4);
}

static uint8_t g_seq;

static uint8_t mav_frame(uint8_t *out, uint8_t msgid,
			  const uint8_t *payload, uint8_t plen,
			  uint8_t crc_extra)
{
	out[0] = 0xFE;
	out[1] = plen;
	out[2] = g_seq++;
	out[3] = 255;   /* sysid  = 255 (GCS) */
	out[4] = 0;     /* compid = 0         */
	out[5] = msgid;
	memcpy(&out[6], payload, plen);

	uint16_t crc = 0xFFFF;
	for (uint8_t i = 1; i < 6u + plen; i++) {
		crc_accum(&crc, out[i]);
	}
	crc_accum(&crc, crc_extra);

	out[6 + plen] = (uint8_t)(crc & 0xFF);
	out[7 + plen] = (uint8_t)(crc >> 8);
	return 8u + plen;
}

/* HEARTBEAT  msgid=0, plen=9, crc_extra=50 */
static uint8_t mav_heartbeat(uint8_t *out)
{
	const uint8_t p[9] = {
		0, 0, 0, 0,   /* custom_mode (uint32 LE)          */
		6,            /* type      = MAV_TYPE_GCS          */
		8,            /* autopilot = MAV_AUTOPILOT_INVALID */
		0,            /* base_mode                         */
		0,            /* system_status                     */
		3,            /* mavlink_version                   */
	};
	return mav_frame(out, 0, p, sizeof(p), 50);
}

/* COMMAND_LONG  msgid=76, plen=33, crc_extra=152 */
static uint8_t mav_command_long(uint8_t *out, uint16_t cmd,
				float p1, float p2, float p3,
				float p4, float p5, float p6, float p7)
{
	uint8_t p[33];
	memcpy(&p[ 0], &p1, 4);
	memcpy(&p[ 4], &p2, 4);
	memcpy(&p[ 8], &p3, 4);
	memcpy(&p[12], &p4, 4);
	memcpy(&p[16], &p5, 4);
	memcpy(&p[20], &p6, 4);
	memcpy(&p[24], &p7, 4);
	p[28] = (uint8_t)(cmd & 0xFF);
	p[29] = (uint8_t)(cmd >> 8);
	p[30] = 1;   /* target_system    = 1 */
	p[31] = 1;   /* target_component = 1 */
	p[32] = 0;   /* confirmation         */
	return mav_frame(out, 76, p, sizeof(p), 152);
}

/* ─────────────────────────────────────────────────────────────
 * MAVLink v1 — decoder
 * ───────────────────────────────────────────────────────────── */

static bool     g_heartbeat_received;   /* set when any HEARTBEAT arrives   */
static bool     g_ack_ready;            /* set when a COMMAND_ACK arrives   */
static uint16_t g_ack_cmd;             /* which command was acknowledged    */
static uint8_t  g_ack_result;          /* 0 = MAV_RESULT_ACCEPTED           */

static uint8_t crc_extra_for(uint8_t msgid)
{
	switch (msgid) {
	case  0: return  50;   /* HEARTBEAT   */
	case 77: return 143;   /* COMMAND_ACK */
	default: return   0;
	}
}

static void mav_dispatch(uint8_t msgid, const uint8_t *payload)
{
	switch (msgid) {
	case 0:
		/* HEARTBEAT — connection is established */
		g_heartbeat_received = true;
		break;

	case 77:
		/* COMMAND_ACK: command (uint16 @ 0), result (uint8 @ 2) */
		memcpy(&g_ack_cmd, &payload[0], 2);
		g_ack_result = payload[2];
		g_ack_ready  = true;
		break;

	default:
		break;
	}
}

/* MAVLink v1 byte parser — state persists between poll_uart() calls */
static enum {
	RX_STX, RX_LEN, RX_SEQ, RX_SYS, RX_COMP, RX_MSGID,
	RX_PAYLOAD, RX_CRC1, RX_CRC2
} rx_state;

static uint8_t  rx_plen, rx_msgid, rx_payload[64], rx_pidx, rx_crc1_got;
static uint16_t rx_crc_run;

static void mav_parse_byte(uint8_t b)
{
	switch (rx_state) {
	case RX_STX:
		if (b == 0xFE) { rx_crc_run = 0xFFFF; rx_state = RX_LEN; }
		break;
	case RX_LEN:
		crc_accum(&rx_crc_run, b); rx_plen = b; rx_state = RX_SEQ;
		break;
	case RX_SEQ:
		crc_accum(&rx_crc_run, b); rx_state = RX_SYS;
		break;
	case RX_SYS:
		crc_accum(&rx_crc_run, b); rx_state = RX_COMP;
		break;
	case RX_COMP:
		crc_accum(&rx_crc_run, b); rx_state = RX_MSGID;
		break;
	case RX_MSGID:
		crc_accum(&rx_crc_run, b);
		rx_msgid = b; rx_pidx = 0;
		rx_state = (rx_plen > 0) ? RX_PAYLOAD : RX_CRC1;
		break;
	case RX_PAYLOAD:
		crc_accum(&rx_crc_run, b);
		if (rx_pidx < sizeof(rx_payload)) { rx_payload[rx_pidx] = b; }
		if (++rx_pidx >= rx_plen) { rx_state = RX_CRC1; }
		break;
	case RX_CRC1:
		rx_crc1_got = b; rx_state = RX_CRC2;
		break;
	case RX_CRC2: {
		uint16_t final_crc = rx_crc_run;
		crc_accum(&final_crc, crc_extra_for(rx_msgid));
		uint16_t got = rx_crc1_got | ((uint16_t)b << 8);
		if (final_crc == got) { mav_dispatch(rx_msgid, rx_payload); }
		rx_state = RX_STX;
		break;
	}
	}
}

/* ─────────────────────────────────────────────────────────────
 * Helpers — replacing Python's socket/recv_match/time.sleep
 * ───────────────────────────────────────────────────────────── */

/* Drain every byte from the ring buffer through the MAVLink parser.
 * Call this frequently so g_heartbeat_received / g_ack_ready stay fresh. */
static void poll_uart(void)
{
	uint8_t b;

	while (rx_get(&b)) {
		mav_parse_byte(b);
	}
}

/* Sleep for 'ms' milliseconds while:
 *  - draining the UART ring buffer every 100 ms  (prevents overflow)
 *  - sending a MAVLink heartbeat every 1 s       (keeps PX4 link alive)
 *
 * Replaces Python's  time.sleep(seconds).
 */
static void sleep_and_poll(int32_t ms)
{
	uint8_t  frame[17];
	int64_t  end     = k_uptime_get() + ms;
	int64_t  next_hb = k_uptime_get() + 1000;

	while (k_uptime_get() < end) {
		poll_uart();
		if (k_uptime_get() >= next_hb) {
			uart_send(frame, mav_heartbeat(frame));
			next_hb += 1000;
		}
		k_msleep(100);
	}
}

/*
 * Block until a COMMAND_ACK for 'cmd' arrives or timeout expires.
 * Returns true  if PX4 accepted the command (result == 0).
 * Returns false on rejection or timeout.
 *
 * Replaces the Python  while True: recv_match(type='COMMAND_ACK')  loop.
 * Also sends heartbeats every 1 s to keep the link alive.
 */
static bool wait_for_ack(uint16_t cmd, uint32_t timeout_ms)
{
	uint8_t  frame[17];
	int64_t  end     = k_uptime_get() + timeout_ms;
	int64_t  next_hb = k_uptime_get() + 1000;

	while (k_uptime_get() < end) {
		poll_uart();

		if (g_ack_ready && g_ack_cmd == cmd) {
			bool ok = (g_ack_result == 0);

			printk("  COMMAND_ACK cmd=%u result=%u → %s\n",
			       g_ack_cmd, g_ack_result, ok ? "ACCEPTED" : "REJECTED");
			return ok;
		}

		if (k_uptime_get() >= next_hb) {
			uart_send(frame, mav_heartbeat(frame));
			next_hb += 1000;
		}

		k_msleep(100);
	}

	printk("  COMMAND_ACK timeout (cmd=%u)\n", cmd);
	return false;
}

/*
 * Send heartbeats until PX4 sends one back.
 * Replaces Python's  master.wait_heartbeat().
 */
static void wait_for_heartbeat(void)
{
	uint8_t frame[17];

	printk("Sending heartbeats — waiting for PX4 connection...\n");
	g_heartbeat_received = false;

	while (!g_heartbeat_received) {
		uart_send(frame, mav_heartbeat(frame));
		/* poll for up to 1 s between sends */
		for (int i = 0; i < 10 && !g_heartbeat_received; i++) {
			k_msleep(100);
			poll_uart();
		}
	}

	printk("Connection established.\n");
}

/* ─────────────────────────────────────────────────────────────
 * Mission commands — mirrors Python functions directly
 * ───────────────────────────────────────────────────────────── */

#define CMD_DO_SET_MODE  176u   /* MAV_CMD_DO_SET_MODE            */
#define CMD_ARM_DISARM   400u   /* MAV_CMD_COMPONENT_ARM_DISARM   */
#define CMD_TAKEOFF       22u   /* MAV_CMD_NAV_TAKEOFF             */
#define CMD_LAND          21u   /* MAV_CMD_NAV_LAND                */

#define TAKEOFF_ALT_M  10       /* metres */

/*
 * arm_and_takeoff(master, altitude)  →  arm_and_takeoff(altitude)
 *
 * Python:
 *   master.set_mode_auto()               → send CMD_DO_SET_MODE
 *   time.sleep(5)                        → sleep_and_poll(5000)
 *   master.arducopter_arm()              → send CMD_ARM_DISARM param1=1
 *   time.sleep(5)                        → sleep_and_poll(5000)
 *   command_long_send(MAV_CMD_NAV_TAKEOFF, altitude)
 *   time.sleep(5)                        → sleep_and_poll(5000)
 *   while True: recv_match COMMAND_ACK   → wait_for_ack(CMD_TAKEOFF)
 */
static void arm_and_takeoff(int altitude)
{
	uint8_t frame[64];
	uint8_t len;

	/* Set AUTO mode
	 *   param1 = 1  → MAV_MODE_FLAG_CUSTOM_MODE_ENABLED
	 *   param2 = 4  → PX4 main mode 4 = AUTO                    */
	len = mav_command_long(frame, CMD_DO_SET_MODE,
			       1.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	uart_send(frame, len);
	printk("Set AUTO mode sent\n");
	sleep_and_poll(5000);   /* time.sleep(5) */

	/* Arm
	 *   param1 = 1  → ARM  (use 21196 to force-arm in SITL)     */
	len = mav_command_long(frame, CMD_ARM_DISARM,
			       1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	uart_send(frame, len);
	printk("Arm command sent\n");
	sleep_and_poll(5000);   /* time.sleep(5) */

	/* Takeoff
	 *   param7 = altitude in metres (relative to home)           */
	g_ack_ready = false;    /* clear any stale ACK before takeoff */
	len = mav_command_long(frame, CMD_TAKEOFF,
			       0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, (float)altitude);
	uart_send(frame, len);
	printk("Take off command sent\n");
	sleep_and_poll(5000);   /* time.sleep(5) */

	/* Wait for COMMAND_ACK (mirrors Python's while True: recv_match) */
	bool ok = wait_for_ack(CMD_TAKEOFF, 30000);

	if (ok) {
		printk("Takeoff to %d meters succeeded\n", altitude);
	} else {
		printk("Takeoff to %d meters failed\n", altitude);
	}
}

/*
 * land(master)  →  land()
 *
 * Python:
 *   command_long_send(MAV_CMD_NAV_LAND, ...)
 *   time.sleep(5)
 *   while True: recv_match COMMAND_ACK
 */
static void land(void)
{
	uint8_t frame[64];

	/* All params = 0 → land at current (home) position */
	g_ack_ready = false;
	uart_send(frame, mav_command_long(frame, CMD_LAND,
					  0.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 0.0f, 0.0f));
	printk("Land command sent\n");
	sleep_and_poll(5000);   /* time.sleep(5) */

	bool ok = wait_for_ack(CMD_LAND, 30000);

	if (ok) {
		printk("Landing succeeded\n");
	} else {
		printk("Landing failed\n");
	}
}

/*
 * master.arducopter_disarm()  →  disarm()
 *   param1 = 0 → DISARM
 */
static void disarm(void)
{
	uint8_t frame[64];

	uart_send(frame, mav_command_long(frame, CMD_ARM_DISARM,
					  0.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 0.0f, 0.0f));
	printk("Disarmed\n");
}

/* ─────────────────────────────────────────────────────────────
 * main
 * ───────────────────────────────────────────────────────────── */
int main(void)
{
	uart_dev = DEVICE_DT_GET(MAVLINK_UART_NODE);
	if (!device_is_ready(uart_dev)) {
		printk("ERROR: USART3 not ready — check app.overlay\n");
		return -1;
	}
	uart_irq_callback_user_data_set(uart_dev, uart_isr, NULL);
	uart_irq_rx_enable(uart_dev);

	printk("PX4 simple MAVLink — USART3 57600 baud (PB10=TX, PB11=RX)\n");

	/* ── 1. wait_heartbeat() ───────────────────────────────── */
	wait_for_heartbeat();

	/* ── 2 & 3. arm_and_takeoff(altitude) ─────────────────── */
	arm_and_takeoff(TAKEOFF_ALT_M);

	/* ── 4. Hover 5 s ──────────────────────────────────────── */
	printk("Hovering...\n");
	sleep_and_poll(5000);   /* time.sleep(5) */

	/* ── 5. land() ─────────────────────────────────────────── */
	land();

	/* ── 6. disarm() ───────────────────────────────────────── */
	disarm();

	printk("Mission complete.\n");
	return 0;
}
