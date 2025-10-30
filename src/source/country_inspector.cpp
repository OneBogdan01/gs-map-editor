#include "country_inspector.hpp"
#include "country_data.hpp"
#include "godot_cpp/classes/button.hpp"
#include "godot_cpp/classes/color_picker.hpp"
#include "godot_cpp/classes/h_box_container.hpp"
#include "godot_cpp/classes/item_list.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/line_edit.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/popup_menu.hpp"
#include "godot_cpp/classes/popup_panel.hpp"
#include "godot_cpp/classes/rich_text_label.hpp"
#include "godot_cpp/classes/scroll_container.hpp"
#include "godot_cpp/classes/timer.hpp"
#include "godot_cpp/classes/tree.hpp"
#include "godot_cpp/classes/v_box_container.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/color.hpp"
using namespace godot;

namespace
{
TreeItem *find_country_item(TreeItem *root, const String &country_id)
{
	if (root == nullptr)
	{
		return nullptr;
	}

	TreeItem *child = root->get_first_child();
	while (child != nullptr)
	{
		if (child->get_metadata(0) == country_id)
		{
			return child;
		}
		child = child->get_next();
	}

	return nullptr;
}
} // namespace

void CountryInspector::on_parse_button_pressed()
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
void CountryInspector::on_search_timer_timeout()
{
	update_display(pending_search_term);
}

void CountryInspector::create_containers()
{
	search_timer = memnew(Timer);
	search_timer->set_wait_time(search_delay);
	search_timer->set_one_shot(true);
	search_timer->connect("timeout", callable_mp(this, &CountryInspector::on_search_timer_timeout));

	parse_button = memnew(Button);
	parse_button->set_text("Parse All Files");
	parse_button->connect("pressed", callable_mp(this, &CountryInspector::on_parse_button_pressed));
	add_custom_control(parse_button);

	search_container = memnew(HBoxContainer);

	search_label = memnew(Label);
	search_label->set_text("Filter:");
	search_label->set_custom_minimum_size(Vector2(50, 0));
	search_container->add_child(search_label);

	search_line_edit = memnew(LineEdit);
	search_line_edit->set_placeholder("Search entities or provinces...");
	search_line_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	search_line_edit->connect("text_changed", callable_mp(this, &CountryInspector::on_search_text_changed));
	search_container->add_child(search_line_edit);

	clear_button = memnew(Button);
	clear_button->set_text("Clear");
	clear_button->connect("pressed", callable_mp(this, &CountryInspector::on_clear_search));
	search_container->add_child(clear_button);
	search_container->add_child(search_timer);
	add_custom_control(search_container);

	scroll_container = memnew(ScrollContainer);
	scroll_container->set_custom_minimum_size(Vector2i(200, 600));
	scroll_container->set_v_size_flags(Control::SIZE_EXPAND_FILL);

	data_container = memnew(VBoxContainer);
	data_container->set_name("ParsedDataContainer");
	data_container->set_custom_minimum_size(Vector2(400, 0));
	data_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	data_container->set_v_size_flags(Control::SIZE_EXPAND_FILL);

	scroll_container->add_child(data_container);
	add_custom_control(scroll_container);
}

void CountryInspector::cache_display_data()
{
	display_data.clear();
	TypedDictionary<String, Color> country_name_color = country_data->get_country_name_to_color();
	TypedDictionary<String, String> country_id_name = country_data->get_country_id_to_country_name();

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
	Tree *tree_display = nullptr;
	TypedArray<Node> children = data_container->get_children();
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

		// signals
		tree_display->connect("item_edited", callable_mp(this, &CountryInspector::on_tree_item_edited));
		tree_display->connect("item_activated", callable_mp(this, &CountryInspector::on_tree_item_rmb_selected));

		data_container->add_child(tree_display);
	}

	tree_display->clear();

	TreeItem *root = tree_display->create_item();

	TypedDictionary<String, Color> country_name_color = country_data->get_country_name_to_color();
	// disabled all input for empty color cells
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
	bool is_searching = !search_lower.is_empty();

	for (const String &country_id : display_data.keys())
	{
		Dictionary country_info = display_data[country_id];
		String country_name = country_info["name"];
		Color country_color = country_info["color"];
		PackedStringArray provinces = country_info["provinces"];

		String country_id_lower = country_id.to_lower();
		String country_name_lower = country_name.to_lower();

		bool country_id_matches = is_searching && country_id_lower.contains(search_lower);
		bool country_name_matches = is_searching && country_name_lower.contains(search_lower);
		bool country_matches = !is_searching || country_id_matches || country_name_matches;

		PackedStringArray matching_provinces;
		if (is_searching && !country_matches)
		{
			for (const String &province : provinces)
			{
				if (province.to_lower().contains(search_lower))
				{
					matching_provinces.append(province);
				}
			}
		}
		else
		{
			// Do not search provinces or the country if they do not match.
			matching_provinces = provinces;
		}

		// Skip country or its provinces if they do not match.
		if (is_searching && !country_matches && matching_provinces.size() == 0)
		{
			continue;
		}

		String country_display_text = country_name + " (ID: " + country_id + ")";
		if (is_searching && (country_id_matches || country_name_matches))
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

		for (const String &province : matching_provinces)
		{
			TreeItem *province_item = tree_display->create_item(country_item);

			String province_display_text = province;
			if (is_searching && !country_matches && province.to_lower().contains(search_lower))
			{
				province_display_text += " [MATCH]";
			}

			province_item->set_text(0, province_display_text);
			province_item->set_expand_right(0, true);
			province_item->set_metadata(0, province);

			province_item->set_metadata(1, country_id);
			province_item->set_selectable(1, false);
			province_item->set_editable(1, false);
			province_item->set_text(1, "");
			province_item->set_custom_bg_color(1, Color(0, 0, 0, 0));
		}

		if (is_searching)
		{
			country_item->set_collapsed(false);
		}
		else
		{
			country_item->set_collapsed(true);
		}
	}
}
void CountryInspector::on_tree_item_edited()
{
	Tree *tree = Object::cast_to<Tree>(data_container->get_child(0));
	if (tree == nullptr)
	{
		return;
	}

	TreeItem *item = tree->get_edited();
	if (item == nullptr)
	{
		return;
	}

	int column = tree->get_edited_column();
	if (column != 1)
	{
		return;
	}

	String item_type = item->get_metadata(1);
	if (item_type != "country")
	{
		return;
	}

	String country_id = item->get_metadata(0);
	Color current_color = item->get_custom_bg_color(1);

	ColorPicker *color_picker = memnew(ColorPicker);
	color_picker->set_pick_color(current_color);
	color_picker->set_edit_alpha(false);
	color_picker->set_color_mode(ColorPicker::MODE_RGB);

	PopupPanel *popup = memnew(PopupPanel);
	popup->add_child(color_picker);
	data_container->add_child(popup);
	country_color_save = "";
	color_picker->connect("color_changed", callable_mp(this, &CountryInspector::on_color_changed).bind(item, country_id));
	popup->connect("popup_hide", callable_mp(this, &CountryInspector::on_color_picker_closed).bind(popup));

	Vector2 mouse_pos = tree->get_screen_position() + tree->get_local_mouse_position();
	popup->set_position(mouse_pos);
	popup->popup();
}

void CountryInspector::on_tree_item_rmb_selected()
{
	Tree *tree = Object::cast_to<Tree>(data_container->get_child(0));
	TreeItem *selected = tree->get_selected();
	TreeItem *parent = selected->get_parent();
	// Do not make a pop-up unless it is a province.
	if ((parent == nullptr) || parent == tree->get_root())
	{
		return;
	}
	Rect2 item_rect = tree->get_item_area_rect(selected);
	Vector2 position = tree->get_screen_position() + item_rect.position;

	String province_id = selected->get_metadata(0);
	String current_country_id = selected->get_metadata(1);
	show_province_context_menu(position, province_id, current_country_id, selected);
}

void CountryInspector::show_province_context_menu(Vector2 position, const String &province_id, const String &current_country_id, TreeItem *province_item)
{
	PopupPanel *popup = memnew(PopupPanel);
	popup->set_size(Vector2(300, 400));

	VBoxContainer *vbox = memnew(VBoxContainer);
	popup->add_child(vbox);

	Label *title = memnew(Label);
	title->set_text("Transfer Province to:");
	vbox->add_child(title);

	LineEdit *search_bar = memnew(LineEdit);
	search_bar->set_placeholder("Search countries...");
	search_bar->set_clear_button_enabled(true);
	vbox->add_child(search_bar);

	ItemList *country_list = memnew(ItemList);
	country_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	country_list->set_meta("current_country_id", current_country_id);
	vbox->add_child(country_list);

	// Populate the list with all countries except the currently opened one.
	for (const String &country_id : display_data.keys())
	{
		if (country_id == current_country_id)
		{
			continue;
		}
		Dictionary country_info = display_data[country_id];
		String country_name = country_info["name"];
		int idx = country_list->add_item(country_name);
		country_list->set_item_metadata(idx, country_id);
	}

	search_bar->connect("text_changed", callable_mp(this, &CountryInspector::on_country_search_changed).bind(country_list));

	country_list->connect("item_activated", callable_mp(this, &CountryInspector::on_country_transfer_selected).bind(province_id, current_country_id, province_item, popup));

	popup->connect("popup_hide", callable_mp(this, &CountryInspector::on_transfer_popup_closed).bind(popup));

	data_container->add_child(popup);
	popup->set_position(position);
	popup->popup();

	search_bar->grab_focus();
}
void CountryInspector::on_country_search_changed(const String &search_text, ItemList *country_list)
{
	String search_lower = search_text.to_lower();

	String current_country_id = country_list->get_meta("current_country_id", "");

	country_list->clear();

	for (const String &country_id : display_data.keys())
	{
		if (country_id == current_country_id)
		{
			continue; // Skip current owner
		}

		Dictionary country_info = display_data[country_id];
		String country_name = country_info["name"];

		if (search_lower.is_empty() || country_name.to_lower().contains(search_lower))
		{
			int idx = country_list->add_item(country_name);
			country_list->set_item_metadata(idx, country_id);
		}
	}
}
void CountryInspector::on_country_transfer_selected(int index, const String &province_id, const String &current_country_id, TreeItem *province_item, PopupPanel *popup)
{
	ItemList *country_list = Object::cast_to<ItemList>(popup->get_child(0)->get_child(2));

	if ((country_list == nullptr) || country_list->is_item_disabled(index))
	{
		return;
	}

	String new_country_id = country_list->get_item_metadata(index);

	country_data->get_province_id_to_owner()[province_id.to_int()] = new_country_id;

	country_data->export_owner_data(province_id.to_int());
	// Update display_data cache with new data, so we do not have to recalculate everything.
	if (display_data.has(current_country_id))
	{
		Dictionary old_country_info = display_data[current_country_id];
		PackedStringArray old_provinces = old_country_info["provinces"];

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

		new_provinces.append(province_id);
		new_country_info["provinces"] = new_provinces;
		display_data[new_country_id] = new_country_info;
	}
	// Update the tree container:
	Tree *tree_display = Object::cast_to<Tree>(data_container->get_children()[0]);
	if (tree_display == nullptr)
	{
		popup->hide();
		return;
	}

	TreeItem *root = tree_display->get_root();
	if (root == nullptr)
	{
		popup->hide();
		return;
	}
	TreeItem *old_country_item = find_country_item(root, current_country_id);
	if (old_country_item != nullptr)
	{
		TreeItem *child = old_country_item->get_first_child();
		while (child != nullptr)
		{
			if (child->get_metadata(0) == province_id)
			{
				old_country_item->remove_child(child);
				break;
			}
			child = child->get_next();
		}
	}

	TreeItem *new_country_item = find_country_item(root, new_country_id);
	if (new_country_item != nullptr)
	{
		TreeItem *new_province_item = tree_display->create_item(new_country_item);
		new_province_item->set_text(0, province_id);
		new_province_item->set_expand_right(0, true);
		new_province_item->set_metadata(0, province_id);
		new_province_item->set_metadata(1, new_country_id);
		new_province_item->set_selectable(1, false);
		new_province_item->set_editable(1, false);
		new_province_item->set_text(1, "");
		new_province_item->set_custom_bg_color(1, Color(0, 0, 0, 0));
	}
	popup->hide();
}

void CountryInspector::on_transfer_popup_closed(PopupPanel *popup)
{
	popup->queue_free();
}
void CountryInspector::on_color_changed(Color new_color, TreeItem *item, const String &country_id)
{
	item->set_custom_bg_color(1, new_color);

	String country_name = item->get_text(0).split(" (ID:")[0];

	TypedDictionary<String, Color> name_color = country_data->get_country_name_to_color();
	if (name_color.has(country_name))
	{
		country_color_save = country_name;
		name_color[country_name] = new_color;

		if (display_data.has(country_id))
		{
			Dictionary country_info = display_data[country_id];
			country_info["color"] = new_color;
			display_data[country_id] = country_info;
		}
	}
}

void CountryInspector::on_color_picker_closed(PopupPanel *popup)
{
	if (country_color_save.is_empty() == false)
	{
		country_data->export_color_data(country_color_save);
	}
	popup->queue_free();
}

void CountryInspector::on_context_menu_closed(PopupMenu *menu)
{
	menu->queue_free();
}
