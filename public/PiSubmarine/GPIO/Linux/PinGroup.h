#pragma once

#include "PiSubmarine/GPIO/Api/IPinGroup.h"
#include <gpiod.hpp>
#include <unordered_map>

#include "PinInfo.h"

namespace PiSubmarine::GPIO::Linux
{
    class Driver;

    class PinGroup : public Api::IPinGroup
    {
    public:
        PinGroup(gpiod::chip& chip, const std::vector<gpiod::line::offset>& offsets, std::string_view groupName);
        ~PinGroup() override = default;

        std::size_t Num() const override;
        Api::Directions GetDirections() override;
        void SetDirections(Api::Directions directions) override;
        Api::Levels GetLevels() override;
        void SetLevels(Api::Levels levels, Api::Mask mask) override;

        std::string_view GetName() const;
    private:
        gpiod::chip& m_Chip;
        std::vector<gpiod::line::offset> m_Pins;
        std::unique_ptr<gpiod::line_request> m_LineRequest;
        Api::Directions m_LineRequestDirections {0};
        std::string m_GroupName;
    };
}
