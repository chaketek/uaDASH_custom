# application sources live in firmware/ (shared with the Arduino IDE build)
file(GLOB app_sources "${CMAKE_SOURCE_DIR}/firmware/*.c" "${CMAKE_SOURCE_DIR}/firmware/*.cpp")

idf_component_register(SRCS "main.cpp" ${app_sources}
                       INCLUDE_DIRS "../firmware")

# generated SquareLine/legacy code has warnings, keep them visible but not fatal
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-enum-enum-conversion>)
