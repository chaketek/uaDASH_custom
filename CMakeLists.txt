# ESP-IDF build (Arduino core as a component), see docs/adr/0001-build-with-esp-idf.md
#   idf.py -DBOARD=WAVESHARE_S3_LCD7B build
# Sources are shared with the Arduino IDE / arduino-cli build in firmware/.
cmake_minimum_required(VERSION 3.16)

set(BOARD "WAVESHARE_S3_LCD7B" CACHE STRING "Display board define (see firmware/config.h)")
# optional test defines, e.g. -DAPP_DEFINES="DEMO_DATA;PERF_MONITOR"
set(APP_DEFINES "" CACHE STRING "Extra defines (DEBUG, DEMO_DATA, PERF_MONITOR)")

# board specific sdkconfig on top of the common one
if(NOT DEFINED SDKCONFIG_DEFAULTS)
    set(SDKCONFIG_DEFAULTS "sdkconfig.defaults")
    if(EXISTS "${CMAKE_SOURCE_DIR}/sdkconfig.defaults.${BOARD}")
        list(APPEND SDKCONFIG_DEFAULTS "sdkconfig.defaults.${BOARD}")
    endif()
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# the board define and lv_conf.h are needed by every component (LVGL reads lv_conf.h)
idf_build_set_property(COMPILE_DEFINITIONS "${BOARD}" APPEND)
idf_build_set_property(COMPILE_DEFINITIONS "BOARD_HAS_PSRAM" APPEND)
idf_build_set_property(COMPILE_DEFINITIONS "LV_CONF_INCLUDE_SIMPLE" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/firmware" APPEND)
foreach(def ${APP_DEFINES})
    idf_build_set_property(COMPILE_DEFINITIONS "${def}" APPEND)
endforeach()

project(uaDash)
