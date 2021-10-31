/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/HashTable.h>
#include <AK/Optional.h>
#include <AK/StringView.h>
#include <AK/Time.h>
#include <AK/Types.h>
#include <LibCore/DateTime.h>

class ChronExpression {
public:
    static Optional<ChronExpression> from_string(const StringView&);

    Optional<Core::DateTime> next_tick(const Core::DateTime&) const;

private:
    enum class DayOfWeek {
        Sun = 0,
        Mon = 1,
        Tue = 2,
        Wed = 3,
        Thu = 4,
        Fri = 5,
        Sat = 6
    };

    static Optional<DayOfWeek> day_of_week_from_string(const StringView&);
    static Optional<HashTable<u32>> parse_field(const StringView&, u32, u32, bool = false);
    static Optional<u32> parse_field_component(const StringView&, u32, u32, bool);

    ChronExpression(HashTable<u32> months, HashTable<u32> day_of_months, HashTable<u32> hours, HashTable<u32> minutes, HashTable<u32> days_of_week)
        : m_months(months)
        , m_day_of_month(day_of_months)
        , m_hours(hours)
        , m_minutes(minutes)
        , m_days_of_week(days_of_week)
    {
    }

    HashTable<u32> m_months;
    HashTable<u32> m_day_of_month;
    HashTable<u32> m_hours;
    HashTable<u32> m_minutes;
    HashTable<u32> m_days_of_week;
};
