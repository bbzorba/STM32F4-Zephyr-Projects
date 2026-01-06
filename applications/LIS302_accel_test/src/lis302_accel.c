#include "lis302_accel.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include <errno.h>

int st_spi_read_u8(const struct spi_dt_spec *spi, uint8_t reg, uint8_t *val)
{
	uint8_t tx[2] = { (uint8_t)(reg | ST_SPI_READ), 0x00 };
	uint8_t rx[2] = { 0 };
	struct spi_buf tx_buf = { .buf = tx, .len = sizeof(tx) };
	struct spi_buf rx_buf = { .buf = rx, .len = sizeof(rx) };
	struct spi_buf_set tx_set = { .buffers = &tx_buf, .count = 1 };
	struct spi_buf_set rx_set = { .buffers = &rx_buf, .count = 1 };
	int ret = spi_transceive_dt(spi, &tx_set, &rx_set);
	if (ret != 0) {
		return ret;
	}
	*val = rx[1];
	return 0;
}

int st_spi_write_u8(const struct spi_dt_spec *spi, uint8_t reg, uint8_t val)
{
	uint8_t tx[2] = { (uint8_t)(reg & (uint8_t)~ST_SPI_READ), val };
	struct spi_buf tx_buf = { .buf = tx, .len = sizeof(tx) };
	struct spi_buf_set tx_set = { .buffers = &tx_buf, .count = 1 };
	return spi_write_dt(spi, &tx_set);
}

int st_spi_read_u8_retry(const struct spi_dt_spec *spi, uint8_t reg, uint8_t *val, int tries)
{
	int ret;
	uint8_t v = 0;

	for (int i = 0; i < tries; i++) {
		ret = st_spi_read_u8(spi, reg, &v);
		if (ret != 0) {
			k_busy_wait(10);
			continue;
		}
		/* 0xFF is a common “bad read” signature on SPI (MISO pulled high). */
		if (v != 0xFF) {
			*val = v;
			return 0;
		}
		k_busy_wait(10);
	}

	*val = v;
	return -EIO;
}

int32_t counts_to_mg(int16_t counts)
{
	return ((int32_t)counts * 1000) / LIS3DSH_LSB_PER_G;
}

int lis_spi_setup(struct spi_dt_spec *spi_out)
{
	const struct device *const spi_dev = DEVICE_DT_GET(DT_NODELABEL(spi1));
	if (!device_is_ready(spi_dev)) {
		return -ENODEV;
	}

	const struct gpio_dt_spec cs_gpio =
		GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(spi1), cs_gpios, 0);
	if (!gpio_is_ready_dt(&cs_gpio)) {
		return -ENODEV;
	}

	int ret = gpio_pin_configure_dt(&cs_gpio, GPIO_OUTPUT_INACTIVE);
	if (ret != 0) {
		return ret;
	}

	const struct spi_cs_control cs_ctrl = {
		.gpio = cs_gpio,
		.delay = 5,
		.cs_is_gpio = true,
	};

	/* Conservative SPI settings for reliability. */
	*spi_out = (struct spi_dt_spec){
		.bus = spi_dev,
		.config = {
			.frequency = 400000,
			.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB |
					SPI_MODE_CPOL | SPI_MODE_CPHA,
			.slave = 0,
			.cs = cs_ctrl,
		},
	};

	return 0;
}

int lis302dl_init(const struct spi_dt_spec *spi)
{
	return st_spi_write_u8(spi, LIS302DL_CTRL1, 0x47);
}

int lis302dl_read(const struct spi_dt_spec *spi, struct lis_accel_sample *s)
{
	uint8_t x_u = 0, y_u = 0, z_u = 0;
	int ret = 0;
	ret |= st_spi_read_u8(spi, LIS302DL_OUT_X, &x_u);
	ret |= st_spi_read_u8(spi, LIS302DL_OUT_Y, &y_u);
	ret |= st_spi_read_u8(spi, LIS302DL_OUT_Z, &z_u);
	if (ret != 0) {
		return -EIO;
	}

	s->x = (int16_t)(int8_t)x_u;
	s->y = (int16_t)(int8_t)y_u;
	s->z = (int16_t)(int8_t)z_u;
	s->x_mg = 0;
	s->y_mg = 0;
	s->z_mg = 0;
	s->status = 0;
	return 0;
}

int lis3dsh_init(const struct spi_dt_spec *spi)
{
	int ret = st_spi_write_u8(spi, LIS3DSH_CTRL4, 0x6F);
	if (ret != 0) {
		return ret;
	}
	return st_spi_write_u8(spi, LIS3DSH_CTRL5, 0x00);
}

int lis3dsh_read(const struct spi_dt_spec *spi, struct lis_accel_sample *s)
{
	uint8_t status = 0;
	int ret = st_spi_read_u8_retry(spi, LIS3DSH_STATUS, &status, 3);
	if (ret != 0) {
		return ret;
	}
	/* ZYXDA (bit3) indicates new XYZ data available. */
	if ((status & 0x08) == 0) {
		return -EAGAIN;
	}

	uint8_t xl = 0, xh = 0, yl = 0, yh = 0, zl = 0, zh = 0;
	ret = 0;
	ret |= st_spi_read_u8(spi, LIS3DSH_OUT_X_L, &xl);
	ret |= st_spi_read_u8(spi, LIS3DSH_OUT_X_L + 1, &xh);
	ret |= st_spi_read_u8(spi, LIS3DSH_OUT_X_L + 2, &yl);
	ret |= st_spi_read_u8(spi, LIS3DSH_OUT_X_L + 3, &yh);
	ret |= st_spi_read_u8(spi, LIS3DSH_OUT_X_L + 4, &zl);
	ret |= st_spi_read_u8(spi, LIS3DSH_OUT_X_L + 5, &zh);
	if (ret != 0) {
		return -EIO;
	}

	s->x = (int16_t)((uint16_t)xh << 8 | xl);
	s->y = (int16_t)((uint16_t)yh << 8 | yl);
	s->z = (int16_t)((uint16_t)zh << 8 | zl);
	s->x_mg = counts_to_mg(s->x);
	s->y_mg = counts_to_mg(s->y);
	s->z_mg = counts_to_mg(s->z);
	s->status = status;
	return 0;
}

const char *lis_accel_type_str(enum lis_accel_type type)
{
	switch (type) {
	case LIS_ACCEL_LIS302DL:
		return "LIS302DL";
	case LIS_ACCEL_LIS3DSH:
		return "LIS3DSH";
	default:
		return "UNKNOWN";
	}
}

int lis_accel_init(struct lis_accel_device *dev)
{
	if (dev == NULL) {
		return -EINVAL;
	}

	int ret = lis_spi_setup(&dev->spi);
	if (ret != 0) {
		dev->type = LIS_ACCEL_UNKNOWN;
		dev->whoami = 0;
		return ret;
	}

	uint8_t whoami = 0;
	ret = st_spi_read_u8(&dev->spi, 0x0F, &whoami);
	if (ret != 0) {
		dev->type = LIS_ACCEL_UNKNOWN;
		dev->whoami = 0;
		return ret;
	}
	dev->whoami = whoami;

	if (whoami == 0x3B) {
		dev->type = LIS_ACCEL_LIS302DL;
		return lis302dl_init(&dev->spi);
	}
	if (whoami == 0x3F) {
		dev->type = LIS_ACCEL_LIS3DSH;
		return lis3dsh_init(&dev->spi);
	}

	dev->type = LIS_ACCEL_UNKNOWN;
	return -ENODEV;
}

int lis_accel_read(struct lis_accel_device *dev, struct lis_accel_sample *sample)
{
	if (dev == NULL || sample == NULL) {
		return -EINVAL;
	}

	sample->x = 0;
	sample->y = 0;
	sample->z = 0;
	sample->x_mg = 0;
	sample->y_mg = 0;
	sample->z_mg = 0;
	sample->status = 0;

	switch (dev->type) {
	case LIS_ACCEL_LIS302DL:
		return lis302dl_read(&dev->spi, sample);
	case LIS_ACCEL_LIS3DSH:
		return lis3dsh_read(&dev->spi, sample);
	default:
		return -ENODEV;
	}
}