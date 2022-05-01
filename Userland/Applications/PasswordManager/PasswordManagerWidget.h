/*
 * Copyright (c) 2022, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "PasswordDatabaseModel.h"
#include <LibGUI/Action.h>
#include <LibGUI/Clipboard.h>
#include <LibGUI/Widget.h>
#include <LibGUI/Window.h>

class PasswordManagerWidget final : public GUI::Widget {
    C_OBJECT(PasswordManagerWidget)
public:
    virtual ~PasswordManagerWidget() override = default;
    void initialize_menubar(GUI::Window&);

private:
    PasswordManagerWidget();

    String m_path;
    NonnullRefPtr<PasswordDatabaseModel> m_database_model;

    RefPtr<GUI::Statusbar> m_statusbar;
    RefPtr<GUI::TableView> m_database_view;

    RefPtr<GUI::Action> m_new_action;
    RefPtr<GUI::Action> m_open_action;
    RefPtr<GUI::Action> m_save_action;
    RefPtr<GUI::Action> m_save_as_action;

    RefPtr<GUI::Action> m_copy_username_action;
    RefPtr<GUI::Action> m_copy_password_action;
    RefPtr<GUI::Action> m_add_entry_action;
    RefPtr<GUI::Action> m_remove_entry_action;
};
