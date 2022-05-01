/*
 * Copyright (c) 2022, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "PasswordDatabaseModel.h"
#include "PasswordManagerWidget.h"
#include <AK/Try.h>
#include <LibCore/File.h>
#include <LibGUI/Application.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Widget.h>
#include <LibGUI/Window.h>
#include <LibMain/Main.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    // TODO: add pledges
    auto app = TRY(GUI::Application::try_create(arguments));
    auto app_icon = TRY(GUI::Icon::try_create_default_icon("app-passwordmanager"));

    auto window = TRY(GUI::Window::try_create());
    window->set_title("PasswordManager");
    window->resize(640, 400);

    auto password_manager_widget = TRY(window->try_set_main_widget<PasswordManagerWidget>());
    password_manager_widget->initialize_menubar(window);

    window->show();
    window->set_icon(app_icon.bitmap_for_size(16));
    return app->exec();
}
