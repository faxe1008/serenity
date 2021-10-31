/*
 * Copyright (c) 2021, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Vector.h>
#include <LibCore/ChronExpression.h>
#include <LibTest/TestCase.h>

TEST_CASE(every_five_minutes)
{
    auto now = Core::DateTime::create(2021, 1, 1, 0, 0, 30);

    auto cron_expression = ChronExpression::from_string("*/5 * * * *");
    EXPECT(cron_expression.has_value());

    auto current_tick = now;
    for (int i = 0; i < 30; i++) {
        auto next_tick = cron_expression->next_tick(current_tick);
        EXPECT(next_tick.has_value());
        EXPECT(next_tick.value().minute() % 5 == 0);
        current_tick = next_tick.release_value();
    }
}

TEST_CASE(ranged_hours)
{
    auto now = Core::DateTime::create(2021, 1, 1, 0, 0, 30);

    auto cron_expression = ChronExpression::from_string("* 4-12 * * *");
    EXPECT(cron_expression.has_value());

    auto current_tick = now;
    for (int i = 0; i < 13; i++) {
        auto next_tick = cron_expression->next_tick(current_tick);
        EXPECT(next_tick.has_value());
        EXPECT(next_tick.value().hour() >= 4 && next_tick.value().hour() <= 12);
        current_tick = next_tick.release_value();
    }
}

TEST_CASE(listed_month)
{
    auto now = Core::DateTime::create(2021, 1, 1, 0, 0, 30);

    auto cron_expression = ChronExpression::from_string("* * * 3,5 *");
    EXPECT(cron_expression.has_value());

    auto current_tick = now;
    for (int i = 0; i < 5; i++) {
        auto next_tick = cron_expression->next_tick(current_tick);
        EXPECT(next_tick.has_value());
        EXPECT(next_tick.value().month() == 3 || next_tick.value().month() == 5);
        current_tick = next_tick.release_value();
    }
}

TEST_CASE(range_of_day_of_week)
{
    auto now = Core::DateTime::create(2021, 1, 1, 0, 0, 30);

    auto cron_expression = ChronExpression::from_string("0 9 * * Mon-Fri");
    EXPECT(cron_expression.has_value());

    auto current_tick = now;
    for (int i = 0; i < 8; i++) {
        auto next_tick = cron_expression->next_tick(current_tick);
        EXPECT(next_tick.has_value());
        EXPECT(next_tick.value().weekday() >= 1 && next_tick.value().weekday() <= 5);
        EXPECT(next_tick.value().hour() == 9);
        EXPECT(next_tick.value().minute() == 0);
        EXPECT(next_tick.value().weekday() != current_tick.weekday());
        current_tick = next_tick.release_value();
    }
}

TEST_CASE(listed_ranged_hour)
{
    auto now = Core::DateTime::create(2021, 1, 1, 0, 0, 30);

    auto cron_expression = ChronExpression::from_string("0 8-10,13,15-18 * * *");
    EXPECT(cron_expression.has_value());
    Vector<u32> expected_hours { 8, 9, 10, 13, 15, 16, 17, 18 };

    auto current_tick = now;
    for (int i = 0; i < 5; i++) {
        auto next_tick = cron_expression->next_tick(current_tick);
        EXPECT(next_tick.has_value());
        EXPECT(next_tick.value().minute() == 0);
        EXPECT(expected_hours.contains_in_range(next_tick.value().hour(), 0, expected_hours.size() - 1));
        current_tick = next_tick.release_value();
    }
}

TEST_CASE(invalid_expression)
{
    Vector<StringView> invalid_expressions {
        "*",
        "* * * *",
        "Mon * * * *",
        "* 4 Lorem * *",
        "80 * * * *",
        "* 25 * * *",
        "* * 32 * *",
        "* * * 14 *",
        "* * * * 7-12",
        "-1 * * * *",
        "12-4 * * * *",
        "* * * * THU-MON"
    };

    for (auto& expression : invalid_expressions) {
        EXPECT(!ChronExpression::from_string(expression).has_value());
    }
}
