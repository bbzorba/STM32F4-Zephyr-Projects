#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <errno.h>

#include "lis302_accel.h"

int main(void)
{
	struct lis_accel_device accel;
	int ret = lis_accel_init(&accel);
	if (ret != 0) {
		printk("Accelerometer init failed (%d)\n", ret);
		return 0;
	}

	printk("Accelerometer WHOAMI=0x%02x (%s)\n", accel.whoami, lis_accel_type_str(accel.type));
	printk("Init OK\n");

	while (1) {
		struct lis_accel_sample s;
		ret = lis_accel_read(&accel, &s);
		if (ret == -EAGAIN) {
			k_sleep(K_MSEC(10));
			continue;
		}
		if (ret != 0) {
			printk("read failed (%d)\n", ret);
			k_sleep(K_MSEC(50));
			continue;
		}

		if (accel.type == LIS_ACCEL_LIS302DL) {
			printk("raw8: x=%d y=%d z=%d\n", (int)s.x, (int)s.y, (int)s.z);
		} else if (accel.type == LIS_ACCEL_LIS3DSH) {
			static uint32_t count;
			if ((count++ % 20U) == 0U) {
				printk("status=0x%02x\n", s.status);
			}
			printk("raw16: x=%d y=%d z=%d | mg: x=%ld y=%ld z=%ld\n",
			       s.x, s.y, s.z,
			       (long)s.x_mg, (long)s.y_mg, (long)s.z_mg);
		} else {
			printk("Unknown accel type\n");
		}

		k_sleep(K_MSEC(50));
	}
}
