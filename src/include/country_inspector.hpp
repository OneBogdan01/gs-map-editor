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
#include "godot_cpp/variant/typed_dictionary.hpp"
#include <cstdint>
namespace godot
{
class CountryData;
class CountryInspector : public EditorInspectorPlugin
{
	GDCLASS(CountryInspector, EditorInspectorPlugin);

public:
	CountryInspector() = default;
	~CountryInspector() = default;

	// Getters and setters
	LineEdit *get_search_line_edit();
	void set_search_line_edit(LineEdit *value);
	VBoxContainer *get_data_container();
	void set_data_container(VBoxContainer *value);
	CountryData *get_country_data();

protected:
	// call on parse begin
	void create_containers(CountryData *data);
	void on_parse_button_pressed(CountryData *data);
	void on_search_text_changed(const String &search_term);
	void on_clear_search();
	void update_display(const String &search_term);
	void _on_tree_item_edited();
	void _on_tree_item_rmb_selected();
	// Show province transfer popup with search
	void _show_province_context_menu(Vector2 position, const String &province_id, const String &current_country_id, TreeItem *province_item);

	// Filter countries based on search text
	void _on_country_search_changed(const String &search_text, ItemList *country_list);

	// Handle country selection from the list
	void _on_country_transfer_selected(int index, const String &province_id, const String &current_country_id, TreeItem *province_item, PopupPanel *popup);

	// Cleanup when transfer popup is closed
	void _on_transfer_popup_closed(PopupPanel *popup);

	// Transfer province to new country (if you don't already have this)
	void _on_province_context_menu_selected(int id, const String &province_id, const String &old_country_id, TreeItem *province_item);
	void _on_color_changed(Color new_color, TreeItem *item, const String &country_id);
	void _on_color_picker_closed(PopupPanel *popup);
	void _on_context_menu_closed(PopupMenu *menu);
	void _on_search_timer_timeout();
	// Called whenever the data is changed, it is used to for update_display function.
	void cache_display_data();
	static void _bind_methods();

	CountryData *country_data{ nullptr };
	LineEdit *search_line_edit{ nullptr };
	VBoxContainer *data_container{ nullptr };

private:
	Dictionary display_data;
	int32_t save_color_index{ -1 };
	Timer *search_timer;
	String pending_search_term;
};
} // namespace godot
