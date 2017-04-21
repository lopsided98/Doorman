set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

cmake_minimum_required(VERSION 2.8)

project(Doorman C CXX)

set(ARDUINO_DEFAULT_PORT /dev/ttyACM0)

generate_arduino_firmware(Doorman
        SRCS Doorman.cpp Lock.cpp StepperLock.cpp Authenticator.cpp
        HDRS Lock.h StepperLock.h Authenticator.h
        BOARD uno)
