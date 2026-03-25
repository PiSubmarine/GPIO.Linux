#include "PiSubmarine/GPIO/Linux/Driver.h"
#include <gpiod.hpp>

namespace PiSubmarine::GPIO::Linux
{
    Driver::Driver(std::string_view consumerName) : m_ConsumerName(consumerName)
    {
        const auto chipPaths = GetGpioChips();

        for (const auto& chipPath : chipPaths)
        {
            gpiod::chip chip(chipPath);

            const size_t linesNum = chip.get_info().num_lines();

            for (size_t lineIndex = 0; lineIndex < linesNum; ++lineIndex)
            {
                const auto& lineInfo = chip.get_line_info(lineIndex);
                PinInfo pinInfo;
                pinInfo.DevicePath = chipPath;
                pinInfo.Line = lineInfo.offset();
                pinInfo.SystemName = lineInfo.name();
                m_Pins.push_back(pinInfo);
            }
        }
    }

    std::shared_ptr<Api::IPinGroup> Driver::GetPinGroup(std::string_view groupName)
    {
        return nullptr;
    }

    std::shared_ptr<Api::IPinGroup> Driver::CreatePinGroup(std::string_view groupName, const std::filesystem::path& chipPath,
                                                           const std::vector<std::size_t>& pins)
    {
        for (const auto& group: m_PinGroups)
        {
            if (group->GetName() == groupName)
            {
                return group;
            }
        }

        std::vector<gpiod::line::offset> offsets;
        offsets.reserve(pins.size());
        for (const auto& pin : pins)
        {
            offsets.emplace_back(pin);
        }

        auto chip = GetChip(chipPath);
        auto group = std::make_shared<PinGroup>(*chip, offsets, groupName);
        m_PinGroups.push_back(group);
        return group;
    }

    bool Driver::GetPinInfo(std::string_view userName, PinInfo& outInfo) const
    {
        for (const auto& pinInfo : m_Pins)
        {
            if (pinInfo.UserName == userName)
            {
                outInfo = pinInfo;
                return true;
            }
        }

        return false;
    }

    PinInfo& Driver::GetPinInfo(const std::filesystem::path& devicePath, std::size_t line)
    {
        for (auto& pinInfo : m_Pins)
        {
            if (pinInfo.DevicePath == devicePath && pinInfo.Line == line)
            {
                return pinInfo;
            }
        }

        throw std::invalid_argument("PinInfo not found: " + devicePath.string() + "[" + std::to_string(line) + "]");
    }

    std::shared_ptr<gpiod::chip> Driver::GetChip(const std::filesystem::path& devicePath)
    {
        for (const auto& chip : m_Chips)
        {
            auto chipPathCanonical = std::filesystem::weakly_canonical(chip->path());
            auto devicePathCanonical = std::filesystem::weakly_canonical(devicePath);
            if (chipPathCanonical == devicePathCanonical)
            {
                return chip;
            }
        }

        auto chip = std::make_shared<gpiod::chip>(devicePath);
        m_Chips.push_back(chip);
        return chip;
    }

    std::string_view Driver::GetConsumerName() const
    {
        return m_ConsumerName;
    }

    std::vector<std::filesystem::path> Driver::GetGpioChips()
    {
        std::vector<std::filesystem::path> result;
        const std::filesystem::path devDir = "/dev/";

        if (!std::filesystem::exists(devDir))
        {
            return result;
        }

        for (const auto& entry : std::filesystem::directory_iterator(devDir))
        {
            if (!entry.is_character_file())
            {
                continue;
            }

            if (gpiod::is_gpiochip_device(entry.path()))
            {
                result.push_back(entry.path());
            }
        }
        return result;
    }
}
