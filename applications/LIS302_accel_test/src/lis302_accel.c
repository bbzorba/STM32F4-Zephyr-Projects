#include "lis302_accel.h"

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