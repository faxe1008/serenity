/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Format.h>
#include <ChronServer/ChronTask.h>
#include <ChronServer/ChronTaskIdentifier.h>
#include <ChronServer/ClientConnection.h>
namespace ChronServer {

static HashMap<int, RefPtr<ClientConnection>> s_connections;
static HashMap<ChronTaskIdentifier, ChronTask> s_tasks;

void check_and_execute_tasks()
{
    auto now = Core::DateTime::now();
    for (auto& [task_identifier, task] : s_tasks) {
        if (task.has_expired(now)) {
            dbgln("ChronServer: {} Executing task {} for {}", now.to_string(), task_identifier.task_name(), task_identifier.client_user_id());
            if (task.execute(now)) {
                for (auto& connection : s_connections) {
                    if (connection.value->client_user_id() == task_identifier.client_user_id()) {
                        connection.value->async_task_started(task_identifier.task_name());
                    }
                }
            }
        }
    }
}

ClientConnection::ClientConnection(NonnullRefPtr<Core::LocalSocket> client_socket, int client_id)
    : IPC::ClientConnection<ChronClientEndpoint, ChronServerEndpoint>(*this, move(client_socket), client_id)
{
    s_connections.set(client_id, *this);
}

ClientConnection::~ClientConnection()
{
}

void ClientConnection::pledge_euid(const uid_t& euid)
{
    dbgln("Client {}: got euid {}", client_id(), euid);
    if (m_client_euid.has_value()) {
        did_misbehave("ChronServer: Can not pledge euid more then once");
        return;
    }
    m_client_euid = euid;
}

Messages::ChronServer::ScheduleChronTaskResponse ClientConnection::schedule_chron_task(String const& task_name, String const& cron_expression, String const& task)
{
    if (!m_client_euid.has_value()) {
        did_misbehave("ChronServer: Need to pledge euid first");
        return false;
    }

    auto cron_exp_result = ChronExpression::from_string(cron_expression);
    if (!cron_exp_result.has_value())
        return false;

    ChronTaskIdentifier identifier { m_client_euid.value(), task_name };
    if (s_tasks.contains(identifier))
        return false;

    dbgln("ChronServer: Scheduled {} at {}: {}", task_name, cron_expression, task);
    s_tasks.set(identifier, { cron_exp_result.value(), task });
    return true;
}

Messages::ChronServer::RescheduleChronTaskResponse ClientConnection::reschedule_chron_task(String const& task_name, String const& chron_expression)
{
    if (!m_client_euid.has_value()) {
        did_misbehave("ChronServer: Need to pledge euid first");
        return false;
    }

    auto chron_exp_result = ChronExpression::from_string(chron_expression);
    if (!chron_exp_result.has_value())
        return false;

    auto task = s_tasks.get({ m_client_euid.value(), task_name });
    if (!task.has_value())
        return false;

    dbgln("ChronServer: rescheduling {} to {}", task_name, chron_expression);
    task->reschedule(chron_exp_result.value());
    return true;
}

void ClientConnection::delete_chron_task(String const& task_name)
{
    if (!m_client_euid.has_value()) {
        did_misbehave("ChronServer: Need to pledge euid first");
        return;
    }
    dbgln("ChronServer: removing task {}", task_name);
    s_tasks.remove({ m_client_euid.value(), task_name });
}

}
