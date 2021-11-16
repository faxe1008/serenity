@GUI::Widget {
    fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [8]
    }

    @GUI::Label {
        fixed_height: 16
        text: "Pinned Apps:"
        text_alignment: "CenterLeft"
    }

    @GUI::Widget {
        layout: @GUI::HorizontalBoxLayout {
            spacing: 8
        }
        
        @GUI::IconView {
            name: "quicklaunch_view"
        }

        @GUI::Widget {
            shrink_to_fit: true
            layout: @GUI::VerticalBoxLayout {
            }
            @GUI::Button {
                name: "appfile_open_button"
                tooltip: "Select app file from system"
                text: "Browse..."
                shrink_to_fit: true
                min_width: 80
            }
        }
        
    }
}
