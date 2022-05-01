/*
 * Copyright (c) 2022, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/Vector.h>
#include <LibCore/DateTime.h>
#include <LibCore/File.h>
#include <LibGUI/Clipboard.h>
#include <LibGUI/Model.h>

struct PasswordEntry {
    int id;
    String title;
    String username;
    String password;
    String description;
    Core::DateTime modified;
};

class PasswordDatabaseModel final : public GUI::Model {
public:
    enum Column {
        Title,
        Username,
        Description,
        Modified,
        __Count
    };

    enum class CopyableContent {
        Username,
        Password
    };

    static NonnullRefPtr<PasswordDatabaseModel> create_empty() { return adopt_ref(*new PasswordDatabaseModel); }
    static ErrorOr<NonnullRefPtr<PasswordDatabaseModel>> try_load_from_file(Core::File&);
    virtual ~PasswordDatabaseModel() override = default;

    ErrorOr<void> save(Core::File&);
    ErrorOr<void> append_empty_entry();
    PasswordEntry& at(GUI::ModelIndex const& index) { return m_entries[index.row()]; }
    void remove_item(int);
    void copy_to_clipboard(const GUI::ModelIndex&, CopyableContent);

    virtual GUI::Variant data(const GUI::ModelIndex&, GUI::ModelRole) const override;
    virtual int row_count(const GUI::ModelIndex&) const override { return m_entries.size(); }

private:
    PasswordDatabaseModel() = default;
    PasswordDatabaseModel(const Vector<PasswordEntry>& password_entries)
        : m_entries(std::move(password_entries))
    {
    }

    virtual String column_name(int) const override;
    virtual int column_count(const GUI::ModelIndex&) const override { return Column::__Count; }

    Vector<PasswordEntry> m_entries;
};
