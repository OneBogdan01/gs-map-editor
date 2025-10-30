#pragma once
#include "godot_cpp/classes/editor_inspector_plugin.hpp"

#include "godot_cpp/classes/line_edit.hpp"
#include "godot_cpp/classes/popup_menu.hpp"
#include "godot_cpp/classes/v_box_container.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"

namespace godot
{
class CountryData;
class Button;
class HBoxContainer;
class Label;
class ScrollContainer;
class Timer;
class LineEdit;
class VBoxContainer;
class PopupPanel;
class ItemList;
class TreeItem;

class CountryInspector : public EditorInspectorPlugin
{
	GDCLASS(CountryInspector, EditorInspectorPlugin);

public:
	// Getters and setters
	LineEdit *get_search_line_edit();
	void set_search_line_edit(LineEdit *data);
	VBoxContainer *get_data_container();
	void set_data_container(VBoxContainer *data);
	CountryData *get_country_data();
	void set_country_data(CountryData *data);
	Button *get_parse_button();
	void set_parse_button(Button *data);
	HBoxContainer *get_search_container();
	void set_search_container(HBoxContainer *data);
	Label *get_search_label();
	void set_search_label(Label *data);
	Button *get_clear_button();
	void set_clear_button(Button *data);
	ScrollContainer *get_scroll_container();
	void set_scroll_container(ScrollContainer *data);
	Timer *get_search_timer();
	void set_search_timer(Timer *data);
	float get_search_delay();
	void set_search_delay(float data);
	// call on parse begin
	void create_containers();

protected:
	static void _bind_methods();

private:
	// Used as callables that are triggered by user interaction.

	void on_parse_button_pressed();
	void on_search_text_changed(const String &search_term);
	void on_clear_search();
	void on_tree_item_edited();
	void on_tree_item_rmb_selected();
	void on_country_search_changed(const String &search_text, ItemList *country_list);
	void on_country_transfer_selected(int index, const String &province_id, const String &current_country_id, TreeItem *province_item, PopupPanel *popup);
	void on_transfer_popup_closed(PopupPanel *popup);
	void on_color_changed(Color new_color, TreeItem *item, const String &country_id);
	void on_color_picker_closed(PopupPanel *popup);
	void on_context_menu_closed(PopupMenu *menu);
	void on_search_timer_timeout();
	// This context menu is used to transfer provinces to other countries.
	void show_province_context_menu(Vector2 position, const String &province_id, const String &current_country_id, TreeItem *province_item);
	// Updates the tree structure that is visible in the editor.
	void update_display(const String &search_term);
	// Called whenever the data is changed, it is used for update_display function.
	void cache_display_data();

	Button *parse_button{};
	HBoxContainer *search_container{};
	Label *search_label{};
	LineEdit *search_line_edit{};
	Button *clear_button{};
	ScrollContainer *scroll_container{};
	VBoxContainer *data_container{};
	CountryData *country_data{};
	Timer *search_timer{};

	Dictionary display_data;
	String pending_search_term;
	String country_color_save;
	float search_delay = 0.3;
};
} // namespace godot
