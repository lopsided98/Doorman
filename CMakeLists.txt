set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

cmake_minimum_required(VERSION 2.8)

project(Doorman C CXX)

set(ARDUINO_DEFAULT_PORT /dev/ttyACM0)

# Enable recursion for the DigitalIO library
set(DigitalIO_RECURSE True)

generate_arduino_firmware(Doorman
        SRCS Doorman.cpp Lock.cpp StepperLock.cpp Authenticator.cpp
        RFIDAuthenticator.cpp StepperControl.cpp DummyLock.cpp
        HDRS Lock.h StepperLock.h Authenticator.h RFIDAuthenticator.h
        StepperControl.h DummyLock.h
        BOARD uno
        ARDLIBS SoftwareSerial)

# Fix linking order
target_link_libraries(Doorman uno_SoftwareSerial)