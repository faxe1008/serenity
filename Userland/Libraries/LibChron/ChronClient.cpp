/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibChron/ChronClient.h>
#include <unistd.h>
namespace Chron {

static RefPtr<ChronClient> s_the = nullptr;

ChronClient& ChronClient::the()
{
    if (!s_the || !s_the->is_open()) {
        VERIFY(Core::EventLoop::has_been_instantiated());
        s_the = ChronClient::construct();
    }
    return *s_the;
}

bool ChronClient::schedule_task(String const& task_name, String const& chron_expression, String const& task)
{
    return schedule_chron_task(task_name, chron_expression, task);
}

bool ChronClient::reschedule_task(String const& task_name, String const& chron_expression)
{
    return reschedule_chron_task(task_name, chron_expression);
}

void ChronClient::delete_task(String const& task_name)
{
    async_delete_chron_task(task_name);
}

}
