/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Optional.h>
#include <AK/Traits.h>
#include <AK/Tuple.h>
#include <AK/Vector.h>
#include <ChronServer/ChronClientEndpoint.h>
#include <ChronServer/ChronServerEndpoint.h>
#include <LibCore/ChronExpression.h>
#include <LibCore/DateTime.h>
#include <LibCore/Timer.h>
#include <LibIPC/ClientConnection.h>

namespace ChronServer {

class ChronTask {
public:
    ChronTask(ChronExpression chron_expression, String task)
        : m_chron_expression(chron_expression)
        , m_next_expiration(m_chron_expression.next_tick(Core::DateTime::now()))
        , m_executable_components(task.split(' '))
    {
    }
    bool has_expired(const Core::DateTime& time) const;
    void reschedule(ChronExpression& exp) { m_chron_expression = std::move(exp); }
    bool execute() const;

private:
    ChronExpression m_chron_expression;
    Optional<Core::DateTime> m_next_expiration;
    Vector<String> m_executable_components;
};

class ChronTaskIdentifier {
public:
    ChronTaskIdentifier(uid_t client_user_id, String task_name)
        : m_client_euid(client_user_id)
        , m_task_name(task_name)
    {
    }

    const uid_t& client_user_id() const { return m_client_euid; }
    const String& task_name() const { return m_task_name; }

private:
    uid_t m_client_euid;
    String m_task_name;
};

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

void check_and_execute_tasks();

}

template<>
struct AK::Traits<ChronServer::ChronTaskIdentifier> : public AK::GenericTraits<ChronServer::ChronTaskIdentifier> {
    static constexpr bool is_trivial() { return false; }
    static unsigned hash(ChronServer::ChronTaskIdentifier const& entry) { return pair_int_hash(entry.client_user_id(), entry.task_name().hash()); }
    static bool equals(ChronServer::ChronTaskIdentifier const& a, ChronServer::ChronTaskIdentifier const& b) { return a.client_user_id() == b.client_user_id() && a.task_name() == b.task_name(); }
};
