# Install script for directory: /home/bbzorba/zephyrproject/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/home/bbzorba/zephyr-sdk-0.16.5/arm-zephyr-eabi/bin/arm-zephyr-eabi-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/zephyr/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/zephyr/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/zephyr/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/zephyr/boards/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/zephyr/subsys/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/zephyr/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/acpica/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/cmsis/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/cmsis-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/cmsis-nn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/cmsis_6/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/adi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_afbr/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_ambiq/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/atmel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_bouffalolab/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_espressif/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_ethos_u/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_gigadevice/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_infineon/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_intel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/microchip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_nordic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/nuvoton/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_nxp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/openisa/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/quicklogic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_realtek/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_renesas/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_rpi_pico/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_sifli/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_silabs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_st/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_stm32/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_tdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_telink/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/ti/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_wch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hal_wurthelektronik/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/xtensa/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/hostap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/liblc3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/libmctp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/libmetal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/libsbc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/lora-basics-modem/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/loramac-node/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/lvgl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/mcuboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/mipi-sys-t/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/nanopb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/nrf_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/open-amp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/percepio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/picolibc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/segger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/trusted-firmware-a/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/trusted-firmware-m/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/uoscore-uedhoc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/zcbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/modules/nrf_hw_models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/zephyr/kernel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/zephyr/cmake/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/zephyr/cmake/usage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/bbzorba/zephyrproject/applications/Threaded_Button_LED_Blink/build-cmake-make/zephyr/cmake/reports/cmake_install.cmake")
endif()

