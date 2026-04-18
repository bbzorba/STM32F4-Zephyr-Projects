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
COMPILE_DIR ?= applications/Intr_Btn_LED_Blink
#COMPILE_DIR ?= applications/Threaded_Button_LED_Blink
#COMPILE_DIR ?= applications/LIS302_accel_test

BOARD     ?= stm32f4_disco
BUILD_DIR ?= $(COMPILE_DIR)/build

# Python from the workspace virtual environment (platform-detected)
ifeq ($(OS),Windows_NT)
    PYTHON := .venv/Scripts/python.exe
else
    PYTHON := .venv/bin/python
endif

# ============================================================
.DEFAULT_GOAL := build
.PHONY: help build flash clean build-flash

help:
	@echo Usage: make [build, flash, clean, build-flash] [COMPILE_DIR=...] [BOARD=...]
	@echo   build       - Build the selected application
	@echo   flash       - Flash to STM32F4 Discovery board
	@echo   clean       - Remove build directory
	@echo   build-flash - Build then flash
	@echo COMPILE_DIR=$(COMPILE_DIR)
	@echo BOARD=$(BOARD)

build:
	$(PYTHON) -m west build -b $(BOARD) $(COMPILE_DIR) -d $(BUILD_DIR)

flash:
	$(PYTHON) -m west flash -d $(BUILD_DIR)

clean:
	$(PYTHON) -c "import shutil; shutil.rmtree('$(BUILD_DIR)', ignore_errors=True); print('Cleaned: $(BUILD_DIR)')"

build-flash: build flash
