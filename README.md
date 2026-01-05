
# Zephyr Workspace Notes

This repository is a Zephyr workspace checkout.

## Development Projects Folder

Local application projects live under:

- `dev/`

Some apps in this workspace also live under:

- `applications/`

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

## One Makefile To Rule Them All

There is a root `Makefile` that can build/flash any app by setting `COMPILE_DIR`.

Examples:

```sh
# Build + flash via west (default)
make build-flash COMPILE_DIR=applications/Threaded_Button_LED_Blink

# Build options
make build METHOD=west
make build METHOD=cmake-make
make build METHOD=cmake-ninja

# Force a pristine rebuild with west
make build-flash COMPILE_DIR=applications/Threaded_Button_LED_Blink PRISTINE=1

# Flash options
make flash METHOD=west
make flash METHOD=openocd

# Monitor serial output (UART over USB)
make monitor COMPORT=/dev/ttyUSB0

# Flash then automatically open the serial monitor (auto-detects COMPORT if not set)
make flashmonitor-auto
```

Defaults:

- If you do not specify `METHOD`, `BUILD_METHOD`, or `FLASH_METHOD`, then `make build` and `make flash` both use `west`.
- `make build-flash` also defaults to `west` for both build and flash.

Key variables:

- `COMPILE_DIR`: which app to build
- `BOARD`: which Zephyr board to target (default: `stm32f4_disco`)
- `METHOD`: for `make build`, use `west|cmake-ninja|cmake-make`; for `make flash`, use `west|openocd`
- `BUILD_METHOD`: optional explicit build method (same options as above)
- `FLASH_METHOD`: optional explicit flash method (`west|openocd`)
- `COMPORT`: serial port device (e.g. `/dev/ttyACM0`). If not set, the Makefile tries to auto-detect.
- `COMPORT`: serial port device (default `/dev/ttyUSB0`)
- `BAUD`: serial baud rate (default `115200`)

Note: if you build with `METHOD=cmake-make` (or `cmake-ninja`) and want to flash that output, run:

```sh
make flash METHOD=openocd BUILD_METHOD=cmake-make
```

## Build (Compile)

From your app directory (example uses `stm32f4_disco`):

```sh
cd dev/LED_Blink
west build -b stm32f4_disco . -- -DDTC=/usr/bin/dtc
```

## Build Without `west` (Plain CMake)

You can configure and build a Zephyr application with plain CMake.

Important: in this workspace, force system `dtc` (`/usr/bin/dtc`) because the Zephyr-SDK bundled host `dtc` may not be runnable on this machine.

### Ninja

```sh
source zephyr/zephyr-env.sh
cmake -S ./applications/Threaded_Button_LED_Blink -B ./build-ninja -GNinja -DBOARD=stm32f4_disco -DDTC=/usr/bin/dtc
cmake --build ./build-ninja
```

### GNU Make

```sh
source zephyr/zephyr-env.sh
cmake -S ./applications/Threaded_Button_LED_Blink -B ./build-make -G "Unix Makefiles" -DBOARD=stm32f4_disco -DDTC=/usr/bin/dtc
cmake --build ./build-make
```

## Flash

For STM32F4 Disco in this setup, use the OpenOCD runner:

```sh
cd dev/LED_Blink
west flash --runner openocd
```

## Flash Without `west` (OpenOCD Direct)

Use the root Makefile:

```sh
# Flash the currently selected build output with OpenOCD directly (no west)
make flash METHOD=openocd

# If you built with cmake-make and want to ensure the build dir matches:
make flash METHOD=openocd BUILD_METHOD=cmake-make
```

Notes:

- `west flash` without `--runner openocd` may default to `stm32cubeprogrammer` on this board; that requires STM32CubeProgrammer (`STM32_Programmer_CLI`) to be installed and on `PATH`.

