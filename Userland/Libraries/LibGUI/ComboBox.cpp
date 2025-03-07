/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGUI/Button.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/Desktop.h>
#include <LibGUI/Event.h>
#include <LibGUI/ListView.h>
#include <LibGUI/Model.h>
#include <LibGUI/Scrollbar.h>
#include <LibGUI/TextBox.h>
#include <LibGUI/TextEditor.h>
#include <LibGUI/Window.h>

REGISTER_WIDGET(GUI, ComboBox)

namespace GUI {

class ComboBoxEditor final : public TextEditor {
    C_OBJECT(ComboBoxEditor);

public:
    Function<void(int delta)> on_mousewheel;
    Function<void(KeyEvent& event)> on_keypress;

private:
    ComboBoxEditor()
        : TextEditor(TextEditor::SingleLine)
    {
    }

    virtual void mousewheel_event(MouseEvent& event) override
    {
        if (!is_focused())
            set_focus(true);
        if (on_mousewheel)
            on_mousewheel(event.wheel_delta_y());
    }

    virtual void keydown_event(KeyEvent& event) override
    {
        if (event.key() == Key_Escape) {
            if (is_focused())
                set_focus(false);
            event.accept();
        } else {
            on_keypress(event);
            TextEditor::keydown_event(event);
        }
    }
};

ComboBox::ComboBox()
{
    REGISTER_STRING_PROPERTY("placeholder", editor_placeholder, set_editor_placeholder);
    REGISTER_BOOL_PROPERTY("model_only", only_allow_values_from_model, set_only_allow_values_from_model);
    REGISTER_INT_PROPERTY("max_visible_items", max_visible_items, set_max_visible_items);

    set_min_size({ 40, 22 });
    set_preferred_size({ SpecialDimension::OpportunisticGrow, 22 });

    m_editor = add<ComboBoxEditor>();
    m_editor->set_frame_thickness(0);
    m_editor->on_return_pressed = [this] {
        if (on_return_pressed)
            on_return_pressed();
    };
    m_editor->on_up_pressed = [this] {
        navigate(AbstractView::CursorMovement::Up);
    };
    m_editor->on_down_pressed = [this] {
        navigate(AbstractView::CursorMovement::Down);
    };
    m_editor->on_pageup_pressed = [this] {
        navigate(AbstractView::CursorMovement::PageUp);
    };
    m_editor->on_pagedown_pressed = [this] {
        navigate(AbstractView::CursorMovement::PageDown);
    };
    m_editor->on_mousewheel = [this](int delta) {
        // Since we can only show one item at a time we don't want to
        // skip any. So just move one item at a time.
        navigate_relative(delta > 0 ? 1 : -1);
    };
    m_editor->on_mousedown = [this] {
        if (only_allow_values_from_model())
            m_open_button->click();
    };
    m_editor->on_keypress = [this](KeyEvent& event) {
        if (!m_only_allow_values_from_model)
            return;
        if (!m_list_window->is_visible() && event.key() <= Key_Z && event.key() >= Key_A) {
            open();
            m_list_window->event(event);
        }
    };

    m_open_button = add<Button>();
    m_open_button->set_button_style(Gfx::ButtonStyle::ThickCap);
    m_open_button->set_icon(Gfx::Bitmap::try_load_from_file("/res/icons/16x16/downward-triangle.png"sv).release_value_but_fixme_should_propagate_errors());
    m_open_button->set_focus_policy(GUI::FocusPolicy::NoFocus);
    m_open_button->on_click = [this](auto) {
        if (!m_list_view->item_count())
            return;
        if (m_list_window->is_visible())
            close();
        else
            open();
    };

    m_list_window = add<Window>(window());
    m_list_window->set_frameless(true);
    m_list_window->set_window_mode(WindowMode::CaptureInput);
    m_list_window->on_active_input_change = [this](bool is_active_input) {
        if (!is_active_input) {
            m_open_button->set_enabled(false);
            close();
        }
        m_open_button->set_enabled(true);
    };
    m_list_window->on_input_preemption = [this](auto) { close(); };

    m_list_view = m_list_window->set_main_widget<ListView>();
    m_list_view->set_should_hide_unnecessary_scrollbars(true);
    m_list_view->set_alternating_row_colors(false);
    m_list_view->set_hover_highlighting(true);
    m_list_view->set_frame_thickness(1);
    m_list_view->set_frame_shadow(Gfx::FrameShadow::Plain);
    m_list_view->set_activates_on_selection(true);
    m_list_view->on_selection_change = [this] {
        VERIFY(model());
        const auto& index = m_list_view->selection().first();
        if (m_updating_model)
            selection_updated(index);
    };

    m_list_view->on_activation = [this](auto& index) {
        deferred_invoke([this, index] {
            selection_updated(index);
            if (on_change)
                on_change(m_editor->text(), index);
        });
        close();
    };

    m_list_view->on_escape_pressed = [this] {
        close();
    };
}

ComboBox::~ComboBox() = default;

void ComboBox::set_editor_placeholder(StringView placeholder)
{
    m_editor->set_placeholder(placeholder);
}

String const& ComboBox::editor_placeholder() const
{
    return m_editor->placeholder();
}

void ComboBox::navigate(AbstractView::CursorMovement cursor_movement)
{
    auto previous_selected = m_list_view->cursor_index();
    m_list_view->move_cursor(cursor_movement, AbstractView::SelectionUpdate::Set);
    auto current_selected = m_list_view->cursor_index();
    selection_updated(current_selected);
    if (previous_selected.row() != current_selected.row() && on_change)
        on_change(m_editor->text(), current_selected);
}

void ComboBox::navigate_relative(int delta)
{
    auto previous_selected = m_list_view->cursor_index();
    m_list_view->move_cursor_relative(delta, AbstractView::SelectionUpdate::Set);
    auto current_selected = m_list_view->cursor_index();
    selection_updated(current_selected);
    if (previous_selected.row() != current_selected.row() && on_change)
        on_change(m_editor->text(), current_selected);
}

void ComboBox::selection_updated(ModelIndex const& index)
{
    if (index.is_valid())
        m_selected_index = index;
    else
        m_selected_index.clear();
    auto new_value = index.data().to_string();
    m_editor->set_text(new_value);
    if (!m_only_allow_values_from_model)
        m_editor->select_all();
}

void ComboBox::resize_event(ResizeEvent& event)
{
    Widget::resize_event(event);
    int button_height = event.size().height() - frame_thickness() * 2;
    int button_width = 15;
    m_open_button->set_relative_rect(width() - button_width - frame_thickness(), frame_thickness(), button_width, button_height);
    auto editor_rect = frame_inner_rect();
    editor_rect.set_width(editor_rect.width() - button_width);
    m_editor->set_relative_rect(editor_rect);
}

void ComboBox::set_model(NonnullRefPtr<Model> model)
{
    TemporaryChange change(m_updating_model, true);
    m_selected_index.clear();
    m_list_view->set_model(move(model));
}

void ComboBox::clear_selection()
{
    m_selected_index.clear();
    m_editor->clear_selection();
    m_editor->clear();
}

void ComboBox::set_selected_index(size_t index, AllowCallback allow_callback)
{
    if (!m_list_view->model())
        return;
    size_t previous_index = selected_index();
    TemporaryChange change(m_updating_model, true);
    auto model_index = m_list_view->model()->index(index, 0);
    m_list_view->set_cursor(model_index, AbstractView::SelectionUpdate::Set);
    selection_updated(model_index);
    if (previous_index != selected_index() && on_change && allow_callback == AllowCallback::Yes)
        on_change(m_editor->text(), m_list_view->cursor_index());
}

size_t ComboBox::selected_index() const
{
    return m_selected_index.has_value() ? m_selected_index.value().row() : 0;
}

void ComboBox::select_all()
{
    m_editor->select_all();
}

void ComboBox::open()
{
    m_editor->set_focus(true);

    // Force content size update while invisible
    m_list_view->resize({});

    auto frame = m_list_view->frame_thickness() * 2;
    auto max_height = min(m_list_view->item_height() * m_max_visible_items, m_list_view->content_height());
    Gfx::IntSize size { max(width(), m_list_view->content_width() + frame), max_height + frame };
    Gfx::IntRect rect { screen_relative_rect().bottom_left(), size };

    auto desktop = Desktop::the().rect().shrunken(0, 0, Desktop::the().taskbar_height(), 0);
    auto min_height = 5 * m_list_view->item_height() + frame;
    auto go_upwards_instead = rect.bottom() >= desktop.height() && rect.intersected(desktop).height() < min_height;
    if (go_upwards_instead) {
        auto origin = screen_relative_rect().top_left();
        rect = { Gfx::IntPoint { origin.x(), origin.y() - size.height() }, size };
    }
    rect.intersect(desktop);
    m_list_window->set_rect(rect);

    if (m_selected_index.has_value())
        m_list_view->set_cursor(m_selected_index.value(), AbstractView::SelectionUpdate::Set);

    m_list_window->show();
}

void ComboBox::close()
{
    m_list_window->hide();
}

String ComboBox::text() const
{
    return m_editor->text();
}

void ComboBox::set_text(String const& text, AllowCallback allow_callback)
{
    m_editor->set_text(text, allow_callback);
}

void ComboBox::set_only_allow_values_from_model(bool b)
{
    if (m_only_allow_values_from_model == b)
        return;
    m_only_allow_values_from_model = b;
    m_editor->set_mode(m_only_allow_values_from_model ? TextEditor::DisplayOnly : TextEditor::Editable);
}

Model* ComboBox::model()
{
    return m_list_view->model();
}

Model const* ComboBox::model() const
{
    return m_list_view->model();
}

int ComboBox::model_column() const
{
    return m_list_view->model_column();
}

void ComboBox::set_model_column(int column)
{
    m_list_view->set_model_column(column);
}

}
