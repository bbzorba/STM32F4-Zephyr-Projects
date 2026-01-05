
# Zephyr Workspace Notes

This repository is a Zephyr workspace checkout.

## Development Projects Folder

Local application projects live under:

- `dev/`

Examples in this workspace:

- `dev/LED_Blink/`
- `dev/LIS302_accel_test/`

Each app folder is a normal Zephyr “application” with its own `CMakeLists.txt`, `prj.conf`, and `src/`.

## Create A New Project (Application)

Create a new folder under `dev/` with (minimum) this structure:

```
dev/MyApp/
	CMakeLists.txt
	prj.conf
	src/
		main.c
```

Optional (board/peripheral config):

- `app.overlay` (devicetree overlay)

### Minimal `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(myapp)

target_sources(app PRIVATE src/main.c)
```

### Minimal `prj.conf`

Enable what your app uses (example for console print):

```
CONFIG_STDOUT_CONSOLE=y
```

## Environment Setup

In a new shell, source Zephyr’s environment script before building/flashing:

```sh
source zephyr/zephyr-env.sh
```

This sets `ZEPHYR_BASE` and also prefers the system `openocd` when the Zephyr SDK-bundled one cannot execute on the host.

## Build (Compile)

From your app directory (example uses `stm32f4_disco`):

```sh
cd dev/LED_Blink
west build -b stm32f4_disco . -- -DDTC=/usr/bin/dtc
```

If you change board overlays/Kconfig/build settings and want a clean reconfigure:

```sh
west build -t pristine
west build -b stm32f4_disco . -- -DDTC=/usr/bin/dtc
```

## Flash

For STM32F4 Disco in this setup, use the OpenOCD runner:

```sh
cd dev/LED_Blink
west flash --runner openocd
```

Notes:

- `west flash` without `--runner openocd` may default to `stm32cubeprogrammer` on this board; that requires STM32CubeProgrammer (`STM32_Programmer_CLI`) to be installed and on `PATH`.

