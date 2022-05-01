@GUI::Frame {
    fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        spacing: 2
    }

    @GUI::TableView {
        name: "database_view"
    }


    @GUI::Statusbar {
        name: "statusbar"
    }
}
