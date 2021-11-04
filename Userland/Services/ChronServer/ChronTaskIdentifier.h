/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <AK/Traits.h>
#include <unistd.h>

namespace ChronServer {
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

}

template<>
struct AK::Traits<ChronServer::ChronTaskIdentifier> : public AK::GenericTraits<ChronServer::ChronTaskIdentifier> {
    static constexpr bool is_trivial() { return false; }
    static unsigned hash(ChronServer::ChronTaskIdentifier const& entry) { return pair_int_hash(entry.client_user_id(), entry.task_name().hash()); }
    static bool equals(ChronServer::ChronTaskIdentifier const& a, ChronServer::ChronTaskIdentifier const& b) { return a.client_user_id() == b.client_user_id() && a.task_name() == b.task_name(); }
};
