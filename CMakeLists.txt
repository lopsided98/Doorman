set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

cmake_minimum_required(VERSION 2.8)

project(Doorman C CXX)

set(ARDUINO_DEFAULT_PORT /dev/ttyUSB0)

generate_arduino_library(AMIS30543
                         SRCS AMIS30543/AMIS30543.cpp)

generate_arduino_firmware(Doorman
                          SRCS Doorman.cpp Lock.cpp
                          LIBS AMIS30543
                          BOARD uno)
