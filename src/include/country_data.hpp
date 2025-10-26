#pragma once
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_color_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_dictionary.hpp"

namespace godot
{
// parses data to create gameplay entities, provinces, states countries
class CountryData : public Node
{
	GDCLASS(CountryData, Node);

public:
	// Constructor/Destructor
	CountryData() = default;
	~CountryData() override = default;

	void _ready() override;
	// Initialization
	void parse_all_files();

	// Query
	PackedStringArray get_country_provinces(const String &country_id);

	// Export generation
	void export_color_data(const String &country_name);
	void export_owner_data(int64_t province_id);
	PackedInt32Array populate_color_map_buffers();

	// Getters and Setters
	// Folders
	void set_countries_folder(const String &p_path);
	String get_countries_folder() const;
	void set_countries_color_folder(const String &p_path);
	String get_countries_color_folder() const;
	void set_provinces_folder(const String &p_path);
	String get_provinces_folder() const;

	// Dictionaries
	void set_terrain_colors(const TypedDictionary<String, Color> &data) { terrain_colors = data; }
	TypedDictionary<String, Color> get_terrain_colors() const { return terrain_colors; }
	void set_country_id_to_country_name(const TypedDictionary<String, String> &data) { country_id_to_country_name = data; }
	TypedDictionary<String, String> get_country_id_to_country_name() const { return country_id_to_country_name; }
	void set_country_name_to_color(const TypedDictionary<String, Color> &data) { country_name_to_color = data; }
	TypedDictionary<String, Color> get_country_name_to_color() const { return country_name_to_color; }
	void set_country_id_to_color(const TypedDictionary<String, Color> &data) { country_id_to_color = data; }
	TypedDictionary<String, Color> get_country_id_to_color() const { return country_id_to_color; }
	void set_province_id_to_owner(const TypedDictionary<int32_t, String> &data) { province_id_to_owner = data; }
	TypedDictionary<int32_t, String> get_province_id_to_owner() const { return province_id_to_owner; }
	void set_province_id_to_name(const TypedDictionary<int32_t, String> &data) { province_id_to_name = data; }
	TypedDictionary<int32_t, String> get_province_id_to_name() const { return province_id_to_name; }

private:
	void build_look_up_tables(const Array &province_data, const Array &country_data, const Array &country_color_data);
	void store_filename_data();
	bool sort_by_id(const Dictionary &a, const Dictionary &b);

	static void _bind_methods();
	// Parsing methods
	String parse_province_owner(const String &file_path);
	Color parse_country_color(const String &file_path);
	// folders
	String countries_folder_path;
	String country_colors_folder_path;
	String provinces_folder_path;
	// these are for fast lookups, need to be build at the start
	TypedDictionary<String, String> country_id_to_country_name;
	TypedDictionary<String, Color> country_name_to_color;
	TypedDictionary<String, Color> country_id_to_color;
	TypedDictionary<int32_t, String> province_id_to_owner;
	TypedDictionary<int32_t, String> province_id_to_name;
	TypedDictionary<String, Color> terrain_colors;
	// This is used to fix provinces that have no owner, think of oceans,
	// lakes and so on, otherwise code will only handle the case where each province is assigned to something.
	bool should_patch_terrain{ true };
	// Will handle provinces with no owner, this is so it supports data from Europa Universalis 4.
	bool should_assign_country_to_non_ownable{ true };
};
} // namespace godot