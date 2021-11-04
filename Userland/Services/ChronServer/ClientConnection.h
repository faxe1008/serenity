/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Optional.h>
#include <AK/Traits.h>
#include <ChronServer/ChronClientEndpoint.h>
#include <ChronServer/ChronServerEndpoint.h>
#include <LibIPC/ClientConnection.h>

namespace ChronServer {

void check_and_execute_tasks();

class ClientConnection final : public IPC::ClientConnection<ChronClientEndpoint, ChronServerEndpoint> {
    C_OBJECT(ClientConnection)

public:
    ~ClientConnection() override;
    virtual void die() override { }
    const Optional<uid_t>& client_user_id() const { return m_client_euid; }

private:
    explicit ClientConnection(NonnullRefPtr<Core::LocalSocket>, int client_id);

    virtual void pledge_euid(uid_t const& euid) override;
    virtual Messages::ChronServer::ScheduleChronTaskResponse schedule_chron_task(String const& task_name, String const& chron_expression, String const& task) override;
    virtual Messages::ChronServer::RescheduleChronTaskResponse reschedule_chron_task(String const& task_name, String const& chron_expression) override;
    virtual void delete_chron_task(String const& task_name) override;

    Optional<uid_t> m_client_euid;
};

}
