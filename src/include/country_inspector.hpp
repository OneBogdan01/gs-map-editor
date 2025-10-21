#pragma once

#include "godot_cpp/classes/editor_inspector_plugin.hpp"
#include "godot_cpp/classes/item_list.hpp"
#include "godot_cpp/classes/line_edit.hpp"
#include "godot_cpp/classes/popup_panel.hpp"
#include "godot_cpp/classes/timer.hpp"
#include "godot_cpp/classes/tree_item.hpp"
#include "godot_cpp/classes/v_box_container.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include <cstddef>
#include <cstdint>
namespace godot
{
class CountryData;
class CountryInspector : public EditorInspectorPlugin
{
	GDCLASS(CountryInspector, EditorInspectorPlugin);

public:
	CountryInspector() = default;
	~CountryInspector() override = default;

protected:
	// Getters and setters
	LineEdit *get_search_line_edit();
	void set_search_line_edit(LineEdit *value);
	VBoxContainer *get_data_container();
	void set_data_container(VBoxContainer *value);
	CountryData *get_country_data();
	void update_display(const String &search_term);
	void set_country_data(CountryData *data);
	// call on parse begin
	void create_containers();
	void on_parse_button_pressed();
	void on_search_text_changed(const String &search_term);
	void on_clear_search();
	void on_tree_item_edited();
	void on_tree_item_rmb_selected();
	// Show province transfer popup with search
	void show_province_context_menu(Vector2 position, const String &province_id, const String &current_country_id, TreeItem *province_item);

	// Filter countries based on search text
	void on_country_search_changed(const String &search_text, ItemList *country_list);

	// Handle country selection from the list
	void on_country_transfer_selected(int index, const String &province_id, const String &current_country_id, TreeItem *province_item, PopupPanel *popup);

	// Cleanup when transfer popup is closed
	void on_transfer_popup_closed(PopupPanel *popup);

	void on_color_changed(Color new_color, TreeItem *item, const String &country_id);
	void on_color_picker_closed(PopupPanel *popup);
	void on_context_menu_closed(PopupMenu *menu);
	void on_search_timer_timeout();
	// Called whenever the data is changed, it is used to for update_display function.
	void cache_display_data();
	static void _bind_methods();

	CountryData *country_data{ nullptr };
	LineEdit *search_line_edit{ nullptr };
	VBoxContainer *data_container{ nullptr };
	Timer *search_timer{ nullptr };

private:
	Dictionary display_data;
	String pending_search_term;
	String country_color_save;
};
} // namespace godot
