#include "country_inspector.h"
#include "country_data.h"
#include "godot_cpp/classes/button.hpp"
#include "godot_cpp/classes/color_picker.hpp"
#include "godot_cpp/classes/item_list.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/line_edit.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/popup_menu.hpp"
#include "godot_cpp/classes/rich_text_label.hpp"
#include "godot_cpp/classes/time.hpp"
#include "godot_cpp/classes/tree.hpp"
#include "godot_cpp/classes/v_box_container.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/color_names.inc.hpp"
#include "godot_cpp/variant/string.hpp"
#include <cstdint>
#include <godot_cpp/classes/editor_inspector_plugin.hpp>
using namespace godot;

void CountryInspector::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("on_parse_button_pressed", "data"), &CountryInspector::on_parse_button_pressed);
	ClassDB::bind_method(D_METHOD("create_containers", "data"), &CountryInspector::create_containers);
	ClassDB::bind_method(D_METHOD("on_search_text_changed", "search_term"), &CountryInspector::on_search_text_changed);
	ClassDB::bind_method(D_METHOD("on_clear_search"), &CountryInspector::on_clear_search);
	ClassDB::bind_method(D_METHOD("update_display", "search_term"), &CountryInspector::update_display);

	ClassDB::bind_method(D_METHOD("_on_tree_item_edited"), &CountryInspector::_on_tree_item_edited);
	ClassDB::bind_method(D_METHOD("_on_tree_item_rmb_selected"), &CountryInspector::_on_tree_item_rmb_selected);
	ClassDB::bind_method(D_METHOD("_on_province_context_menu_selected", "id", "province_id", "old_country_id", "province_item"), &CountryInspector::_on_province_context_menu_selected);
	ClassDB::bind_method(D_METHOD("_on_color_changed", "new_color", "item", "country_id"), &CountryInspector::_on_color_changed);
	ClassDB::bind_method(D_METHOD("_on_color_picker_closed", "popup"), &CountryInspector::_on_color_picker_closed);
	ClassDB::bind_method(D_METHOD("_on_context_menu_closed", "menu"), &CountryInspector::_on_context_menu_closed);
	// getters setters
	ClassDB::bind_method(D_METHOD("get_search_line_edit"), &CountryInspector::get_search_line_edit);
	ClassDB::bind_method(D_METHOD("set_search_line_edit", "data"), &CountryInspector::set_search_line_edit);
	ClassDB::bind_method(D_METHOD("get_data_container"), &CountryInspector::get_data_container);
	ClassDB::bind_method(D_METHOD("set_data_container", "data"), &CountryInspector::set_data_container);
	ClassDB::bind_method(D_METHOD("get_country_data"), &CountryInspector::get_country_data);
}
void CountryInspector::on_parse_button_pressed(CountryData *data)
{
	country_data->parse_all_files();
	cache_display_data();
	update_display("");
}
void CountryInspector::on_search_text_changed(const String &search_term)
{
	if (pending_search_term == search_term)
	{
		return;
	}
	UtilityFunctions::print_verbose("Search term was changed to:", search_term);
	pending_search_term = search_term;
	search_timer->start();
}

void CountryInspector::on_clear_search()
{
	UtilityFunctions::print_verbose("Cleared search term");
	search_line_edit->clear();
	pending_search_term = "";
	update_display(pending_search_term);
}
void CountryInspector::_on_search_timer_timeout()
{
	update_display(pending_search_term);
}
void CountryInspector::create_containers(CountryData *data)
{
	country_data = data;
	search_line_edit = memnew(LineEdit);
	data_container = memnew(VBoxContainer);
	search_timer = memnew(Timer);
	search_timer->set_wait_time(0.3); // 300ms delay
	search_timer->set_one_shot(true);
	search_timer->connect("timeout", callable_mp(this, &CountryInspector::_on_search_timer_timeout));
	search_line_edit->add_child(search_timer);
	//
	data_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	data_container->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	// if (country_data->get_country_id_to_color().is_empty())
	// {
	// 	country_data->parse_all_files();
	// }
	UtilityFunctions::print_verbose("Inspector initialized containers");
}
void CountryInspector::cache_display_data()
{
	display_data.clear();
	auto country_name_color = country_data->get_country_name_to_color();
	auto country_id_name = country_data->get_country_id_to_country_name();

	for (const String &country_id : country_id_name.keys())
	{
		Dictionary country_info;
		String country_name = country_id_name[country_id];

		country_info["name"] = country_name;
		country_info["color"] = country_name_color.get(country_name, Color(1, 1, 1));
		country_info["provinces"] = country_data->get_country_provinces(country_id);

		display_data[country_id] = country_info;
	}

	UtilityFunctions::print_verbose("Display data for ", display_data.size());
}
void CountryInspector::update_display(const String &search_term)
{
	uint64_t start_time = Time::get_singleton()->get_ticks_usec();
	Tree *tree_display = nullptr;
	auto children = data_container->get_children();
	if (children.size() > 0)
	{
		tree_display = Object::cast_to<Tree>(children[0]);
	}
	if (tree_display == nullptr)
	{
		tree_display = memnew(Tree);
		tree_display->set_columns(2);
		tree_display->set_column_expand(0, true);
		tree_display->set_column_expand(1, false);
		tree_display->set_column_custom_minimum_width(1, 50);
		tree_display->set_hide_root(true);
		tree_display->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		tree_display->set_v_size_flags(Control::SIZE_EXPAND_FILL);
		tree_display->connect("item_edited", callable_mp(this, &CountryInspector::_on_tree_item_edited));
		tree_display->connect("item_activated", callable_mp(this, &CountryInspector::_on_tree_item_rmb_selected));
		data_container->add_child(tree_display);
	}
	tree_display->clear();
	TreeItem *root = tree_display->create_item();
	auto country_name_color = country_data->get_country_name_to_color();
	if (country_name_color.size() == 0)
	{
		TreeItem *no_data = tree_display->create_item(root);
		no_data->set_text(0, "No data parsed, click 'Parse All Files' to load data.");
		no_data->set_custom_color(0, Color(1, 0, 0));
		tree_display->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
		return;
	}
	// allow input
	tree_display->set_mouse_filter(Control::MOUSE_FILTER_STOP);
	String search_lower = search_term.to_lower().strip_edges();
	bool has_search = !search_lower.is_empty();

	uint64_t populate_start = Time::get_singleton()->get_ticks_usec();
	for (const String &country_id : display_data.keys())
	{
		Dictionary country_info = display_data[country_id];
		String country_name = country_info["name"];
		Color country_color = country_info["color"];
		PackedStringArray provinces = country_info["provinces"];

		// Pre-compute lowercase versions for performance
		String country_id_lower = country_id.to_lower();
		String country_name_lower = country_name.to_lower();

		// Check if country matches search
		bool country_id_matches = has_search && country_id_lower.contains(search_lower);
		bool country_name_matches = has_search && country_name_lower.contains(search_lower);
		bool country_matches = !has_search || country_id_matches || country_name_matches;

		// Find matching provinces
		PackedStringArray matching_provinces;
		if (has_search && !country_matches)
		{
			// Only check provinces if country doesn't match
			for (const String &province : provinces)
			{
				if (province.to_lower().contains(search_lower))
				{
					matching_provinces.append(province);
				}
			}
		}
		else if (!has_search)
		{
			// No search, show all provinces
			matching_provinces = provinces;
		}
		else
		{
			// Country matches, show all provinces but mark matching ones
			matching_provinces = provinces;
		}

		// Skip country if neither it nor any provinces match
		if (has_search && !country_matches && matching_provinces.size() == 0)
		{
			continue;
		}

		// Build country display text with MATCH indicator
		String country_display_text = country_name + " (ID: " + country_id + ")";
		if (has_search && (country_id_matches || country_name_matches))
		{
			country_display_text += " [MATCH]";
		}

		TreeItem *country_item = tree_display->create_item(root);
		country_item->set_text(0, country_display_text);
		country_item->set_metadata(0, country_id);
		country_item->set_custom_bg_color(1, country_color);
		country_item->set_cell_mode(1, TreeItem::CELL_MODE_CUSTOM);
		country_item->set_editable(1, true);
		country_item->set_metadata(1, "country");

		// Add provinces
		for (const String &province : matching_provinces)
		{
			TreeItem *province_item = tree_display->create_item(country_item);

			// Add MATCH indicator if province matches search
			String province_display_text = province;
			if (has_search && !country_matches && province.to_lower().contains(search_lower))
			{
				province_display_text += " [MATCH]";
			}

			province_item->set_text(0, province_display_text);
			province_item->set_metadata(0, province);
			province_item->set_metadata(1, country_id);
			province_item->set_selectable(1, false);
		}

		// Auto-expand if searching
		if (has_search)
		{
			country_item->set_collapsed(false);
		}
		else
		{
			country_item->set_collapsed(true);
		}
	}
	uint64_t populate_end = Time::get_singleton()->get_ticks_usec();
	UtilityFunctions::print("Tree populate time: ", (populate_end - populate_start) / 1000.0, " ms");
	uint64_t end_time = Time::get_singleton()->get_ticks_usec();
	UtilityFunctions::print("TOTAL update_display: ", (end_time - start_time) / 1000.0, " ms");
}
void CountryInspector::_on_tree_item_edited()
{
	Tree *tree = Object::cast_to<Tree>(data_container->get_child(0));
	if (!tree)
	{
		return;
	}

	TreeItem *item = tree->get_edited();
	if (!item)
	{
		return;
	}

	// Get the edited column
	int column = tree->get_edited_column();
	if (column != 1)
	{
		return;
	}

	// Check if it's a country
	String item_type = item->get_metadata(1);
	if (item_type != "country")
	{
		return;
	}

	String country_id = item->get_metadata(0);
	Color current_color = item->get_custom_bg_color(1);

	// Create color picker popup
	ColorPicker *color_picker = memnew(ColorPicker);
	color_picker->set_pick_color(current_color);
	color_picker->set_edit_alpha(false);
	color_picker->set_color_mode(ColorPicker::MODE_RGB);

	PopupPanel *popup = memnew(PopupPanel);
	popup->add_child(color_picker);
	data_container->add_child(popup);
	save_color_index = -1;
	color_picker->connect("color_changed", callable_mp(this, &CountryInspector::_on_color_changed).bind(item, country_id));
	popup->connect("popup_hide", callable_mp(this, &CountryInspector::_on_color_picker_closed).bind(popup));

	Vector2 mouse_pos = tree->get_screen_position() + tree->get_local_mouse_position();
	popup->set_position(mouse_pos);
	popup->popup();
}

void CountryInspector::_on_tree_item_rmb_selected()
{
	Tree *tree = Object::cast_to<Tree>(data_container->get_child(0));
	TreeItem *selected = tree->get_selected();
	TreeItem *parent = selected->get_parent();
	if (!parent || parent == tree->get_root())
	{
		// This is a country
		return;
	}
	Rect2 item_rect = tree->get_item_area_rect(selected);
	Vector2 position = tree->get_screen_position() + item_rect.position;

	String province_id = selected->get_metadata(0);
	String current_country_id = selected->get_metadata(1);
	_show_province_context_menu(position, province_id, current_country_id, selected);
}

void CountryInspector::_show_province_context_menu(Vector2 position, const String &province_id, const String &current_country_id, TreeItem *province_item)
{
	// Create a popup panel
	PopupPanel *popup = memnew(PopupPanel);
	popup->set_size(Vector2(300, 400));

	// Create a vertical container
	VBoxContainer *vbox = memnew(VBoxContainer);
	popup->add_child(vbox);

	// Add title label
	Label *title = memnew(Label);
	title->set_text("Transfer Province to:");
	vbox->add_child(title);

	// Add search bar
	LineEdit *search_bar = memnew(LineEdit);
	search_bar->set_placeholder("Search countries...");
	search_bar->set_clear_button_enabled(true);
	vbox->add_child(search_bar);

	// Add item list for countries
	ItemList *country_list = memnew(ItemList);
	country_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	country_list->set_meta("current_country_id", current_country_id);
	vbox->add_child(country_list);

	// Populate the list
	for (const String &country_id : display_data.keys())
	{
		if (country_id == current_country_id)
		{
			continue; // Skip current owner
		}
		Dictionary country_info = display_data[country_id];
		String country_name = country_info["name"];
		int idx = country_list->add_item(country_name);
		country_list->set_item_metadata(idx, country_id);
	}

	// Connect search bar to filter function
	search_bar->connect("text_changed", callable_mp(this, &CountryInspector::_on_country_search_changed).bind(country_list));

	// Connect item selection
	country_list->connect("item_activated", callable_mp(this, &CountryInspector::_on_country_transfer_selected).bind(province_id, current_country_id, province_item, popup));

	// Connect popup close
	popup->connect("popup_hide", callable_mp(this, &CountryInspector::_on_transfer_popup_closed).bind(popup));

	data_container->add_child(popup);
	popup->set_position(position);
	popup->popup();

	// Focus the search bar
	search_bar->grab_focus();
}
void CountryInspector::_on_country_search_changed(const String &search_text, ItemList *country_list)
{
	String search_lower = search_text.to_lower();

	// Store current country_id from the popup context
	// We need to get the current_country_id that was passed to the popup
	String current_country_id = country_list->get_meta("current_country_id", "");

	// Clear and rebuild the list
	country_list->clear();

	for (const String &country_id : display_data.keys())
	{
		if (country_id == current_country_id)
		{
			continue; // Skip current owner
		}

		Dictionary country_info = display_data[country_id];
		String country_name = country_info["name"];

		// Only add if matches search or search is empty
		if (search_lower.is_empty() || country_name.to_lower().contains(search_lower))
		{
			int idx = country_list->add_item(country_name);
			country_list->set_item_metadata(idx, country_id);
		}
	}
}
void CountryInspector::_on_country_transfer_selected(int index, const String &province_id, const String &current_country_id, TreeItem *province_item, PopupPanel *popup)
{
	ItemList *country_list = Object::cast_to<ItemList>(popup->get_child(0)->get_child(2)); // Get the ItemList

	if (!country_list || country_list->is_item_disabled(index))
	{
		return;
	}

	String new_country_id = country_list->get_item_metadata(index);

	country_data->change_province_owner(province_id.to_int(), new_country_id);

	// Update display_data cache
	if (display_data.has(current_country_id))
	{
		Dictionary old_country_info = display_data[current_country_id];
		PackedStringArray old_provinces = old_country_info["provinces"];

		// Remove province from old country
		int province_index = old_provinces.find(province_id);
		if (province_index != -1)
		{
			old_provinces.remove_at(province_index);
			old_country_info["provinces"] = old_provinces;
			display_data[current_country_id] = old_country_info;
		}
	}

	if (display_data.has(new_country_id))
	{
		Dictionary new_country_info = display_data[new_country_id];
		PackedStringArray new_provinces = new_country_info["provinces"];

		// Add province to new country
		new_provinces.append(province_id);
		new_country_info["provinces"] = new_provinces;
		display_data[new_country_id] = new_country_info;
	}

	// Update the tree display
	update_display("");

	popup->hide();
}

void CountryInspector::_on_transfer_popup_closed(PopupPanel *popup)
{
	popup->queue_free();
}
void CountryInspector::_on_province_context_menu_selected(int id, const String &province_id, const String &old_country_id, TreeItem *province_item)
{
	if (id == 0)
	{
		return; // "Change Owner" header
	}

	PopupMenu *menu = Object::cast_to<PopupMenu>(province_item->get_tree()->get_viewport()->gui_get_focus_owner());
	if (!menu)
	{
		return;
	}

	String new_country_id = menu->get_item_metadata(id);

	// Update the data
	// country_data->transfer_province(province_id, old_country_id, new_country_id);

	// Refresh display
	cache_display_data();
	update_display("");

	UtilityFunctions::print("Transferred province ", province_id, " from ", old_country_id, " to ", new_country_id);
}

void CountryInspector::_on_color_changed(Color new_color, TreeItem *item, const String &country_id)
{
	item->set_custom_bg_color(1, new_color);

	String country_name = item->get_text(0).split(" (ID:")[0];
	save_color_index = country_data->set_country_color_by_name(country_name, new_color);
	if (display_data.has(country_id))
	{
		Dictionary country_info = display_data[country_id];
		country_info["color"] = new_color;
		display_data[country_id] = country_info;
	}
}

void CountryInspector::_on_color_picker_closed(PopupPanel *popup)
{
	if (save_color_index != -1)
	{
		country_data->export_color_data(save_color_index);
	}
	popup->queue_free();
}

void CountryInspector::_on_context_menu_closed(PopupMenu *menu)
{
	menu->queue_free();
}
LineEdit *CountryInspector::get_search_line_edit()
{
	return search_line_edit;
}
void CountryInspector::set_search_line_edit(LineEdit *value)
{
	search_line_edit = value;
}
VBoxContainer *CountryInspector::get_data_container()
{
	return data_container;
}
void CountryInspector::set_data_container(VBoxContainer *value)
{
	data_container = value;
}
CountryData *CountryInspector::get_country_data()
{
	return country_data;
}
