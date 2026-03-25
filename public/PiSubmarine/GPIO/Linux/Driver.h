#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include "PinInfo.h"
#include "PiSubmarine/GPIO/Api/IDriver.h"
#include "PiSubmarine/GPIO/Linux/PinGroup.h"
#include "PiSubmarine/GPIO/Api/Mask.h"

namespace PiSubmarine::GPIO::Linux
{
    class Driver : public Api::IDriver
    {
    public:
        explicit Driver(std::string_view consumerName);
        ~Driver() override = default;
        std::shared_ptr<Api::IPinGroup> GetPinGroup(std::string_view groupName) override;
        std::shared_ptr<Api::IPinGroup> CreatePinGroup(std::string_view groupName, const std::filesystem::path& chipPath, const std::vector<std::size_t>& pins);

        bool GetPinInfo(std::string_view userName, PinInfo& outInfo) const;
        PinInfo& GetPinInfo(const std::filesystem::path& devicePath, std::size_t line);

        std::shared_ptr<gpiod::chip> GetChip(const std::filesystem::path& devicePath);
        [[nodiscard]] std::string_view GetConsumerName() const;

        static std::vector<std::filesystem::path> GetGpioChips();
    private:
        std::string m_ConsumerName;

        std::vector<PinInfo> m_Pins;
        std::vector<std::shared_ptr<PinGroup>> m_PinGroups;
        std::vector<std::shared_ptr<gpiod::chip>> m_Chips;
    };
}
