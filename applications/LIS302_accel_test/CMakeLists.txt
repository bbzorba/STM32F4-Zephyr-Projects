cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Helps VS Code IntelliSense by emitting compile_commands.json into the build dir.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

project(lis302dl_test)

target_sources(app PRIVATE
	src/main.c
	src/lis302_accel.c
)

target_include_directories(app PRIVATE inc)
