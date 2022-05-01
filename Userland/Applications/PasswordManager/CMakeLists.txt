serenity_component(
    PasswordManager
    RECOMMENDED
    TARGETS PasswordManager
)

compile_gml(PasswordManagerWindow.gml PasswordManagerWindowGML.h password_manager_window_gml)
compile_gml(EditPasswordDialog.gml EditPasswordDialogGML.h edit_password_dialog_gml)

set(SOURCES
    main.cpp
    PasswordDatabaseModel.cpp
    PasswordManagerWidget.cpp
    PasswordManagerWindowGML.h
    EditPasswordDialog.cpp
    EditPasswordDialogGML.h
)

serenity_app(PasswordManager ICON app-passwordmanager)
target_link_libraries(PasswordManager LibMain LibCore LibGUI)
