#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

int main(void) {
	const struct device *const dev = DEVICE_DT_GET(DT_NODELABEL(accel));

	if (!device_is_ready(dev)) {
		printf("Device %s is not ready\n", dev->name);
		return 0;
	}

	printf("LIS302DL sensor found and ready!\n");

	while (1) {
		struct sensor_value accel_val[3];
		sensor_sample_fetch(dev);
		sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel_val);

		printf("X: %f, Y: %f, Z: %f\n", 
            sensor_value_to_double(&accel_val[0]),
            sensor_value_to_double(&accel_val[1]),
            sensor_value_to_double(&accel_val[2]));

		k_sleep(K_MSEC(500));
	}
	return 0;
}
