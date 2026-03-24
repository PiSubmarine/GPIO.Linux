#include "PiSubmarine/GPIO/RPi/PinGroup.h"
#include <gpiod.hpp>
#include <ranges>
#include <unordered_map>

#include "PiSubmarine/GPIO/RPi/Driver.h"

namespace
{
    PiSubmarine::GPIO::Api::Direction Convert(gpiod::line::direction direction)
    {
        switch (direction)
        {
        case gpiod::line::direction::INPUT:
            return PiSubmarine::GPIO::Api::Direction::Input;
        case gpiod::line::direction::OUTPUT:
            return PiSubmarine::GPIO::Api::Direction::Output;
        default:
            throw std::invalid_argument("Invalid direction");
        }
    }

    gpiod::line::direction Convert(PiSubmarine::GPIO::Api::Direction direction)
    {
        switch (direction)
        {
        case PiSubmarine::GPIO::Api::Direction::Input:
            return gpiod::line::direction::INPUT;
        case PiSubmarine::GPIO::Api::Direction::Output:
            return gpiod::line::direction::OUTPUT;
        }
        throw std::invalid_argument("Invalid direction");
    }

    gpiod::line::value Convert(PiSubmarine::GPIO::Api::Level level)
    {
        switch (level)
        {
        case PiSubmarine::GPIO::Api::Level::Low:
            return gpiod::line::value::INACTIVE;
        case PiSubmarine::GPIO::Api::Level::High:
            return gpiod::line::value::ACTIVE;
        }
        throw std::invalid_argument("Invalid level");
    }
}

namespace PiSubmarine::GPIO::RPi
{
    PinGroup::PinGroup(Driver& driver, const std::vector<PinInfo>& pins, std::string_view groupName) :
        m_Pins(pins),
        m_Driver(driver),
        m_GroupName(groupName)
    {
        if (pins.size() > sizeof(Api::BitfieldType) * 8)
        {
            throw std::invalid_argument("Number of pins is too large");
        }

        const std::filesystem::path chipPath = std::filesystem::canonical(pins[0].DevicePath);
        for (const auto& pin : pins)
        {
            if (pin.DevicePath != chipPath)
            {
                throw std::invalid_argument("Only pins from the same GPIO chip are supported in the same PinGroup");
            }
        }
    }

    std::size_t PinGroup::Num() const
    {
        return m_Pins.size();
    }

    Api::Directions PinGroup::GetDirections()
    {
        if (!m_LineRequest)
        {
            throw std::runtime_error("No pins requested");
        }

        return m_LineRequestDirections;
    }

    void PinGroup::SetDirections(Api::Directions directions)
    {
        m_LineRequest = nullptr;

        auto chip = m_Driver.GetChip(m_Pins[0].DevicePath);
        gpiod::line_settings inputSettings;
        inputSettings.set_direction(gpiod::line::direction::INPUT);
        gpiod::line_settings outputSettings;
        outputSettings.set_direction(gpiod::line::direction::OUTPUT);

        auto requestBuilder = chip->prepare_request();
        for (size_t i = 0; i < m_Pins.size(); i++)
        {
            if (directions.Get(i) == Api::Direction::Input)
            {
                requestBuilder.add_line_settings(m_Pins[i].Line, inputSettings);
            }
            else if (directions.Get(i) == Api::Direction::Output)
            {
                requestBuilder.add_line_settings(m_Pins[i].Line, outputSettings);
            }
            else
            {
                throw std::runtime_error("Invalid direction");
            }
        }

        auto request = requestBuilder.do_request();

        m_LineRequest = std::make_unique<gpiod::line_request>(std::move(request));
        m_LineRequestDirections = directions;
    }

    Api::Levels PinGroup::GetLevels()
    {
        if (!m_LineRequest)
        {
            throw std::runtime_error("No pins requested");
        }

        Api::Levels levelsBitField{0};

        auto levels = m_LineRequest->get_values();
        for (size_t i = 0; i < levels.size(); i++)
        {
            if (levels[i] == gpiod::line::value::ACTIVE)
            {
                levelsBitField.Set(i, Api::Level::High);
            }
        }

        return levelsBitField;
    }

    void PinGroup::SetLevels(Api::Levels levels, Api::Mask mask)
    {
        std::vector<gpiod::line::value> levelsGpiod;
        for (size_t i = 0; i < m_Pins.size(); i++)
        {
            if (mask.Get(i) != Api::MaskBit::Included)
            {
               continue;
            }
            m_LineRequest->set_value(m_Pins[i].Line, Convert(levels.Get(i)));
        }
    }
}
