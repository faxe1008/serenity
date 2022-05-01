@GUI::Frame {
    fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [10]
    }


    @GUI::Widget {
        shrink_to_fit: true
        layout: @GUI::HorizontalBoxLayout {
            spacing: 16
        }
        @GUI::Label {
            text: "Title:"
            text_alignment: "CenterLeft"
            fixed_height: 32
            fixed_width: 80
        }
        @GUI::TextBox {
            name: "title_textbox"
        }
    }
    @GUI::Widget {
        shrink_to_fit: true
        layout: @GUI::HorizontalBoxLayout {
            spacing: 16
        }
        @GUI::Label {
            text: "User Name:"
            text_alignment: "CenterLeft"
            fixed_height: 32
            fixed_width: 80
        }
        @GUI::TextBox {
            name: "username_textbox"
        }
    }
    @GUI::Widget {
        shrink_to_fit: true
        layout: @GUI::HorizontalBoxLayout {
            spacing: 16
        }
        @GUI::Label {
            text: "Password:"
            text_alignment: "CenterLeft"
            fixed_height: 32
            fixed_width: 80
        }
        @GUI::PasswordBox {
            name: "password_textbox"
            show_reveal_button: true
        }
        @GUI::Button {
            name: "generate_password_button"
            fill_with_background_color: true
            icon: "/res/icons/passwordmanager/generate-password.png"
            fixed_width: 42
        }
    }
    @GUI::Widget {
        shrink_to_fit: true
        layout: @GUI::HorizontalBoxLayout {
            spacing: 16
        }
        @GUI::Label {
            text: "Repeat:"
            text_alignment: "CenterLeft"
            fixed_height: 32
            fixed_width: 80
        }
        @GUI::PasswordBox {
            name: "password_repeat_textbox"
            show_reveal_button: true
        }
    }

    @GUI::Widget {
        shrink_to_fit: true
        layout: @GUI::HorizontalBoxLayout {
            spacing: 16
        }
        @GUI::Label {
            text: "Description:"
            text_alignment: "TopLeft"
            fixed_height: 32
            fixed_width: 80
        }
        @GUI::TextEditor {
            name: "description_texteditor"
            fixed_height: 60
        }
    }

    @GUI::Widget {
        max_height: 24
        layout: @GUI::HorizontalBoxLayout {
            margins: [4]
        }

        @GUI::Widget {}

        @GUI::Button {
            name: "save_button"
            text: "Save"
            max_width: 75
        }

        @GUI::Button {
            name: "cancel_button"
            text: "Cancel"
            max_width: 75
        }
    }
}
