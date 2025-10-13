#include "country_inspector.h"
#include "country_data.h"
#include "godot_cpp/classes/button.hpp"
#include "godot_cpp/core/memory.hpp"
#include <godot_cpp/classes/editor_inspector_plugin.hpp>
using namespace godot;

void godot::CountryInspector::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("_on_parse_button_pressed", "data"), &CountryInspector::_on_parse_button_pressed);
}
void godot::CountryInspector::_on_parse_button_pressed(CountryData *data)
{
	UtilityFunctions::print("Button pressed!");
}
void godot::CountryInspector::_on_search_text_changed_(CountryData *data)
{
	UtilityFunctions::print("Button pressed!");
}

void godot::CountryInspector::_on_clear_search()
{
	UtilityFunctions::print("Button pressed!");
}

void godot::CountryInspector::_on_parse_button_pressed(CountryData *data)
{
	UtilityFunctions::print("Button pressed!");
}
