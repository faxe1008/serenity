/*
 * Copyright (c) 2018-2020, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibCore/ConfigFile.h>
#include <LibDesktop/AppFile.h>
#include <LibGUI/IconView.h>
#include <LibGUI/Widget.h>

namespace TaskbarSettings {

class QuickLaunchAppsModel final : public GUI::Model {
public:
    static NonnullRefPtr<QuickLaunchAppsModel> create(NonnullRefPtrVector<Desktop::AppFile> apps)
    {
        return adopt_ref(*new QuickLaunchAppsModel(apps));
    }

    void append(Desktop::AppFile& app_file)
    {
        m_apps.append(app_file);
        invalidate();
    }
    virtual int row_count(GUI::ModelIndex const&) const override { return m_apps.size(); }
    virtual int column_count(GUI::ModelIndex const&) const override { return 1; }

    virtual GUI::ModelIndex index(int row, int column, GUI::ModelIndex const&) const override
    {
        if (row < 0 || row >= (int)m_apps.size())
            return {};
        return create_index(row, column, &m_apps[row]);
    }

    virtual GUI::Variant data(GUI::ModelIndex const& index, GUI::ModelRole role) const override
    {
        auto& app = m_apps[index.row()];
        if (role == GUI::ModelRole::Icon) {
            return app.icon();
        }
        if (role == GUI::ModelRole::Display) {
            return app.name();
        }
        if (role == GUI::ModelRole::Custom) {
            return app.executable();
        }
        return {};
    }

private:
    QuickLaunchAppsModel(NonnullRefPtrVector<Desktop::AppFile> apps)
        : m_apps(move(apps))
    {
    }

    NonnullRefPtrVector<Desktop::AppFile> m_apps;
};

class QuickLaunchSettingsWidget : public GUI::Widget {
    C_OBJECT(QuickLaunchSettingsWidget);

public:
    virtual ~QuickLaunchSettingsWidget() override;

    void apply_settings();

private:
    QuickLaunchSettingsWidget();
    NonnullRefPtrVector<Desktop::AppFile> load_quick_launch_apps();

    NonnullRefPtr<QuickLaunchAppsModel> m_app_model;
    RefPtr<GUI::IconView> m_apps_view;
};

}
