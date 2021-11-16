/*
 * Copyright (c) 2018-2020, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "QuickLaunchSettingsWidget.h"
#include <Applications/TaskbarSettings/QuickLaunchSettingsGML.h>
#include <LibGUI/Button.h>
#include <LibGUI/FilePicker.h>
#include <LibGUI/IconView.h>

namespace TaskbarSettings {

QuickLaunchSettingsWidget::QuickLaunchSettingsWidget()
    : m_app_model(QuickLaunchAppsModel::create(load_quick_launch_apps()))
{
    load_from_gml(quick_launch_settings_gml);
    m_apps_view = *find_descendant_of_type_named<GUI::IconView>("quicklaunch_view");
    m_apps_view->set_model(m_app_model);

    auto& button = *find_descendant_of_type_named<GUI::Button>("appfile_open_button");
    button.on_click = [this](auto) {
        auto path = GUI::FilePicker::get_open_filepath(window(), "Select app file from file system", "/res/apps");
        if (!path.has_value())
            return;
        auto app_file = Desktop::AppFile::open(path.value());
        if (app_file->is_valid())
            m_app_model->append(app_file);
    };
}

QuickLaunchSettingsWidget::~QuickLaunchSettingsWidget()
{
}

NonnullRefPtrVector<Desktop::AppFile> QuickLaunchSettingsWidget::load_quick_launch_apps()
{
    NonnullRefPtrVector<Desktop::AppFile> apps;
    auto config = Core::ConfigFile::open_for_app("Taskbar");
    for (auto& key : config->keys("QuickLaunch")) {
        auto value = config->read_entry("QuickLaunch", key);
        dbgln("{}", value);
        auto af_path = String::formatted("{}/{}", Desktop::AppFile::APP_FILES_DIRECTORY, value);
        auto af = Desktop::AppFile::open(af_path);
        if (af->is_valid()) {
            apps.append(af);
        }
    }

    return apps;
}

void QuickLaunchSettingsWidget::apply_settings()
{
    auto config = Core::ConfigFile::open_for_app("Taskbar");
}

}
