serenity_component(
    TaskbarSettings
    REQUIRED
    TARGETS TaskbarSettings
)

compile_gml(QuickLaunchSettings.gml QuickLaunchSettingsGML.h quick_launch_settings_gml)

set(SOURCES
    QuickLaunchSettingsWidget.cpp
    QuickLaunchSettingsGML.h
    main.cpp
)

serenity_app(TaskbarSettings ICON ladyball)
target_link_libraries(TaskbarSettings LibDesktop LibGUI LibConfig)
