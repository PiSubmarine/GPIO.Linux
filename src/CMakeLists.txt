message("${_build_cmake_local}")
include("${_build_cmake_local}")

add_library(PiSubmarine.GPIO.Linux
        PiSubmarine/GPIO/Linux/Driver.cpp
        PiSubmarine/GPIO/Linux/PinGroup.cpp
)
target_include_directories(PiSubmarine.GPIO.Linux PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../public")
PiSubmarineInitTarget(PiSubmarine.GPIO.Linux)

PiSubmarineAddDependency("https://github.com/PiSubmarine/GPIO.Api" "")
target_link_libraries(PiSubmarine.GPIO.Linux PUBLIC PiSubmarine.GPIO.Api)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED IMPORTED_TARGET libgpiod)
pkg_check_modules(GPIODCXX REQUIRED IMPORTED_TARGET libgpiodcxx)

# target_include_directories(PiSubmarine.GPIO.Linux PUBLIC ${GPIOD_INCLUDE_DIRS})
# target_link_libraries(PiSubmarine.GPIO.Linux PUBLIC ${GPIOD_LIBRARIES})
# target_include_directories(PiSubmarine.GPIO.Linux PUBLIC ${GPIODCXX_INCLUDE_DIRS})
# target_link_libraries(PiSubmarine.GPIO.Linux PUBLIC ${GPIODCXX_LIBRARIES})
target_link_libraries(PiSubmarine.GPIO.Linux PUBLIC
        PkgConfig::GPIODCXX
        PkgConfig::GPIOD
)