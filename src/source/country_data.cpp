#include "country_data.hpp"

#include "utility.hpp"

#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/print_string.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <cstdint>

using namespace godot;
using namespace godot::gsg;

void CountryData::build_look_up_tables(const Array &province_data, const Array &country_data, const Array &country_color_data)
{
	country_id_to_country_name.clear();
	country_id_to_color.clear();
	country_name_to_color.clear();
	province_id_to_owner.clear();
	province_id_to_name.clear();
	for (const Dictionary &dict : country_color_data)
	{
		country_name_to_color[dict["Name"]] = dict["Color"];
	}
	for (const Dictionary &dict : country_data)
	{
		country_id_to_country_name[dict["Id"]] = dict["Name"];
		country_id_to_color[dict["Id"]] = country_name_to_color[dict["Name"]];
	}

	for (const Dictionary &dict : province_data)
	{
		province_id_to_owner[dict["Id"]] = dict["Owner"];
		province_id_to_name[dict["Id"]] = dict["Name"];
	}
	UtilityFunctions::print("Parsed Provinces:", province_data.size());
	UtilityFunctions::print("Parsed Country Colors:", country_color_data.size());
	UtilityFunctions::print("Parsed Countries:", country_data.size());
}

PackedInt32Array CountryData::populate_color_map_buffers()
{
	PackedInt32Array data;

	for (int32_t province_id : province_id_to_name.keys())
	{
		String country_id = province_id_to_owner[province_id];
		Color color = country_id_to_color[country_id];
		data.append_array({ color.get_r8(), color.get_g8(), color.get_b8(), province_id });
	}

	return data;
}

bool CountryData::sort_by_id(const Dictionary &a, const Dictionary &b)
{
	return (int)a["Id"] < (int)b["Id"];
}
void CountryData::store_filename_data()
{
	Array province_data;
	Array country_color_data;
	Array country_data;
	PackedStringArray country_filenames = get_txt_files_in_folder(countries_folder_path);
	PackedStringArray country_color_filenames = get_txt_files_in_folder(country_colors_folder_path);
	PackedStringArray province_filenames = get_txt_files_in_folder(provinces_folder_path);

	for (const String &country_color_filename : country_color_filenames)
	{
		// filenames are like this: Aachen.txt
		Dictionary country_colors;
		country_colors["Name"] = country_color_filename.split(".")[0];
		String filename = country_color_filename.get_file();
		String full_path = country_colors_folder_path.path_join(country_color_filename);
		Color country_color = parse_country_color(full_path);
		country_colors["Color"] = country_color;
		country_color_data.push_back(country_colors);
	}

	for (const String &country_filename : country_filenames)
	{
		Dictionary country_codes;
		// This is how the file names are AAC - Aachen.txt.
		country_codes["Id"] = country_filename.substr(0, 3);			  // gets AAC identifier
		country_codes["Name"] = country_filename.substr(6).split(".")[0]; // get country name Aachen

		country_data.push_back(country_codes);
	}

	if (should_assign_country_to_non_ownable == true)
	{
		UtilityFunctions::print_verbose("Assigning provinces with no owner...");
		terrain_colors["Ocean"] = Color(0.1, 0.4, 0.7, 1.0);

		terrain_colors["No Owner"] = Color(0.7, 0.5, 0.1, 1.0);

		Array terrain_keys = terrain_colors.keys();
		for (const String &terrain_key : terrain_keys)
		{
			Dictionary terrain_dict;
			String terrain_name = terrain_key;
			terrain_dict["Name"] = terrain_name;
			terrain_dict["Color"] = terrain_colors[terrain_name];
			country_color_data.push_back(terrain_dict);
			Dictionary terrain_data;
			terrain_data["Id"] = terrain_name;
			terrain_data["Name"] = terrain_name;
			country_data.push_back(terrain_data);
		}
	}
	for (const String &province_filename : province_filenames)
	{
		// This will be 1-Uppland.txt.
		Dictionary province_codes;
		String filename = province_filename.get_file();

		PackedStringArray parts = filename.split(" - ");
		if (filename.contains(" - "))
		{
			parts = filename.split(" - ");
		}
		else if (filename.contains("-"))
		{
			parts = filename.split("-");
		}
		// Check if we got the expected number of parts.
		if (parts.size() < 2)
		{
			continue;
		}
		province_codes["Id"] = parts[0].strip_edges().to_int();
		province_codes["Name"] = parts[1].split(".txt")[0].strip_edges(); // gets "Uppland"
		String full_path = provinces_folder_path.path_join(province_filename);
		province_codes["Owner"] = parse_province_owner(full_path);
		province_data.push_back(province_codes);
	}
	// This is so, we can get the data in a logical way.
	province_data.sort_custom(callable_mp(this, &CountryData::sort_by_id));

	build_look_up_tables(province_data, country_data, country_color_data);
}
void CountryData::_ready()
{
	parse_all_files();
}
void CountryData::parse_all_files()
{
	UtilityFunctions::print("Parsing countries ...");

	if (countries_folder_path.is_empty())
	{
		print_error("Countries folder path is not set!");
		return;
	}
	if (country_colors_folder_path.is_empty())
	{
		print_error("Country colors folder path is not set!");
		return;
	}
	if (provinces_folder_path.is_empty())
	{
		print_error("Province folder path is not set!");
		return;
	}
	store_filename_data();
}

PackedStringArray CountryData::get_country_provinces(const String &country_id)
{
	PackedStringArray provinces_output;
	Array province_ids = province_id_to_owner.keys();

	for (int32_t i : province_ids)
	{
		int32_t province_id = i;

		if (province_id_to_owner[province_id] == country_id)
		{
			provinces_output.push_back("  Province: " + String(province_id_to_name[province_id]) + " (ID: " + itos(province_id) + ")");
		}
	}

	return provinces_output;
}
