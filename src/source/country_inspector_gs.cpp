#include "country_data.hpp"
#include "country_inspector.hpp"

using namespace godot;
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
	ClassDB::bind_method(D_METHOD("on_parse_button_pressed"), &CountryInspector::on_parse_button_pressed);
	ClassDB::bind_method(D_METHOD("create_containers"), &CountryInspector::create_containers);
	ClassDB::bind_method(D_METHOD("on_search_text_changed", "search_term"), &CountryInspector::on_search_text_changed);
	ClassDB::bind_method(D_METHOD("on_clear_search"), &CountryInspector::on_clear_search);
	ClassDB::bind_method(D_METHOD("update_display", "search_term"), &CountryInspector::update_display);

	ClassDB::bind_method(D_METHOD("on_tree_item_edited"), &CountryInspector::on_tree_item_edited);
	ClassDB::bind_method(D_METHOD("on_tree_item_rmb_selected"), &CountryInspector::on_tree_item_rmb_selected);
	ClassDB::bind_method(D_METHOD("on_color_changed", "new_color", "item", "country_id"), &CountryInspector::on_color_changed);
	ClassDB::bind_method(D_METHOD("on_color_picker_closed", "popup"), &CountryInspector::on_color_picker_closed);
	ClassDB::bind_method(D_METHOD("on_context_menu_closed", "menu"), &CountryInspector::on_context_menu_closed);
	// getters setters
	ClassDB::bind_method(D_METHOD("get_search_line_edit"), &CountryInspector::get_search_line_edit);
	ClassDB::bind_method(D_METHOD("set_search_line_edit", "data"), &CountryInspector::set_search_line_edit);
	ClassDB::bind_method(D_METHOD("get_data_container"), &CountryInspector::get_data_container);
	ClassDB::bind_method(D_METHOD("set_data_container", "data"), &CountryInspector::set_data_container);
	ClassDB::bind_method(D_METHOD("get_country_data"), &CountryInspector::get_country_data);
	ClassDB::bind_method(D_METHOD("set_country_data", "data"), &CountryInspector::set_country_data);
}