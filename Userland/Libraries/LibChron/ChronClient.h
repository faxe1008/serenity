/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <ChronServer/ChronClientEndpoint.h>
#include <ChronServer/ChronServerEndpoint.h>
#include <LibIPC/ServerConnection.h>

namespace Chron {

class ChronClient final
    : public IPC::ServerConnection<ChronClientEndpoint, ChronServerEndpoint>
    , public ChronClientEndpoint {
    C_OBJECT(ChronClient)

public:
    static ChronClient& the();
    bool schedule_task(String const&, String const&, String const&);
    bool reschedule_task(String const&, String const&);
    void delete_task(String const&);

private:
    explicit ChronClient()
        : IPC::ServerConnection<ChronClientEndpoint, ChronServerEndpoint>(*this, "/tmp/portal/chron")
    {
        pledge_euid(geteuid());
    }

    void task_started(String const&) override { }
};

static inline bool schedule_task(String const& task_name, String const& chron_exp, String const& task)
{
    return ChronClient::the().schedule_task(task_name, chron_exp, task);
}
static inline bool reschedule_task(String const& task_name, String const& chron_exp)
{
    return ChronClient::the().reschedule_task(task_name, chron_exp);
}
static inline void delete_task(String const& task_name)
{
    ChronClient::the().delete_task(task_name);
}

}
