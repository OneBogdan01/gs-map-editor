#include "country_data.hpp"
#include "country_inspector.hpp"
#include "godot_cpp/classes/button.hpp"
#include "godot_cpp/classes/h_box_container.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/line_edit.hpp"
#include "godot_cpp/classes/scroll_container.hpp"
#include "godot_cpp/classes/v_box_container.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/string.hpp"
#include <godot_cpp/classes/editor_inspector_plugin.hpp>

using namespace godot;
Button *CountryInspector::get_parse_button()
{
	return parse_button;
}

void CountryInspector::set_parse_button(Button *data)
{
	parse_button = data;
}

HBoxContainer *CountryInspector::get_search_container()
{
	return search_container;
}

void CountryInspector::set_search_container(HBoxContainer *data)
{
	search_container = data;
}

Label *CountryInspector::get_search_label()
{
	return search_label;
}

void CountryInspector::set_search_label(Label *data)
{
	search_label = data;
}

LineEdit *CountryInspector::get_search_line_edit()
{
	return search_line_edit;
}

void CountryInspector::set_search_line_edit(LineEdit *data)
{
	search_line_edit = data;
}

Button *CountryInspector::get_clear_button()
{
	return clear_button;
}

void CountryInspector::set_clear_button(Button *data)
{
	clear_button = data;
}

ScrollContainer *CountryInspector::get_scroll_container()
{
	return scroll_container;
}

void CountryInspector::set_scroll_container(ScrollContainer *data)
{
	scroll_container = data;
}
float CountryInspector::get_search_delay()
{
	return search_delay;
}
void CountryInspector::set_search_delay(float data)
{
	search_delay = data;
}
VBoxContainer *CountryInspector::get_data_container()
{
	return data_container;
}

void CountryInspector::set_data_container(VBoxContainer *data)
{
	data_container = data;
}
void CountryInspector::set_country_data(CountryData *data)
{
	if (country_data == nullptr)
	{
		country_data = data;
		// prepare data for the first time
		if (country_data->get_country_name_to_color().is_empty())
		{
			on_parse_button_pressed();
			return;
		}
		cache_display_data();
	}

	update_display("");
}
CountryData *CountryInspector::get_country_data()
{
	return country_data;
}

void CountryInspector::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("create_containers"), &CountryInspector::create_containers);

	// getters setters

	ClassDB::bind_method(D_METHOD("get_country_data"), &CountryInspector::get_country_data);
	ClassDB::bind_method(D_METHOD("set_country_data", "data"), &CountryInspector::set_country_data);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "country_data", PROPERTY_HINT_RESOURCE_TYPE, "CountryData"), "set_country_data", "get_country_data");

	ClassDB::bind_method(D_METHOD("get_parse_button"), &CountryInspector::get_parse_button);
	ClassDB::bind_method(D_METHOD("set_parse_button", "data"), &CountryInspector::set_parse_button);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "parse_button", PROPERTY_HINT_RESOURCE_TYPE, "Button"), "set_parse_button", "get_parse_button");

	ClassDB::bind_method(D_METHOD("get_search_container"), &CountryInspector::get_search_container);
	ClassDB::bind_method(D_METHOD("set_search_container", "data"), &CountryInspector::set_search_container);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "search_container", PROPERTY_HINT_RESOURCE_TYPE, "HBoxContainer"), "set_search_container", "get_search_container");

	ClassDB::bind_method(D_METHOD("get_search_label"), &CountryInspector::get_search_label);
	ClassDB::bind_method(D_METHOD("set_search_label", "data"), &CountryInspector::set_search_label);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "search_label", PROPERTY_HINT_RESOURCE_TYPE, "Label"), "set_search_label", "get_search_label");

	ClassDB::bind_method(D_METHOD("get_search_line_edit"), &CountryInspector::get_search_line_edit);
	ClassDB::bind_method(D_METHOD("set_search_line_edit", "data"), &CountryInspector::set_search_line_edit);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "search_line_edit", PROPERTY_HINT_RESOURCE_TYPE, "LineEdit"), "set_search_line_edit", "get_search_line_edit");

	ClassDB::bind_method(D_METHOD("get_clear_button"), &CountryInspector::get_clear_button);
	ClassDB::bind_method(D_METHOD("set_clear_button", "data"), &CountryInspector::set_clear_button);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "clear_button", PROPERTY_HINT_RESOURCE_TYPE, "Button"), "set_clear_button", "get_clear_button");

	ClassDB::bind_method(D_METHOD("get_scroll_container"), &CountryInspector::get_scroll_container);
	ClassDB::bind_method(D_METHOD("set_scroll_container", "data"), &CountryInspector::set_scroll_container);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "scroll_container", PROPERTY_HINT_RESOURCE_TYPE, "ScrollContainer"), "set_scroll_container", "get_scroll_container");

	ClassDB::bind_method(D_METHOD("get_search_delay"), &CountryInspector::get_search_delay);
	ClassDB::bind_method(D_METHOD("set_search_delay", "data"), &CountryInspector::set_search_delay);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "search_delay"), "set_search_delay", "get_search_delay");

	ClassDB::bind_method(D_METHOD("get_data_container"), &CountryInspector::get_data_container);
	ClassDB::bind_method(D_METHOD("set_data_container", "data"), &CountryInspector::set_data_container);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "data_container", PROPERTY_HINT_RESOURCE_TYPE, "VBoxContainer"), "set_data_container", "get_data_container");
}