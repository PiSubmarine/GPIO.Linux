#pragma once

#include <string>
#include <cstdint>
#include <filesystem>
#include <optional>

namespace PiSubmarine::GPIO::Linux
{
    struct PinInfo
    {
        std::optional<std::string> UserName;
        std::string SystemName;
        std::filesystem::path DevicePath;
        std::size_t Line;
    };
}
