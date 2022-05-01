/*
 * Copyright (c) 2022, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "PasswordDatabaseModel.h"
#include <AK/RefPtr.h>
#include <LibGUI/Action.h>
#include <LibGUI/Button.h>
#include <LibGUI/Dialog.h>
#include <LibGUI/TextBox.h>
#include <LibGUI/TextEditor.h>
#include <LibGUI/Window.h>

class EditPasswordDialog final : public GUI::Dialog {
    C_OBJECT(EditPasswordDialog)
public:
    virtual ~EditPasswordDialog() override = default;
    static int show(GUI::Window* parent_window, NonnullRefPtr<PasswordDatabaseModel> database, GUI::ModelIndex const& edited_index);

private:
    EditPasswordDialog(NonnullRefPtr<PasswordDatabaseModel>, GUI::ModelIndex const&);
    NonnullRefPtr<PasswordDatabaseModel> m_database;
    PasswordEntry& m_edited_entry;

    RefPtr<GUI::TextBox> m_title_textbox;
    RefPtr<GUI::TextBox> m_username_textbox;
    RefPtr<GUI::PasswordBox> m_password_textbox;
    RefPtr<GUI::PasswordBox> m_repeat_password_textbox;
    RefPtr<GUI::Button> m_generate_password_button;
    RefPtr<GUI::TextEditor> m_description;

    RefPtr<GUI::Button> m_save_button;
    RefPtr<GUI::Button> m_cancel_button;
};
