/*
 * Copyright (c) 2022, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "PasswordManagerWidget.h"
#include "EditPasswordDialog.h"
#include <Applications/PasswordManager/PasswordManagerWindowGML.h>
#include <LibGUI/Application.h>
#include <LibGUI/Menu.h>
#include <LibGUI/Statusbar.h>
#include <LibGUI/TableView.h>

PasswordManagerWidget::PasswordManagerWidget()
    : m_database_model(PasswordDatabaseModel::create_empty())
{
    load_from_gml(password_manager_window_gml);

    m_statusbar = *find_descendant_of_type_named<GUI::Statusbar>("statusbar");
    m_statusbar->set_text(0, "Selected 1 of 20");

    m_database_view = *find_descendant_of_type_named<GUI::TableView>("database_view");
    m_database_view->set_alternating_row_colors(true);
    m_database_view->set_model(m_database_model);
    m_database_view->set_column_width(0, 90);
    m_database_view->set_column_width(1, 90);
    m_database_view->set_column_width(2, 180);
    m_database_view->set_column_width(3, 90);

    m_database_view->on_activation = [&](GUI::ModelIndex const& index) {
        EditPasswordDialog::show(window(), m_database_model, index);
    };

    m_new_action = GUI::Action::create("New", { Mod_Ctrl, Key_N }, Gfx::Bitmap::try_load_from_file("/res/icons/16x16/new.png").release_value_but_fixme_should_propagate_errors(), [this](const GUI::Action&) {

    });

    m_open_action = GUI::CommonActions::make_open_action([this](auto&) {
    });

    m_save_action = GUI::CommonActions::make_save_action([&](auto&) {

    });

    m_save_as_action = GUI::CommonActions::make_save_as_action([&](auto&) {
    });

    m_copy_username_action = GUI::Action::create("Copy User Name", { Mod_Ctrl, Key_B }, Gfx::Bitmap::try_load_from_file("/res/icons/passwordmanager/copy-username.png").release_value_but_fixme_should_propagate_errors(), [this](const GUI::Action&) {
        m_database_model->copy_to_clipboard(m_database_view->selection().first(), PasswordDatabaseModel::CopyableContent::Username);
    });

    m_copy_password_action = GUI::Action::create("Copy Password", { Mod_Ctrl, Key_C }, Gfx::Bitmap::try_load_from_file("/res/icons/passwordmanager/copy-password.png").release_value_but_fixme_should_propagate_errors(), [this](const GUI::Action&) {
        m_database_model->copy_to_clipboard(m_database_view->selection().first(), PasswordDatabaseModel::CopyableContent::Password);
    });

    m_add_entry_action = GUI::Action::create("Add Entry", { Mod_Ctrl, Key_I }, Gfx::Bitmap::try_load_from_file("/res/icons/passwordmanager/add-entry.png").release_value_but_fixme_should_propagate_errors(), [this](const GUI::Action&) {
        if (!m_database_model->append_empty_entry().is_error())
            EditPasswordDialog::show(window(), m_database_model, m_database_model->index(m_database_model->row_count(GUI::ModelIndex()) - 1));
    });

    m_remove_entry_action = GUI::Action::create("Remove Entry", { Mod_None, Key_Delete }, Gfx::Bitmap::try_load_from_file("/res/icons/passwordmanager/remove-entry.png").release_value_but_fixme_should_propagate_errors(), [this](const GUI::Action&) {
        m_database_view->selection().for_each_index([&](GUI::ModelIndex& index) {
            m_database_model->remove_item(index.row());
        });
    });
}

void PasswordManagerWidget::initialize_menubar(GUI::Window& window)
{
    auto& file_menu = window.add_menu("&File");
    file_menu.add_action(*m_new_action);
    file_menu.add_action(*m_open_action);
    file_menu.add_action(*m_save_action);
    file_menu.add_action(*m_save_as_action);
    file_menu.add_separator();
    file_menu.add_action(GUI::CommonActions::make_quit_action([this](auto&) {
        GUI::Application::the()->quit();
    }));

    auto& entry_menu = window.add_menu("&Entry");
    entry_menu.add_action(*m_copy_username_action);
    entry_menu.add_action(*m_copy_password_action);
    entry_menu.add_separator();
    entry_menu.add_action(*m_add_entry_action);
    entry_menu.add_action(*m_remove_entry_action);
}
