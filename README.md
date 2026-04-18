# STM32F4 Zephyr Workspace

Zephyr RTOS workspace for the STM32F4 Discovery board.

## Project Structure

All user applications live under `applications/`:

```
applications/
├── Intr_Btn_LED_Blink/
├── Intr_Btn_LED_Blink_pressed/
├── LIS302_accel_test/
└── Threaded_Button_LED_Blink/
```

Each app has a standard structure:

```
applications/MyApp/
├── CMakeLists.txt
├── prj.conf
└── src/
    └── main.c
```

Optional: `app.overlay` (devicetree overlay for board/peripheral config).

### Minimal `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(myapp)
target_sources(app PRIVATE src/main.c)
```

### Minimal `prj.conf`

```
CONFIG_STDOUT_CONSOLE=y
```

## Environment Setup (one-time)

### Host Tools

- `python3` + `pip`
- `cmake`
- `ninja` (recommended build tool)
- `git`
- `openocd` (for flashing)

**Linux/macOS:**
```sh
sudo apt install -y python3 python3-pip cmake ninja-build git openocd
```

**Windows:** Install [Python](https://python.org), [CMake](https://cmake.org), [Ninja](https://ninja-build.org), [Git](https://git-scm.com), [OpenOCD](https://openocd.org). The Zephyr SDK provides the ARM toolchain automatically.

### Python Virtual Environment

```sh
python -m venv .venv

# Linux/macOS
source .venv/bin/activate

# Windows (PowerShell)
.venv\Scripts\Activate.ps1

pip install -r requirements.txt
```

### Zephyr SDK (Toolchain)

Download and install the [Zephyr SDK](https://docs.zephyrproject.org/latest/develop/toolchains/zephyr_sdk.html). The build system auto-detects it.

### West Modules (first time or after manifest changes)

```sh
west update
```

## Build & Flash

The application to build is selected via `COMPILE_DIR` at the top of the `Makefile`.
Change that line to switch the default, or override it on the command line.

### Build

```sh
make
make build

# Override application or board:
make build COMPILE_DIR=applications/Threaded_Button_LED_Blink
make build COMPILE_DIR=applications/LIS302_accel_test BOARD=stm32f4_disco
```

### Flash

Connect the STM32F4 Discovery board via USB (ST-LINK), then:

```sh
make flash

# Flash a specific application:
make flash COMPILE_DIR=applications/Threaded_Button_LED_Blink
```

### Build and Flash in One Step

```sh
make build-flash
make build-flash COMPILE_DIR=applications/Intr_Btn_LED_Blink_pressed
```

### Clean Build Directory

```sh
make clean
make clean COMPILE_DIR=applications/LIS302_accel_test
```
