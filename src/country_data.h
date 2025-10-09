#pragma once
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_color_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include <cstdint>
namespace godot
{
//parses data to create gameplay entities, provinces, states countries
class CountryData : public Node
{
	GDCLASS(CountryData, Node);

public:
	CountryData();
	~CountryData();
	bool sort_by_id(const Dictionary &a, const Dictionary &b);
	void set_countries_folder(const String &p_path);
	String get_countries_folder() const;
	void set_countries_color_folder(const String &p_path);
	String get_countries_color_folder() const;
	void set_provinces_folder(const String &p_path);
	String get_provinces_folder() const;
	Array get_province_data() const;
	Array get_country_color_data() const;
	void set_province_data(const Array &data);
	String parse_province_owner(const String &file_path);
	PackedStringArray get_country_provinces(uint32_t country_id);
	Color get_country_color(const String &country_name);
	Array get_country_data() const;
	Color parse_country_color(const String &file_path);
	bool set_country_color_by_name(const String &country_name, const Color &new_color);
	Color get_country_color_from_province_id(uint32_t province_id);
	String get_country_from_province(int32_t province_id);
	void change_province_owner(int32_t province_id, const String &new_country_name);
	Dictionary get_country_from_name(String name);
	Dictionary get_province_from_id(int32_t id);
	void parse_all_files();
	void export_color_data(int64_t color_index);
	int64_t get_country_id_from_name(String name);
	PackedInt32Array populate_color_map_buffers();
	void build_look_up_tables();
	Color get_country_color_lookup(const String &country_id);

private:
	PackedStringArray get_txt_files_in_folder(const String &folder_path);

	static void _bind_methods();
	//dictionaries for each of the folders below
	Array province_data;
	Array country_data;
	Array country_color_data;
	//folders
	String countries_folder_path;
	String country_colors_folder_path;
	String provinces_folder_path;
	//these are for fast lookups, need to be build at the start
	Dictionary country_id_to_country_name;
	Dictionary country_name_to_color;
	Dictionary terrain_tokens;
	//used to path provinces that have no owner, think of oceans,
	//lakes and so on, this plugin only handles the case where each province is assigned to something.
	bool should_patch_terrain{ true };
};
} //namespace godot