/**
MEXC Event Data Models

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/mexc/mexc_event_models.h"
#include "vk/tools/utils.h"
#include "vk/tools/json_utils.h"

namespace vk::mexc::futures {
nlohmann::json WSSubscriptionParameters::toJson() const {
    nlohmann::json result;
    result["symbol"] = m_symbol;

    if (m_compress) {
        result["compress"] = m_compress;
    }

    if (m_limit != -1) {
        result["limit"] = m_limit;
    }

    if (!m_interval.empty()) {
        result["interval"] = m_interval;
    }

    return result;
}

void WSSubscriptionParameters::fromJson(const nlohmann::json &json) {
    throw std::runtime_error("Unimplemented: WSSubscription::fromJson()");
}

nlohmann::json WSSubscription::toJson() const {
    nlohmann::json result;
    result["method"] = m_method;
    result["param"] = m_parameters.toJson();
    return result;
}

void WSSubscription::fromJson(const nlohmann::json &json) {
    throw std::runtime_error("Unimplemented: WSSubscription::fromJson()");
}

nlohmann::json Event::toJson() const {
    throw std::runtime_error("Unimplemented: Event::toJson()");
}

void Event::fromJson(const nlohmann::json &json) {
    //    readValue<std::string>(json, "topic", m_topic);
    //    readMagicEnum<ResponseType>(json, "type", m_type);
    //    readValue<std::int64_t>(json, "ts", m_ts);
    //    m_data = json["data"];
}

nlohmann::json EventTicker::toJson() const {
    throw std::runtime_error("Unimplemented: EventTicker::toJson()");
}

void EventTicker::fromJson(const nlohmann::json &json) {
    readValue<std::string>(json, "symbol", m_symbol);
    m_ask1Price = readStringAsDouble(json, "ask1Price", m_ask1Price);
    m_ask1Size = readStringAsDouble(json, "ask1Size", m_ask1Size);
    m_bid1Price = readStringAsDouble(json, "bid1Price", m_bid1Price);
    m_bid1Size = readStringAsDouble(json, "bid1Size", m_bid1Size);
    m_lastPrice = readStringAsDouble(json, "lastPrice", m_lastPrice);
}

void EventTicker::loadEventData(const Event &event) {
    //    switch (event.m_type) {
    //    case ResponseType::snapshot:
    //        fromJson(event.m_data);
    //        break;
    //    case ResponseType::delta:
    //        fromJson(event.m_data);
    //        break;
    //    }
}

nlohmann::json EventCandlestick::toJson() const {
    throw std::runtime_error("Unimplemented: EventCandlestick::toJson()");
}

void EventCandlestick::fromJson(const nlohmann::json &json) {
    readValue<std::int64_t>(json, "start", m_start);
    readValue<std::int64_t>(json, "end", m_end);
    readValue<std::string>(json, "interval", m_interval);
    m_open = readStringAsDouble(json, "open", m_open);
    m_high = readStringAsDouble(json, "high", m_high);
    m_low = readStringAsDouble(json, "low", m_low);
    m_close = readStringAsDouble(json, "close", m_close);
    m_volume = readStringAsDouble(json, "volume", m_volume);
    m_turnover = readStringAsDouble(json, "turnover", m_turnover);
    readValue<bool>(json, "confirm", m_confirm);
    readValue<std::int64_t>(json, "timestamp", m_timestamp);
}
}
