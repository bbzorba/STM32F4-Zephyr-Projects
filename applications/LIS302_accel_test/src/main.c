#include "lis302_accel.h"


int main(void)
{
	const struct device *const spi_dev = DEVICE_DT_GET(DT_NODELABEL(spi1));
	if (!device_is_ready(spi_dev)) {
		printk("SPI1 is not ready\n");
		return 0;
	}

	const struct gpio_dt_spec cs_gpio = GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(spi1), cs_gpios, 0);
	if (!gpio_is_ready_dt(&cs_gpio)) {
		printk("CS GPIO is not ready\n");
		return 0;
	}

	/* Ensure CS is configured and deasserted (idle high for active-low CS). */
	int ret = gpio_pin_configure_dt(&cs_gpio, GPIO_OUTPUT_INACTIVE);
	if (ret != 0) {
		printk("Failed to configure CS GPIO (%d)\n", ret);
		return 0;
	}

	const struct spi_cs_control cs_ctrl = {
		.gpio = cs_gpio,
		.delay = 5,
		.cs_is_gpio = true,
	};

	/* Bare-metal used a very slow SPI prescaler; start conservative. */
	const struct spi_dt_spec spi = {
		.bus = spi_dev,
		.config = {
			.frequency = 400000,
			.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL | SPI_MODE_CPHA,
			.slave = 0,
			.cs = cs_ctrl,
		},
	};

	uint8_t whoami = 0;
	ret = st_spi_read_u8(&spi, 0x0F, &whoami);
	if (ret != 0) {
		printk("Failed to read WHOAMI (%d)\n", ret);
		return 0;
	}

	printk("Accelerometer WHOAMI=0x%02x\n", whoami);

	if (whoami == 0x3B) {
		/* LIS302DL */
		ret = st_spi_write_u8(&spi, LIS302DL_CTRL1, 0x47);
		if (ret != 0) {
			printk("Failed to init LIS302DL (%d)\n", ret);
			return 0;
		}
		printk("LIS302DL init OK\n");

		while (1) {
			uint8_t x_u = 0, y_u = 0, z_u = 0;
			(void)st_spi_read_u8(&spi, LIS302DL_OUT_X, &x_u);
			(void)st_spi_read_u8(&spi, LIS302DL_OUT_Y, &y_u);
			(void)st_spi_read_u8(&spi, LIS302DL_OUT_Z, &z_u);
			printk("raw8: x=%d y=%d z=%d\n", (int8_t)x_u, (int8_t)y_u, (int8_t)z_u);
			k_sleep(K_MSEC(50));
		}
	}

	if (whoami == 0x3F) {
		/* LIS3DSH (this is what your board is actually reporting) */
		ret = st_spi_write_u8(&spi, LIS3DSH_CTRL4, 0x6F);
		if (ret != 0) {
			printk("Failed to write LIS3DSH CTRL4 (%d)\n", ret);
			return 0;
		}
		(void)st_spi_write_u8(&spi, LIS3DSH_CTRL5, 0x00);
		printk("LIS3DSH init OK\n");

		while (1) {
			uint8_t status = 0;
			ret = st_spi_read_u8_retry(&spi, LIS3DSH_STATUS, &status, 3);
			if (ret != 0) {
				/* Treat as a transient bus glitch; don't print status=0xFF noise. */
				k_sleep(K_MSEC(10));
				continue;
			}
			/* ZYXDA (bit3) indicates new XYZ data available. */
			if ((status & 0x08) == 0) {
				k_sleep(K_MSEC(10));
				continue;
			}

			uint8_t xl = 0, xh = 0, yl = 0, yh = 0, zl = 0, zh = 0;
			ret = 0;
			ret |= st_spi_read_u8(&spi, LIS3DSH_OUT_X_L, &xl);
			ret |= st_spi_read_u8(&spi, LIS3DSH_OUT_X_L + 1, &xh);
			ret |= st_spi_read_u8(&spi, LIS3DSH_OUT_X_L + 2, &yl);
			ret |= st_spi_read_u8(&spi, LIS3DSH_OUT_X_L + 3, &yh);
			ret |= st_spi_read_u8(&spi, LIS3DSH_OUT_X_L + 4, &zl);
			ret |= st_spi_read_u8(&spi, LIS3DSH_OUT_X_L + 5, &zh);
			if (ret != 0) {
				k_sleep(K_MSEC(10));
				continue;
			}

			int16_t x = (int16_t)((uint16_t)xh << 8 | xl);
			int16_t y = (int16_t)((uint16_t)yh << 8 | yl);
			int16_t z = (int16_t)((uint16_t)zh << 8 | zl);
			int32_t x_mg = counts_to_mg(x);
			int32_t y_mg = counts_to_mg(y);
			int32_t z_mg = counts_to_mg(z);

			static uint32_t count;
			if ((count++ % 20U) == 0U) {
				printk("status=0x%02x\n", status);
			}
			printk("raw16: x=%d y=%d z=%d | mg: x=%ld y=%ld z=%ld\n",
			       x, y, z,
			       (long)x_mg, (long)y_mg, (long)z_mg);
			k_sleep(K_MSEC(50));
		}
	}

	printk("Unexpected WHOAMI=0x%02x.\n", whoami);
	return 0;
}
