/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ChronTask.h"
#include <cerrno>
#include <spawn.h>
#include <unistd.h>
namespace ChronServer {

bool ChronTask::has_expired(const Core::DateTime& time) const
{
    if (!m_next_expiration.has_value())
        dbgln("ChronTask: has no next expiration");
    else
        dbgln("ChronTask: next expiration is {}, now is {}", m_next_expiration->to_string(), time.to_string());
    return m_next_expiration.has_value() && m_next_expiration.value() < time;
}

bool ChronTask::execute(const Core::DateTime& time)
{
    if (m_task_args.size() <= 1)
        return false;

    Vector<const char*> args;
    for (const auto& arg : m_task_args)
        args.append(arg.characters());
    args.append(nullptr);

    m_next_expiration = m_chron_expression.next_tick(time);

    auto argv = args.data();
    pid_t pid;
    if ((errno = posix_spawnp(&pid, argv[0], nullptr, nullptr, const_cast<char**>(argv), environ))) {
        perror("posix_spawn");
        return false;
    }
    return true;
}

}
