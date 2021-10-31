/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ChronExpression.h"
#include <AK/Format.h>
#include <AK/Vector.h>

Optional<ChronExpression::DayOfWeek> ChronExpression::day_of_week_from_string(const StringView& str)
{
    if (str.equals_ignoring_case("SUN")) {
        return { DayOfWeek::Sun };
    } else if (str.equals_ignoring_case("MON")) {
        return { DayOfWeek::Mon };
    } else if (str.equals_ignoring_case("TUE")) {
        return { DayOfWeek::Tue };
    } else if (str.equals_ignoring_case("WED")) {
        return { DayOfWeek::Wed };
    } else if (str.equals_ignoring_case("THU")) {
        return { DayOfWeek::Thu };
    } else if (str.equals_ignoring_case("FRI")) {
        return { DayOfWeek::Fri };
    } else if (str.equals_ignoring_case("SAT")) {
        return { DayOfWeek::Sat };
    }
    return {};
}

Optional<ChronExpression> ChronExpression::from_string(const StringView& str)
{
    auto fields = str.split_view(' ');
    if (fields.size() != 5) {
        return {};
    }

    auto month = parse_field(fields[3], 1, 12);
    auto day_of_month = parse_field(fields[2], 1, 31);
    auto hour = parse_field(fields[1], 0, 23);
    auto minute = parse_field(fields[0], 0, 59);
    auto day_of_week = parse_field(fields[4], 0, 6, true);

    if (!month.has_value() || !day_of_month.has_value() || !hour.has_value() || !minute.has_value() || !day_of_week.has_value())
        return {};
    return ChronExpression(month.value(), day_of_month.value(), hour.value(), minute.value(), day_of_week.value());
}

Optional<Core::DateTime> ChronExpression::next_tick(const Core::DateTime& time) const
{
    auto next = Core::DateTime::from_timestamp(time.timestamp() + 60);

    for (;;) {
        if (next.year() - time.year() > 4)
            return {};

        if (!m_months.contains(next.month())) {
            if (next.month() == 12) {
                next = Core::DateTime::create(next.year() + 1, 1, 1, 0, 0, 0);
            } else {
                next = Core::DateTime::create(next.year(), next.month() + 1, 1, 0, 0, 0);
            }
            continue;
        }

        if (!m_day_of_month.contains(next.day())) {
            next = Core::DateTime::from_timestamp(next.timestamp() + 1440 * 60);
            continue;
        }

        if (!m_hours.contains(next.hour())) {
            next = Core::DateTime::from_timestamp(next.timestamp() + 3600);
            continue;
        }

        if (!m_minutes.contains(next.minute())) {
            next = Core::DateTime::from_timestamp(next.timestamp() + 60);
            continue;
        }

        if (!m_days_of_week.contains(next.weekday())) {
            next = Core::DateTime::from_timestamp(next.timestamp() + 1440 * 60);
            continue;
        }

        return next;
    }
    return {};
}

Optional<HashTable<u32>> ChronExpression::parse_field(const StringView& field, u32 min, u32 max, bool allow_dow_literal)
{
    HashTable<u32> values;
    auto sub_fields = field.split_view(',');

    for (auto& sub_field : sub_fields) {
        if (sub_field == "*") {
            for (u32 i = min; i < max; i++)
                values.set(i);
            continue;
        }

        if (sub_field.starts_with("*/")) {
            auto f = sub_field.trim("*/").to_uint();
            if (!f.has_value() || f.value() > max || f.value() < min)
                return {};

            for (u32 i = min; i < max; i += f.value())
                values.set(i);
            continue;
        }

        if (sub_field.contains('-')) {
            auto tmp_fields = sub_field.split_view('-');
            if (tmp_fields.size() != 2)
                return {};

            auto start_value = parse_field_component(tmp_fields[0], min, max, allow_dow_literal);
            if (!start_value.has_value())
                return {};
            u32 range_start = start_value.value();

            auto end_value = parse_field_component(tmp_fields[1], min, max, allow_dow_literal);
            if (!end_value.has_value())
                return {};
            u32 range_end = end_value.value();

            if (range_start > range_end)
                return {};

            for (u32 i = range_start; i < range_end; i++)
                values.set(i);
            continue;
        }

        auto simple_value = parse_field_component(sub_field, min, max, allow_dow_literal);
        if (!simple_value.has_value())
            return {};
        values.set(simple_value.value());
    }
    return values;
}

Optional<u32> ChronExpression::parse_field_component(const StringView& component, u32 min, u32 max, bool allow_dow_literal)
{
    auto day_of_week = day_of_week_from_string(component);
    if (day_of_week.has_value()) {
        if (!allow_dow_literal)
            return {};
        return static_cast<u32>(day_of_week.value());
    } else {
        auto raw_value = component.to_uint();
        if (!raw_value.has_value() || raw_value.value() > max || raw_value.value() < min)
            return {};
        return raw_value.value();
    }
}
