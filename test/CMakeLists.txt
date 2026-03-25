cmake_minimum_required(VERSION 3.20)

include(FetchContent)

project(PiSubmarine.GPIO.Linux.Test LANGUAGES CXX)

set(PiSubmarine.GPIO.Linux.Test.Sources
        "PiSubmarine/GPIO/Linux/DriverTest.cpp")

enable_testing()

add_executable(PiSubmarine.GPIO.Linux.Test ${PiSubmarine.GPIO.Linux.Test.Sources})
PiSubmarineInitTarget(PiSubmarine.GPIO.Linux.Test)

target_link_libraries(PiSubmarine.GPIO.Linux.Test PUBLIC PiSubmarine.GPIO.Linux)

find_package(GTest)
target_link_libraries(PiSubmarine.GPIO.Linux.Test PUBLIC GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(PiSubmarine.GPIO.Linux.Test)