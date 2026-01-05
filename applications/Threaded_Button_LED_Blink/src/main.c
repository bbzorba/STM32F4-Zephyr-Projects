/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/atomic.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   250

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/* The devicetree node identifier for the "sw0" alias (user button). */
#define SW0_NODE DT_ALIAS(sw0)


/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec leds[] = {
	GPIO_DT_SPEC_GET(LED0_NODE, gpios),
	GPIO_DT_SPEC_GET(LED1_NODE, gpios),
	GPIO_DT_SPEC_GET(LED2_NODE, gpios),
	GPIO_DT_SPEC_GET(LED3_NODE, gpios),
};

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

static atomic_t button_pressed;

#define BUTTON_THREAD_STACK_SIZE 1024
#define LED_THREAD_STACK_SIZE 1024
#define BUTTON_THREAD_PRIORITY 5
#define LED_THREAD_PRIORITY 5

K_THREAD_STACK_DEFINE(button_stack, BUTTON_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(led_stack, LED_THREAD_STACK_SIZE);
static struct k_thread button_thread_data;
static struct k_thread led_thread_data;

/*
 * Button config test matrix.
 * Change BUTTON_MODE (0..N-1), rebuild + flash, then press USER.
 */
#define BUTTON_MODE 1

struct button_mode {
	gpio_flags_t flags;
	bool active_high;
	const char *name;
};

static const struct button_mode button_modes[] = {
	/* Most likely for STM32F4-Discovery B1 on PA0 */
	{ GPIO_INPUT | GPIO_PULL_UP,   false, "PULL_UP + ACTIVE_LOW" },
	{ GPIO_INPUT | GPIO_PULL_DOWN, false, "PULL_DOWN + ACTIVE_LOW" },
	{ GPIO_INPUT,                 false, "NO_PULL + ACTIVE_LOW" },
};

static void set_all_leds(int value)
{
	for (size_t i = 0; i < ARRAY_SIZE(leds); i++) {
		(void)gpio_pin_set_dt(&leds[i], value);
	}
}

static void button_thread(void *unused1, void *unused2, void *unused3)
{
	ARG_UNUSED(unused1);
	ARG_UNUSED(unused2);
	ARG_UNUSED(unused3);

	int last_raw = -1;
	int last_pressed = -1;
	const struct button_mode *mode = &button_modes[BUTTON_MODE];

	while (1) {
		int val = gpio_pin_get_dt(&button);
		int pressed = 0;
		if (val >= 0) {
			pressed = mode->active_high ? (val != 0) : (val == 0);
			atomic_set(&button_pressed, pressed);
		}

		if (val != last_raw || pressed != last_pressed) {
			printf("BUTTON mode=%s raw=%d pressed=%d\n", mode->name, val, pressed);
			last_raw = val;
			last_pressed = pressed;
		}

		k_msleep(20);
	}
}

static void led_thread(void *unused1, void *unused2, void *unused3)
{
	ARG_UNUSED(unused1);
	ARG_UNUSED(unused2);
	ARG_UNUSED(unused3);

	while (1) {
		if (atomic_get(&button_pressed)) {
			set_all_leds(1);
			k_msleep(50);
			continue;
		}

		for (size_t i = 0; i < ARRAY_SIZE(leds); i++) {
			if (atomic_get(&button_pressed)) {
				break;
			}

			(void)gpio_pin_toggle_dt(&leds[i]);
			printf("Toggled LED%u\n", (unsigned)i);
			k_msleep(SLEEP_TIME_MS);
		}
	}
}

int main(void)
{
	int ret;

	for (size_t i = 0; i < ARRAY_SIZE(leds); i++) {
		if (!gpio_is_ready_dt(&leds[i])) {
			printf("LED%u GPIO device not ready\n", (unsigned)i);
			return 0;
		}

		ret = gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
		if (ret < 0) {
			printf("Failed to configure LED%u (err %d)\n", (unsigned)i, ret);
			return 0;
		}
	}

	if (!gpio_is_ready_dt(&button)) {
		printf("Button GPIO device not ready\n");
		return 0;
	}

	BUILD_ASSERT(BUTTON_MODE >= 0 && BUTTON_MODE < ARRAY_SIZE(button_modes),
		     "Invalid BUTTON_MODE");

	const struct button_mode *mode = &button_modes[BUTTON_MODE];
	ret = gpio_pin_configure_dt(&button, mode->flags);
	if (ret < 0) {
		printf("Failed to configure button %s (err %d)\n", mode->name, ret);
		return 0;
	}

	printf("Button on %s pin %u configured: %s (dt_flags=0x%x)\n",
	       button.port->name,
	       (unsigned)button.pin,
	       mode->name,
	       (unsigned)button.dt_flags);

	atomic_set(&button_pressed, 0);

	k_thread_create(&button_thread_data, button_stack,
			K_THREAD_STACK_SIZEOF(button_stack),
			button_thread, NULL, NULL, NULL,
			BUTTON_THREAD_PRIORITY, 0, K_NO_WAIT);

	k_thread_create(&led_thread_data, led_stack,
			K_THREAD_STACK_SIZEOF(led_stack),
			led_thread, NULL, NULL, NULL,
			LED_THREAD_PRIORITY, 0, K_NO_WAIT);

	printf("LED_Blink started. Hold USER button to turn all LEDs on.\n");

	while (1) {
		k_sleep(K_SECONDS(1));
	}
}
