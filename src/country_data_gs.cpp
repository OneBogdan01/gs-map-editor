#include "country_data.h"

using namespace godot;
Array CountryData::get_province_data() const
{
	return province_data;
}
Array CountryData::get_country_color_data() const
{
	return country_color_data;
}
void CountryData::set_province_data(const Array &data)
{
	province_data = data;
}
void CountryData::set_provinces_folder(const String &p_path)
{
	provinces_folder_path = p_path;
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