#include "country_data.h"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"

using namespace godot;

void CountryData::set_country_data(const Array &data)
{
	country_data = data;
}
Array CountryData::get_country_data() const
{
	return country_data;
}

void CountryData::set_province_data(const Array &data)
{
	province_data = data;
}
Array CountryData::get_province_data() const
{
	return province_data;
}
void CountryData::set_country_color_data(const Array &data)
{
	country_color_data = data;
}
Array CountryData::get_country_color_data() const
{
	return country_color_data;
}

void CountryData::set_provinces_folder(const String &path)
{
	provinces_folder_path = path;
}
String CountryData::get_provinces_folder() const
{
	return provinces_folder_path;
}

void CountryData::set_countries_folder(const String &p_path)
{
	countries_folder_path = p_path;
}

String CountryData::get_countries_folder() const
{
	return countries_folder_path;
}
void CountryData::set_countries_color_folder(const String &p_path)
{
	country_colors_folder_path = p_path;
}

String CountryData::get_countries_color_folder() const
{
	return country_colors_folder_path;
}
void CountryData::_bind_methods()
{
	// folders
	ClassDB::bind_method(D_METHOD("set_provinces_folder", "path"), &CountryData::set_provinces_folder);
	ClassDB::bind_method(D_METHOD("get_provinces_folder"), &CountryData::get_provinces_folder);
	ClassDB::bind_method(D_METHOD("set_countries_folder", "path"), &CountryData::set_countries_folder);
	ClassDB::bind_method(D_METHOD("get_countries_folder"), &CountryData::get_countries_folder);
	ClassDB::bind_method(D_METHOD("set_countries_color_folder", "path"), &CountryData::set_countries_color_folder);
	ClassDB::bind_method(D_METHOD("get_countries_color_folder"), &CountryData::get_countries_color_folder);

	// data
	ClassDB::bind_method(D_METHOD("get_province_data"), &CountryData::get_province_data);
	ClassDB::bind_method(D_METHOD("set_province_data"), &CountryData::set_province_data);
	ClassDB::bind_method(D_METHOD("get_country_data"), &CountryData::get_country_data);
	ClassDB::bind_method(D_METHOD("set_country_data", "data"), &CountryData::set_country_data);
	ClassDB::bind_method(D_METHOD("get_country_color_data"), &CountryData::get_country_color_data);
	ClassDB::bind_method(D_METHOD("set_country_color_data", "data"), &CountryData::set_country_color_data);

	// dictionaries
	ClassDB::bind_method(D_METHOD("get_country_id_to_country_name"), &CountryData::get_country_id_to_country_name);
	ClassDB::bind_method(D_METHOD("set_country_id_to_country_name", "data"), &CountryData::set_country_id_to_country_name);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "country_id_to_country_name"), "set_country_id_to_country_name", "get_country_id_to_country_name");

	ClassDB::bind_method(D_METHOD("get_country_name_to_color"), &CountryData::get_country_name_to_color);
	ClassDB::bind_method(D_METHOD("set_country_name_to_color", "data"), &CountryData::set_country_name_to_color);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "country_name_to_color"), "set_country_name_to_color", "get_country_name_to_color");

	ClassDB::bind_method(D_METHOD("get_country_id_to_color"), &CountryData::get_country_id_to_color);
	ClassDB::bind_method(D_METHOD("set_country_id_to_color", "data"), &CountryData::set_country_id_to_color);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "country_id_to_color"), "set_country_id_to_color", "get_country_id_to_color");

	ClassDB::bind_method(D_METHOD("get_province_id_to_owner"), &CountryData::get_province_id_to_owner);
	ClassDB::bind_method(D_METHOD("set_province_id_to_owner", "data"), &CountryData::set_province_id_to_owner);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "province_id_to_owner"), "set_province_id_to_owner", "get_province_id_to_owner");

	ClassDB::bind_method(D_METHOD("get_province_id_to_name"), &CountryData::get_province_id_to_name);
	ClassDB::bind_method(D_METHOD("set_province_id_to_name", "data"), &CountryData::set_province_id_to_name);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "province_id_to_name"), "set_province_id_to_name", "get_province_id_to_name");

	// others
	ClassDB::bind_method(D_METHOD("get_terrain_colors"), &CountryData::get_terrain_colors);
	ClassDB::bind_method(D_METHOD("set_terrain_colors", "data"), &CountryData::set_terrain_colors);

	ClassDB::bind_method(D_METHOD("parse_all_files"), &CountryData::parse_all_files);
	ClassDB::bind_method(D_METHOD("get_country_provinces"), &CountryData::get_country_provinces);

	ClassDB::bind_method(D_METHOD("get_country_color", "name"), &CountryData::get_country_color);
	ClassDB::bind_method(D_METHOD("change_province_owner"), &CountryData::change_province_owner);
	ClassDB::bind_method(D_METHOD("get_country_color_from_province_id", "name"), &CountryData::get_country_color_from_province_id);
	ClassDB::bind_method(D_METHOD("populate_color_map_buffers"), &CountryData::populate_color_map_buffers);

	ClassDB::bind_method(D_METHOD("set_country_color_by_name", "name", "color"), &CountryData::set_country_color_by_name);

	ClassDB::bind_method(D_METHOD("export_color_data"), &CountryData::export_color_data);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "provinces_folder", PROPERTY_HINT_DIR), "set_provinces_folder", "get_provinces_folder");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "countries_folder", PROPERTY_HINT_DIR), "set_countries_folder", "get_countries_folder");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "countries_color_folder", PROPERTY_HINT_DIR), "set_countries_color_folder", "get_countries_color_folder");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "terrain_colors", PROPERTY_HINT_DICTIONARY_TYPE, "String:Color"), "set_terrain_colors", "get_terrain_colors");

	// ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "map_data"), "set_countries_folder", "get_countries_folder");
	// ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "map_data"), "set_countries_folder", "get_countries_folder");
}