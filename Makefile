SHELL := bash
.ONESHELL:
.DEFAULT_GOAL := build

# Root build/flash entrypoint for this Zephyr workspace.
#
# Select which application to build with COMPILE_DIR, e.g.:
#   make build COMPILE_DIR=applications/Threaded_Button_LED_Blink
#
# Choose how to build with METHOD:
#   METHOD=west        (default)
#   METHOD=cmake-ninja
#   METHOD=cmake-make
#
# Choose how to flash with FLASH_METHOD:
#   FLASH_METHOD=west     (default)
#   FLASH_METHOD=openocd  (direct OpenOCD, no west)

#COMPILE_DIR ?= applications/Threaded_Button_LED_Blink
COMPILE_DIR ?= applications/LIS302_accel_test
BOARD ?= stm32f4_disco

# Preferred interface:
#   Build:  make build METHOD=west|cmake-ninja|cmake-make
#   Flash:  make flash METHOD=west|openocd
#
# Under the hood we keep separate BUILD_METHOD/FLASH_METHOD, but accept METHOD
# as a convenience alias to reduce typing.
METHOD ?=

# Force system dtc (SDK dtc may not be runnable on this host)
DTC ?= /usr/bin/dtc

# OpenOCD direct-flash defaults
OPENOCD ?= /usr/bin/openocd
OPENOCD_DEFAULT_PATH ?= /usr/share/openocd/scripts
OPENOCD_BOARD_CFG ?= board/stm32f4discovery.cfg

# west build options
PRISTINE ?= 0

# Serial monitor defaults
COMPORT ?= /dev/ttyUSB0
BAUD ?= 115200

## Resolve METHOD alias depending on goal(s)
GOALS := $(strip $(MAKECMDGOALS))
HAS_FLASH := $(filter flash build-flash,$(GOALS))
HAS_BUILD := $(filter build build-flash,$(GOALS))

ifeq ($(HAS_FLASH),)
	# No flash goal: treat METHOD as a build-method alias.
	ifneq ($(METHOD),)
		ifeq ($(BUILD_METHOD),)
			BUILD_METHOD := $(METHOD)
		endif
	endif
else
	# Flash goal present: if METHOD is a flash method, treat it as such.
	ifneq ($(METHOD),)
		ifneq ($(filter west openocd,$(METHOD)),)
			ifeq ($(FLASH_METHOD),)
				FLASH_METHOD := $(METHOD)
			endif
		else
			# Otherwise treat METHOD as a build-method alias.
			ifeq ($(BUILD_METHOD),)
				BUILD_METHOD := $(METHOD)
			endif
		endif
	endif
endif

# Defaults (treat unset/empty as west)
ifeq ($(strip $(BUILD_METHOD)),)
BUILD_METHOD := west
endif
ifeq ($(strip $(FLASH_METHOD)),)
FLASH_METHOD := west
endif

ifeq ($(BUILD_METHOD),west)
BUILD_DIR ?= $(COMPILE_DIR)/build
else ifeq ($(BUILD_METHOD),cmake-ninja)
BUILD_DIR ?= $(COMPILE_DIR)/build-cmake
else ifeq ($(BUILD_METHOD),cmake-make)
BUILD_DIR ?= $(COMPILE_DIR)/build-cmake-make
else
$(error Unknown BUILD_METHOD '$(BUILD_METHOD)'; use west|cmake-ninja|cmake-make)
endif

define _banner
	@echo "==> $(1)"
endef

.PHONY: help
help:
	@echo "Usage: make <target> COMPILE_DIR=path/to/app [BOARD=...] [METHOD=...] [FLASH_METHOD=...]"
	@echo
	@echo "Targets:"
	@echo "  build        Build selected app"
	@echo "  flash        Flash selected app"
	@echo "  build-flash  Build then flash"
	@echo "  monitor      Open serial monitor (USB UART/TTL)"
	@echo "  flashmonitor-auto  Flash then open serial monitor (auto COMPORT)"
	@echo "  clean        Remove BUILD_DIR"
	@echo
	@echo "Key variables:"
	@echo "  COMPILE_DIR   App directory (default: $(COMPILE_DIR))"
	@echo "  BOARD         Zephyr board (default: $(BOARD))"
	@echo "  METHOD        Alias for BUILD_METHOD (build) or FLASH_METHOD (flash)"
	@echo "  BUILD_METHOD  west | cmake-ninja | cmake-make (default: $(BUILD_METHOD))"
	@echo "  BUILD_DIR     Override build directory (default: $(BUILD_DIR))"
	@echo "  FLASH_METHOD  west | openocd (default: $(FLASH_METHOD))"
	@echo "  PRISTINE      1 to force west pristine build (default: $(PRISTINE))"
	@echo "  COMPORT       Serial port path (default: $(COMPORT))"
	@echo "  BAUD          Serial baud rate (default: $(BAUD))"
	@echo
	@echo "Examples:"
	@echo "  make build COMPILE_DIR=applications/Threaded_Button_LED_Blink"
	@echo "  make flash COMPILE_DIR=applications/Threaded_Button_LED_Blink"
	@echo "  make build METHOD=cmake-make COMPILE_DIR=applications/Threaded_Button_LED_Blink"
	@echo "  make flash METHOD=openocd BUILD_METHOD=cmake-make COMPILE_DIR=applications/Threaded_Button_LED_Blink"
	@echo "  make monitor COMPORT=/dev/ttyUSB0"

.PHONY: _env
_env:
	set -euo pipefail
	source ./zephyr/zephyr-env.sh >/dev/null 2>&1 || true

.PHONY: clean
clean:
	$(call _banner,Removing $(BUILD_DIR))
	rm -rf "$(BUILD_DIR)"

.PHONY: build
build: _env
	set -euo pipefail
	$(call _banner,Building $(COMPILE_DIR) (BUILD_METHOD=$(BUILD_METHOD)) into $(BUILD_DIR))
	if [[ ! -f "$(COMPILE_DIR)/CMakeLists.txt" ]]; then
		echo "ERROR: COMPILE_DIR does not look like a Zephyr app: $(COMPILE_DIR)" >&2
		exit 2
	fi

	case "$(BUILD_METHOD)" in \
		west)
			pristine_flag=""
			if [[ "$(PRISTINE)" == "1" ]]; then pristine_flag="-p always"; fi
			west build -b "$(BOARD)" "$(COMPILE_DIR)" -d "$(BUILD_DIR)" $$pristine_flag -- -DDTC="$(DTC)" ;;
		cmake-ninja)
			cmake -S "$(COMPILE_DIR)" -B "$(BUILD_DIR)" -GNinja -DBOARD="$(BOARD)" -DDTC="$(DTC)"
			cmake --build "$(BUILD_DIR)" ;;
		cmake-make)
			cmake -S "$(COMPILE_DIR)" -B "$(BUILD_DIR)" -G "Unix Makefiles" -DBOARD="$(BOARD)" -DDTC="$(DTC)"
			cmake --build "$(BUILD_DIR)" ;;
	esac

.PHONY: flash
flash: _env
	set -euo pipefail
	$(call _banner,Flashing $(COMPILE_DIR) (FLASH_METHOD=$(FLASH_METHOD)) from $(BUILD_DIR))
	if [[ "$(FLASH_METHOD)" == "west" ]]; then
		west flash -d "$(BUILD_DIR)" --runner openocd
		exit 0
	fi

	# Direct OpenOCD (no west)
	elf="$(BUILD_DIR)/zephyr/zephyr.elf"
	if [[ ! -f "$$elf" ]]; then
		echo "ERROR: ELF not found: $$elf" >&2
		echo "Hint: run 'make build ...' first, or set BUILD_DIR=..." >&2
		exit 2
	fi
	"$(OPENOCD)" -s "$(OPENOCD_DEFAULT_PATH)" -f "$(OPENOCD_BOARD_CFG)" -c "program $$elf verify reset exit"

.PHONY: build-flash
build-flash: build flash
	@true

.PHONY: monitor
monitor: _env
	set -euo pipefail
	port="$(COMPORT)"
	# If the default doesn't exist and the user didn't explicitly override COMPORT,
	# try a lightweight auto-detect.
	if [[ "$(COMPORT)" == "/dev/ttyUSB0" && ! -e "$$port" ]]; then
		shopt -s nullglob
		candidates=(/dev/ttyUSB*)
		if (( $${#candidates[@]} )); then port="$${candidates[0]}"; fi
		if [[ -z "$$port" ]]; then
			candidates=(/dev/ttyACM*)
			if (( $${#candidates[@]} )); then port="$${candidates[0]}"; fi
		fi
	fi

	if [[ -z "$$port" ]]; then
		echo "ERROR: No serial port found. Set COMPORT=/dev/ttyUSB0 (or similar)." >&2
		exit 2
	fi

	$(call _banner,Monitoring $$port at $(BAUD) baud (Ctrl-C to exit))
	if ! command -v minicom >/dev/null 2>&1; then
		echo "ERROR: minicom not found. Install it (e.g. 'sudo apt install minicom')." >&2
		exit 2
	fi
	exec minicom -D "$$port" -b "$(BAUD)"

.PHONY: flashmonitor-auto
flashmonitor-auto:
	set -euo pipefail
	$(MAKE) flash
	$(MAKE) monitor COMPORT="$(COMPORT)" BAUD="$(BAUD)"
