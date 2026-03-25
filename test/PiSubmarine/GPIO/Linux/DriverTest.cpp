#include <gtest/gtest.h>
#include "PiSubmarine/GPIO/Linux/Driver.h"
#include "PiSubmarine/GPIO/Linux/PinGroup.h"

#include <cstdint>

namespace PiSubmarine::GPIO::Linux
{
    TEST(DriverTest, GetGpioChips)
    {
        // Driver driver{"PiSubmarine"};
        auto chipPaths = Driver::GetGpioChips();
        EXPECT_GT(chipPaths.size(), 0);
    }

    TEST(DriverTest, RequestInputPins)
    {
        auto chipPaths = Driver::GetGpioChips();
        EXPECT_GT(chipPaths.size(), 0);

        Driver driver{"PiSubmarine"};
        auto pinGroup = driver.CreatePinGroup(chipPaths[0], {2, 4, 6});
    }
}