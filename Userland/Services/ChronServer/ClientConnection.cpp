/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <ChronServer/ClientConnection.h>
#include <spawn.h>

namespace ChronServer {

static HashMap<int, RefPtr<ClientConnection>> s_connections;
static HashMap<ChronTaskIdentifier, ChronTask> s_tasks;

void check_and_execute_tasks()
{
    auto now = Core::DateTime::now();
    for (const auto& [task_identifier, task] : s_tasks) {
        if (task.has_expired(now)) {
            dbgln("ChronServer: Executing task {} for {}", task_identifier.task_name(), task_identifier.client_user_id());
            if (task.execute()) {
                for (auto& connection : s_connections) {
                    if (connection.value->client_user_id() == task_identifier.client_user_id()) {
                        connection.value->async_task_started(task_identifier.task_name());
                    }
                }
            }
        }
    }
}

bool ChronTask::has_expired(const Core::DateTime& time) const
{
    return m_next_expiration.has_value() && m_next_expiration.value() < time;
}

bool ChronTask::execute() const
{
    if (m_executable_components.size() == 0)
        return false;

    Vector<const char*> parts;
    for (const auto& part : m_executable_components)
        parts.append(part.characters());

    parts.append(nullptr);
    auto argv = parts.data();
    pid_t pid;
    if ((errno = posix_spawnp(&pid, argv[0], nullptr, nullptr, const_cast<char**>(argv), environ))) {
        perror("posix_spawn");
        return false;
    }
    return true;
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

    task->reschedule(chron_exp_result.value());
    return true;
}

void ClientConnection::delete_chron_task(String const& task_name)
{
    if (!m_client_euid.has_value()) {
        did_misbehave("ChronServer: Need to pledge euid first");
        return;
    }
    s_tasks.remove({ m_client_euid.value(), task_name });
}

}
