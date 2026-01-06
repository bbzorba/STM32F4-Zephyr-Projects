#ifndef LIS302_ACCEL_H_
#define LIS302_ACCEL_H_

#include <stdint.h>

#include <zephyr/kernel.h>
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

/* Scaling assumption for LIS3DSH processed output (mg). */
#define LIS3DSH_LSB_PER_G    16384

enum lis_accel_type {
	LIS_ACCEL_UNKNOWN = 0,
	LIS_ACCEL_LIS302DL,
	LIS_ACCEL_LIS3DSH,
};

struct lis_accel_device {
	struct spi_dt_spec spi;
	enum lis_accel_type type;
	uint8_t whoami;
};

struct lis_accel_sample {
	int16_t x;
	int16_t y;
	int16_t z;
	int32_t x_mg;
	int32_t y_mg;
	int32_t z_mg;
	uint8_t status;
};

int lis_accel_init(struct lis_accel_device *dev);
int lis_accel_read(struct lis_accel_device *dev, struct lis_accel_sample *sample);
const char *lis_accel_type_str(enum lis_accel_type type);

/* Lower-level helper APIs (exported for reuse in other modules). */
int st_spi_read_u8(const struct spi_dt_spec *spi, uint8_t reg, uint8_t *val);
int st_spi_write_u8(const struct spi_dt_spec *spi, uint8_t reg, uint8_t val);
int st_spi_read_u8_retry(const struct spi_dt_spec *spi, uint8_t reg, uint8_t *val, int tries);
int32_t counts_to_mg(int16_t counts);

/* Optional device-specific building blocks (also exported). */
int lis_spi_setup(struct spi_dt_spec *spi_out);
int lis302dl_init(const struct spi_dt_spec *spi);
int lis302dl_read(const struct spi_dt_spec *spi, struct lis_accel_sample *s);
int lis3dsh_init(const struct spi_dt_spec *spi);
int lis3dsh_read(const struct spi_dt_spec *spi, struct lis_accel_sample *s);

#endif /* LIS302_ACCEL_H_ */