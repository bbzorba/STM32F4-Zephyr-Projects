#ifndef LIS302_ACCEL_H_
#define LIS302_ACCEL_H_

#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>

/* SPI command bits used by ST MEMS */
#define ST_SPI_READ     0x80
#define ST_SPI_AUTOINC  0x40

/* LIS302DL (WHOAMI 0x3B) registers (subset) */
#define LIS302DL_WHOAMI     0x0F
#define LIS302DL_CTRL1      0x20
#define LIS302DL_OUT_X      0x29
#define LIS302DL_OUT_Y      0x2B
#define LIS302DL_OUT_Z      0x2D

/* LIS3DSH (WHOAMI 0x3F) registers (subset) */
#define LIS3DSH_WHOAMI      0x0F
#define LIS3DSH_CTRL4       0x20
#define LIS3DSH_CTRL5       0x24
#define LIS3DSH_STATUS      0x27
#define LIS3DSH_OUT_X_L     0x28

/*
 * “Processed” scaling: convert raw counts to milli-g (mg).
 * For LIS3DSH this depends on full-scale; with the current init we assume ±2g.
 * If your numbers look off by a constant factor, adjust this value.
 */
#define LIS3DSH_LSB_PER_G    16384

/* Function prototypes */
int st_spi_read_u8(const struct spi_dt_spec *spi, uint8_t reg, uint8_t *val);
int st_spi_write_u8(const struct spi_dt_spec *spi, uint8_t reg, uint8_t val);
int st_spi_read_u8_retry(const struct spi_dt_spec *spi, uint8_t reg, uint8_t *val, int tries);
int32_t counts_to_mg(int16_t counts);

#endif /* LIS302_ACCEL_H_ */