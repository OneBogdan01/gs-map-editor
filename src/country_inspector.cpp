#include "country_inspector.h"
#include "country_data.h"
#include "godot_cpp/classes/button.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/line_edit.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/rich_text_label.hpp"
#include "godot_cpp/classes/time.hpp"
#include "godot_cpp/classes/tree.hpp"
#include "godot_cpp/classes/v_box_container.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/color_names.inc.hpp"
#include "godot_cpp/variant/string.hpp"
#include <godot_cpp/classes/editor_inspector_plugin.hpp>
using namespace godot;

void CountryInspector::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("on_parse_button_pressed", "data"), &CountryInspector::on_parse_button_pressed);
	ClassDB::bind_method(D_METHOD("create_containers", "data"), &CountryInspector::create_containers);
	ClassDB::bind_method(D_METHOD("on_search_text_changed", "search_term"), &CountryInspector::on_search_text_changed);
	ClassDB::bind_method(D_METHOD("on_clear_search"), &CountryInspector::on_clear_search);
	ClassDB::bind_method(D_METHOD("update_display", "search_term"), &CountryInspector::update_display);

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
	update_display("");
}
void CountryInspector::on_search_text_changed(const String &search_term)
{
	UtilityFunctions::print_verbose("Search term was changed to:", search_term);
}

void CountryInspector::on_clear_search()
{
	UtilityFunctions::print("Button pressed!");
}

void CountryInspector::create_containers(CountryData *data)
{
	country_data = data;
	search_line_edit = memnew(LineEdit);
	data_container = memnew(VBoxContainer);
	// if (country_data->get_country_id_to_color().is_empty())
	// {
	// 	country_data->parse_all_files();
	// }
	UtilityFunctions::print_verbose("Inspector initialized containers");
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
		tree_display->set_hide_root(true);
		tree_display->set_custom_minimum_size(Vector2(0, 400));
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
		return;
	}

	auto country_id_name = country_data->get_country_id_to_country_name();

	for (const String &country_id : country_id_name.keys())
	{
		String country_name = country_id_name[country_id];
		Color country_color = country_name_color.get(country_name, Color(1, 1, 1));

		TreeItem *country_item = tree_display->create_item(root);
		country_item->set_text(0, country_name + " (ID: " + country_id + ")");
		country_item->set_custom_color(0, country_color);

		PackedStringArray provinces = country_data->get_country_provinces(country_id);
		for (const auto &province : provinces)
		{
			TreeItem *province_item = tree_display->create_item(country_item);
			province_item->set_text(0, province);
		}

		country_item->set_collapsed(true);
	}

	uint64_t end_time = Time::get_singleton()->get_ticks_usec();
	UtilityFunctions::print("TOTAL update_display: ", (end_time - start_time) / 1000.0, " ms");
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
