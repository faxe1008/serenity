/*
 * Copyright (c) 2022, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "PasswordDatabaseModel.h"
#include "PasswordGenerator.h"
#include <AK/JsonArray.h>
#include <AK/JsonObject.h>
#include <AK/JsonParser.h>
#include <AK/Try.h>

String PasswordDatabaseModel::column_name(int column) const
{
    switch (column) {
    case Column::Title:
        return "Title";
    case Column::Username:
        return "Username";
    case Column::Description:
        return "Description";
    case Column::Modified:
        return "Modified";
    default:
        VERIFY_NOT_REACHED();
    }
}

GUI::Variant PasswordDatabaseModel::data(const GUI::ModelIndex& index, GUI::ModelRole role) const
{
    if (role != GUI::ModelRole::Display)
        return {};
    auto& item = m_entries[index.row()];
    switch (index.column()) {
    case Column::Title:
        return item.title;
    case Column::Username:
        return item.username;
    case Column::Description:
        return item.description;
    case Column::Modified:
        return item.modified.to_string();
    default:
        VERIFY_NOT_REACHED();
    }
}

ErrorOr<NonnullRefPtr<PasswordDatabaseModel>> PasswordDatabaseModel::try_load_from_file(Core::File& file)
{
    if (!file.is_open())
        return Error::from_string_literal("Trying to load from unopened file");

    return adopt_ref(*new PasswordDatabaseModel);
}

ErrorOr<void> PasswordDatabaseModel::save(Core::File&)
{
    return {};
}

ErrorOr<void> PasswordDatabaseModel::append_empty_entry()
{
    int next_id = 0;
    for (auto const& entry : m_entries) {
        if (entry.id > next_id)
            next_id = entry.id;
    }
    next_id += 1;
    if (next_id < 0) {
        return Error::from_string_literal("No more entry ids available");
    }
    auto password = PasswordGenerator::generate(16);
    PasswordEntry new_entry { next_id, "", "", password, "", Core::DateTime::now() };
    m_entries.append(std::move(new_entry));
    did_update();
    return {};
}

void PasswordDatabaseModel::remove_item(int index)
{
    m_entries.remove(index);
    did_update();
}

void PasswordDatabaseModel::copy_to_clipboard(const GUI::ModelIndex& index, CopyableContent content)
{
    auto& item = m_entries[index.row()];
    switch (content) {
    case CopyableContent::Username:
        GUI::Clipboard::the().set_plain_text(item.username);
        break;
    case CopyableContent::Password:
        GUI::Clipboard::the().set_plain_text(item.password);
        break;
    default:
        VERIFY_NOT_REACHED();
    }
}
