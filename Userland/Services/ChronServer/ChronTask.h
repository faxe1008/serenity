/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Optional.h>
#include <AK/String.h>
#include <AK/Vector.h>
#include <LibCore/ChronExpression.h>
#include <LibCore/DateTime.h>

namespace ChronServer {
class ChronTask {
public:
    ChronTask(String task_name, ChronExpression chron_expression, String task)
        : m_task_name(task_name)
        , m_chron_expression(chron_expression)
        , m_next_expiration(m_chron_expression.next_tick(Core::DateTime::now()))
        , m_task_args(task.split(' '))
    {
    }
    bool has_expired(const Core::DateTime& time) const;
    void reschedule(ChronExpression& exp)
    {
        m_chron_expression = std::move(exp);
        m_next_expiration = m_chron_expression.next_tick(Core::DateTime::now());
    }
    bool execute(const Core::DateTime&);
    const String& task_name() const { return m_task_name; }
    String task() const { return String::join(' ', m_task_args); }

private:
    String m_task_name;
    ChronExpression m_chron_expression;
    Optional<Core::DateTime> m_next_expiration;
    Vector<String> m_task_args;
};

}
