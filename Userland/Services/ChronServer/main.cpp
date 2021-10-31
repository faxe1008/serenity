/*
 * Copyright (c) 2021, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <ChronServer/ClientConnection.h>
#include <LibCore/EventLoop.h>
#include <LibCore/LocalServer.h>
#include <LibCore/Timer.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    Core::EventLoop event_loop;
    auto server = Core::LocalServer::construct();

    if (pledge("stdio accept rpath proc exec", nullptr) < 0) {
        perror("pledge");
        return 1;
    }

    auto timer = Core::Timer::create_repeating(30000, [&] {
        ChronServer::check_and_execute_tasks();
    });
    timer->start();

    bool ok = server->take_over_from_system_server();
    VERIFY(ok);

    server->on_ready_to_accept = [&] {
        auto client_socket = server->accept();
        if (!client_socket) {
            dbgln("ChronServer: accept failed.");
            return;
        }
        static int s_next_client_id = 0;
        int client_id = ++s_next_client_id;
        IPC::new_client_connection<ChronServer::ClientConnection>(client_socket.release_nonnull(), client_id);
    };

    return event_loop.exec();
}
