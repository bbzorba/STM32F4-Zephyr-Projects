# ============================================================
# STM32F4 Zephyr Workspace - Makefile
# Platform-independent: Windows, Linux, macOS
#
# Select the application to build below, then:
#   make              -> build
#   make build
#   make flash
#   make build-flash
#   make clean
# ============================================================

# --- Select application to build (uncomment one) ---
#COMPILE_DIR ?= applications/Intr_Btn_LED_Blink_pressed
#COMPILE_DIR ?= applications/Intr_Btn_LED_Blink
#COMPILE_DIR ?= applications/Threaded_Button_LED_Blink
#COMPILE_DIR ?= applications/LIS302_accel_test
#COMPILE_DIR ?= applications/PX4_Takeoff_and_Land
COMPILE_DIR ?= applications/PX4_Takeoff_and_Land_simple

BOARD     ?= stm32f4_disco
BUILD_DIR ?= $(COMPILE_DIR)/build
BAUD      ?= 115200
PORT      ?=

# Python from the workspace virtual environment (platform-detected)
ifeq ($(OS),Windows_NT)
    PYTHON := .venv/Scripts/python.exe
else
    PYTHON := .venv/bin/python
endif

# ============================================================
.DEFAULT_GOAL := build
.PHONY: help build flash clean build-flash update debug monitor flashmonitor-auto _gen-debug-context

help:
	@echo Usage: make [build, flash, clean, build-flash, update, debug, monitor, flashmonitor-auto] [COMPILE_DIR=...] [BOARD=...]
	@echo   build       - Build the selected application
	@echo   flash       - Flash to STM32F4 Discovery board
	@echo   clean       - Remove build directory
	@echo   build-flash - Build then flash
	@echo   update      - Update Zephyr and dependencies
	@echo   debug       - Build then open VS Code debug session (press F5)
	@echo   monitor     - Open serial monitor
	@echo   flashmonitor-auto - Build, flash, then open serial monitor

	@echo COMPILE_DIR=$(COMPILE_DIR)
	@echo BOARD=$(BOARD)

build:
	$(PYTHON) -m west build -b $(BOARD) $(COMPILE_DIR) -d $(BUILD_DIR) --pristine=auto

flash:
	$(PYTHON) -m west flash -d $(BUILD_DIR)

clean:
	$(PYTHON) -c "import shutil; shutil.rmtree('$(BUILD_DIR)', ignore_errors=True); print('Cleaned: $(BUILD_DIR)')"

update:
	$(PYTHON) -m pip install --upgrade pip
	$(PYTHON) -m pip install --upgrade west
	$(PYTHON) -m west update

debug: clean build
	@echo ""
	@echo ">>> Build ready. Press F5 in VS Code to start the debug session."

monitor:
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -ExecutionPolicy Bypass -File "tools/monitor.ps1" $(if $(PORT),-ComPort $(PORT),) -Baud $(BAUD) $(if $(MONITOR_SECONDS),-DurationSec $(MONITOR_SECONDS),)
else
	@echo ">>> Opening serial monitor..."
	$(PYTHON) -m minicom -D /dev/ttyUSB0 -b 115200
endif

build-flash: build flash

flashmonitor-auto: build flash monitor

_gen-debug-context:
	$(PYTHON) .vscode/gen_debug_context.py