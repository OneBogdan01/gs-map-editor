#include "country_data.h"

#include "utility.h"

#include "godot_cpp/classes/file_access.hpp"

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

CountryData::CountryData() = default;
CountryData::~CountryData() = default;

void CountryData::build_look_up_tables()
{
	country_id_to_country_name.clear();
	country_name_to_color.clear();
	province_id_to_owner.clear();
	for (const Dictionary &dict : country_data)
	{
		country_id_to_country_name[dict["Id"]] = dict["Name"];
	}

	for (const Dictionary &dict : country_color_data)
	{
		country_name_to_color[dict["Name"]] = dict["Color"];
	}
	for (const Dictionary &dict : province_data)
	{
		province_id_to_owner[dict["Id"]] = dict["Owner"];
	}
}

Color CountryData::get_country_color_lookup(const String &country_id)
{
	String country_name = country_id_to_country_name[country_id];

	return Color(country_name_to_color[country_name]);
}
PackedInt32Array CountryData::populate_color_map_buffers()
{
	PackedInt32Array data;

	for (const Dictionary &dict : province_data)
	{
		int32_t id = dict["Id"];
		String country_id = dict["Owner"];
		Color color = get_country_color_lookup(country_id);

		data.append_array({ color.get_r8(), color.get_g8(), color.get_b8(), id });
	}
	return data;
}
bool CountryData::sort_by_id(const Dictionary &a, const Dictionary &b)
{
	return (int)a["Id"] < (int)b["Id"];
}

void CountryData::store_filename_data()
{
	province_data.clear();
	country_color_data.clear();
	country_data.clear();
	PackedStringArray country_filenames = get_txt_files_in_folder(countries_folder_path);
	PackedStringArray country_color_filenames = get_txt_files_in_folder(country_colors_folder_path);
	PackedStringArray province_filenames = get_txt_files_in_folder(provinces_folder_path);

	for (const auto &country_color_filename : country_color_filenames)
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

	for (const auto &country_filename : country_filenames)
	{
		Dictionary country_codes;
		// this is how the file names are AAC - Aachen.txt
		country_codes["Id"] = country_filename.substr(0, 3);			  // gets AAC identifier
		country_codes["Name"] = country_filename.substr(6).split(".")[0]; // get country name Aachen

		country_data.push_back(country_codes);
	}

	if (should_assign_country_to_non_ownable == true)
	{
		UtilityFunctions::print("Assigning provinces with no owner...");
		terrain_colors["Ocean"] = Color(0.1, 0.4, 0.7);

		terrain_colors["No Owner"] = Color(0.7, 0.5, 0.1);

		Array terrain_keys = terrain_colors.keys();
		for (const auto &terrain_key : terrain_keys)
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
	for (const auto &province_filename : province_filenames)
	{
		// 1-Uppland.txt
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
		// Check if we got the expected number of parts
		if (parts.size() < 2)
		{
			continue; // Skip this file
		}
		province_codes["Id"] = parts[0].strip_edges().to_int();
		province_codes["Name"] = parts[1].split(".txt")[0].strip_edges(); // gets "Uppland"
		String full_path = provinces_folder_path.path_join(province_filename);
		province_codes["Owner"] = parse_province_owner(full_path);
		province_data.push_back(province_codes);
	}
	// sort them by id
	province_data.sort_custom(callable_mp(this, &CountryData::sort_by_id));
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

	// create lookup tables
	build_look_up_tables();
	UtilityFunctions::print("Parsed Provinces:", province_data.size());
	UtilityFunctions::print("Parsed Country Colors:", country_color_data.size());
	UtilityFunctions::print("Parsed Countries:", country_data.size());
}
PackedStringArray CountryData::get_country_provinces(uint32_t country_index)
{
	PackedStringArray provinces;
	Dictionary country_dict = country_data[country_index];
	String country_id = country_dict["Id"];

	for (const auto &j : province_data)
	{
		Dictionary province_dict = j;
		String owner_id = province_dict["Owner"];

		if (owner_id == country_id)
		{
			provinces.push_back("  Province: " + String(province_dict["Name"]) + " (ID: " + itos(province_dict["Id"]) + ")");
		}
	}
	return provinces;
}
String CountryData::get_country_from_province(uint32_t province_id)
{
	if (province_id_to_owner.has(province_id) == false)
	{
		print_error("Province id not found: ", province_id);
		return "None";
	}

	String country_id = province_id_to_owner[province_id];

	if (country_id_to_country_name.has(country_id) == false)
	{
		print_error("Country id not found: ", country_id);
		return "None";
	}

	return country_id_to_country_name[country_id];
}
Dictionary CountryData::get_country_from_name(String name)
{
	Dictionary dic;
	for (Dictionary entry_country : country_data)
	{
		if (String(entry_country["Name"]) == name)
		{
			dic = entry_country;
			return dic;
		}
	}
	print_error("Province name not found:  ", name);
	return dic;
}
Dictionary CountryData::get_province_from_id(uint32_t id)
{
	Dictionary dic;
	for (Dictionary entry : province_data)
	{
		if (uint32_t(entry["Id"]) == id)
		{
			dic = entry;
			return dic;
		}
	}
	print_error("Province id not found:  ", id);
	return dic;
}
void CountryData::change_province_owner(uint32_t province_id, const String &new_country_name)
{
	// deprecated way of doing it with the array update
	Dictionary country = get_country_from_name(new_country_name);

	Dictionary province = get_province_from_id(province_id);
	UtilityFunctions::print_verbose(new_country_name);
	province["Owner"] = country["Id"];

	province_data.set(province_data.find(province), province);
	UtilityFunctions::print_verbose("Before owner:", province_id_to_owner[province_id]);

	province_id_to_owner[province_id] = country["Id"];
	UtilityFunctions::print_verbose("After owner:", province_id_to_owner[province_id]);

	// UtilityFunctions::print(get_country_provinces(country_data.find(country)));
}
Color CountryData::get_country_color_from_province_id(uint32_t province_id)
{
	// get black
	Color color = Color(0, 0, 0, 1);

	for (const Dictionary &entry : province_data)
	{
		// find province id
		if (uint32_t(entry["Id"]) == province_id)
		{
			// get its owner it should be as an identifier country like AAC (for Aachen)
			String country_id = entry["Owner"];
			for (const Dictionary &entry_country : country_data)
			{
				// find the country with the identifier
				if (String(entry_country["Id"]) == country_id)
				{
					// using the country name get the color
					color = get_country_color(entry_country["Name"]);
					break;
				}
			}
		}
	}

	return color;
}

Color CountryData::get_country_color(const String &country_name)
{
	Color color = Color(1, 1, 1, 1);
	for (const auto &i : country_color_data)
	{
		Dictionary color_entry = i;

		if (country_name == String(color_entry["Name"]))
		{
			// Cast the Variant to Color
			return color = Color(color_entry["Color"]);
		}
	}

	print_error("Country name not found: ", country_name);

	return color;
}

bool CountryData::set_country_color_by_name(const String &country_name, const Color &new_color)
{
	int64_t index = -1;
	for (int i = 0; i < country_color_data.size(); i++)
	{
		Dictionary dict = country_color_data[i];
		if (dict.get("Name", "") == country_name)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{
		return false;
	}
	Dictionary entry = country_color_data.get(index);

	entry["Color"] = new_color;

	country_color_data.set(index, entry);

	return true;
}

void CountryData::export_color_data(int64_t color_index)
{
	// This rewrites the entire file to not corrupt it
	Dictionary color_data = country_color_data.get(color_index);
	String country_name = color_data["Name"];
	Color country_color = color_data["Color"];
	String file_path = country_colors_folder_path.path_join(country_name + ".txt");

	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
	if (!file.is_valid())
	{
		print_error("Could not open file for reading: " + file_path);
		return;
	}

	PackedByteArray raw_bytes = file->get_buffer(file->get_length());
	file->close();

	String content = raw_bytes.get_string_from_ascii();

	PackedStringArray lines = content.split("\n");

	String owner_token = "color = ";
	bool found = false;

	for (int i = 0; i < lines.size(); i++)
	{
		if (lines[i].contains(owner_token))
		{
			int32_t r = country_color.get_r8();
			int32_t g = country_color.get_g8();
			int32_t b = country_color.get_b8();
			String new_color_value = String("{ ") + String::num_int64(r) + " " + String::num_int64(g) + " " + String::num_int64(b) + " }";
			lines[i] = owner_token + new_color_value;
			found = true;
			break;
		}
	}

	if (!found)
	{
		UtilityFunctions::print("Color line not found in: " + country_name);
		return;
	}

	String new_content = "";
	for (int i = 0; i < lines.size(); i++)
	{
		new_content += lines[i];
		if (i < lines.size() - 1)
		{
			new_content += "\n";
		}
	}

	file = FileAccess::open(file_path, FileAccess::WRITE);
	if (!file.is_valid())
	{
		UtilityFunctions::print("Could not open file for writing: " + file_path);
		return;
	}

	file->store_string(new_content);

	UtilityFunctions::print("Successfully updated color for: " + country_name);
}
