/*
 * Copyright (c) 2022, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "EditPasswordDialog.h"
#include "PasswordGenerator.h"
#include <Applications/PasswordManager/EditPasswordDialogGML.h>
#include <LibGUI/Widget.h>

EditPasswordDialog::EditPasswordDialog(NonnullRefPtr<PasswordDatabaseModel> database, GUI::ModelIndex const& index)
    : Dialog(nullptr)
    , m_database(database)
    , m_edited_entry(m_database->at(index))
{
    center_on_screen();
    set_resizable(false);
    resize(350, 250);
    set_title("Edit Password");

    auto& main_widget = set_main_widget<GUI::Widget>();
    if (!main_widget.load_from_gml(edit_password_dialog_gml))
        VERIFY_NOT_REACHED();

    m_title_textbox = *main_widget.find_descendant_of_type_named<GUI::TextBox>("title_textbox");
    m_username_textbox = *main_widget.find_descendant_of_type_named<GUI::TextBox>("username_textbox");
    m_password_textbox = *main_widget.find_descendant_of_type_named<GUI::PasswordBox>("password_textbox");
    m_repeat_password_textbox = *main_widget.find_descendant_of_type_named<GUI::PasswordBox>("password_repeat_textbox");
    m_generate_password_button = *main_widget.find_descendant_of_type_named<GUI::Button>("generate_password_button");
    m_description = *main_widget.find_descendant_of_type_named<GUI::TextEditor>("description_texteditor");

    m_title_textbox->set_text(m_edited_entry.title);
    m_username_textbox->set_text(m_edited_entry.username);
    m_password_textbox->set_text(m_edited_entry.password);
    m_repeat_password_textbox->set_text(m_edited_entry.password);
    m_description->set_text(m_edited_entry.description);

    m_save_button = *main_widget.find_descendant_of_type_named<GUI::Button>("save_button");
    m_cancel_button = *main_widget.find_descendant_of_type_named<GUI::Button>("cancel_button");

    m_generate_password_button->on_click = [this](auto) {
        auto password = PasswordGenerator::generate(16);
        m_edited_entry.password = password;
        m_password_textbox->set_text(m_edited_entry.password);
        m_repeat_password_textbox->set_text(m_edited_entry.password);
    };

    m_cancel_button->on_click = [this](auto) {
        done(ExecResult::ExecOK);
    };

    m_save_button->on_click = [this](auto) {
        // TODO: Compare repeat password
        m_edited_entry.title = m_title_textbox->text();
        m_edited_entry.username = m_username_textbox->text();
        m_edited_entry.password = m_password_textbox->text();
        m_edited_entry.description = m_description->text();
        m_edited_entry.modified = Core::DateTime::now();

        done(ExecResult::ExecOK);
    };
}

int EditPasswordDialog::show(GUI::Window* parent_window, NonnullRefPtr<PasswordDatabaseModel> database, GUI::ModelIndex const& edited_index)
{

    auto dialog = EditPasswordDialog::construct(database, edited_index);

    if (parent_window)
        dialog->set_icon(parent_window->icon());

    auto result = dialog->exec();

    if (result != GUI::Dialog::ExecOK)
        return result;

    return GUI::Dialog::ExecOK;
}
