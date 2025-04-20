/**
MEXC Event Data Models

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef INCLUDE_VK_MEXC_EVENT_MODELS_V5_H
#define INCLUDE_VK_MEXC_EVENT_MODELS_V5_H

#include "vk/interface/i_json.h"
#include <nlohmann/json.hpp>

namespace vk::mexc::futures {

struct WSSubscriptionParameters final : IJson {
    std::string m_symbol{};
    std::string m_interval{};
    std::int32_t m_limit{-1};
    bool m_compress{false};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct WSSubscription final : IJson {
    std::string m_method{};
    WSSubscriptionParameters m_parameters{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Event final : IJson {
    std::string m_channel{};
    std::string m_symbol{};
    std::int64_t m_ts{};
    nlohmann::json m_data{};

    ~Event() override = default;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json& json) override;
};

struct EventTicker final : IJson {
    std::string m_symbol{};
    double m_ask1Price{};
    double m_ask1Size{};
    double m_bid1Price{};
    double m_bid1Size{};
    double m_lastPrice{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json& json) override;

    void loadEventData(const Event& event);
};

struct EventCandlestick final : IJson {
    std::int64_t m_start{};
    std::int64_t m_end{};
    std::string m_interval{};
    double m_open{};
    double m_high{};
    double m_low{};
    double m_close{};
    double m_volume{};
    double m_turnover{};
    bool m_confirm{false};
    std::int64_t m_timestamp{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json& json) override;
};
}
#endif //INCLUDE_VK_MEXC_EVENT_MODELS_V5_H
