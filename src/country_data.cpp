#include "country_data.h"

#include "terrain_patch.h"

#include "godot_cpp/classes/dir_access.hpp"
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

using namespace godot;

CountryData::CountryData() = default;
CountryData::~CountryData() {}

PackedStringArray CountryData::get_txt_files_in_folder(const String &folder_path)
{
	PackedStringArray txt_files;

	Ref<DirAccess> dir = DirAccess::open(folder_path);
	if (dir.is_null())
	{
		UtilityFunctions::print("Failed to open directory: ", folder_path);
		return txt_files;
	}

	dir->list_dir_begin();
	String file_name = dir->get_next();

	while (file_name.is_empty() == false)
	{
		if (dir->current_is_dir() == false && file_name.get_extension().to_lower() == "txt")
		{
			txt_files.push_back(file_name);
		}
		file_name = dir->get_next();
	}

	dir->list_dir_end();
	return txt_files;
}
//call at the beginning
void CountryData::build_look_up_tables()
{
	country_id_to_country_name.clear();
	country_name_to_color.clear();

	for (const Dictionary &dict : country_data)
	{
		country_id_to_country_name[dict["Id"]] = dict["Name"];
	}

	for (const Dictionary &dict : country_color_data)
	{
		country_name_to_color[dict["Name"]] = dict["Color"];
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

	int i = 0;
	for (const Dictionary &dict : province_data)
	{
		int id = dict["Id"];
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
void CountryData::parse_all_files()
{
	UtilityFunctions::print("Parsing countries ...");

	if (countries_folder_path.is_empty())
	{
		UtilityFunctions::print("Countries folder path is not set!");
		return;
	}
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
		country_codes["Id"] = country_filename.substr(0, 3); // gets AAC identifier
		country_codes["Name"] = country_filename.substr(6).split(".")[0]; // get country name Aachen

		country_data.push_back(country_codes);
	}
	// Add terrain colors here:

	terrain_tokens["Ocean"] = Color(0.1, 0.4, 0.7);

	terrain_tokens["No Owner"] = Color(0.7, 0.5, 0.1);

	Array terrain_keys = terrain_tokens.keys();
	for (const auto &terrain_key : terrain_keys)
	{
		Dictionary terrain_dict;
		String terrain_name = terrain_key;
		terrain_dict["Name"] = terrain_name;
		terrain_dict["Color"] = terrain_tokens[terrain_name];
		country_color_data.push_back(terrain_dict);
		Dictionary terrain_data;
		terrain_data["Id"] = terrain_name;
		terrain_data["Name"] = terrain_name;
		country_data.push_back(terrain_data);
	}
	for (const auto &province_filename : province_filenames)
	{
		//1-Uppland.txt
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
String CountryData::get_country_from_province(int32_t province_id)
{
	String country_owner = "None";
	for (const Dictionary &dict : province_data)
	{
		if (int32_t(dict["Id"]) == province_id)
		{
			String country_id = String(dict["Owner"]);
			for (const Dictionary &entry_country : country_data)
			{
				if (String(entry_country["Id"]) == country_id)
				{
					country_owner = (entry_country["Name"]);
					return country_owner;
				}
			}
		}
	}
	print_error("Province id not found:  ", province_id);
	return country_owner;
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
Dictionary CountryData::get_province_from_id(int32_t id)
{
	Dictionary dic;
	for (Dictionary entry : province_data)
	{
		if (int32_t(entry["Id"]) == id)
		{
			dic = entry;
			return dic;
		}
	}
	print_error("Province id not found:  ", id);
	return dic;
}
void CountryData::change_province_owner(int32_t province_id, const String &new_country_name)
{
	Dictionary country = get_country_from_name(new_country_name);

	Dictionary province = get_province_from_id(province_id);
	province["Owner"] = country["Id"];
	province_data.set(province_data.find(province), province);
	// UtilityFunctions::print(get_country_provinces(country_data.find(country)));
}
Color CountryData::get_country_color_from_province_id(uint32_t province_id)
{
	// get black
	Color color = Color(0, 0, 0, 1);

	for (const Dictionary &entry : province_data)
	{
		//find province id
		if (uint32_t(entry["Id"]) == province_id)
		{
			//get its owner it should be as an identifier country like AAC (for Aachen)
			String country_id = entry["Owner"];
			for (const Dictionary &entry_country : country_data)
			{
				//find the country with the identifier
				if (String(entry_country["Id"]) == country_id)
				{
					//using the country name get the color
					color = get_country_color(entry_country["Name"]);
					break;
				}
			}
		}
	}

	return color;
}

String CountryData::parse_province_owner(const String &file_path)
{
	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
	String content = file->get_buffer(file->get_length()).get_string_from_ascii();

	String owner_token = "owner =";
	int owner_pos = content.find(owner_token);
	if (owner_pos == -1 || content.find("tribal_owner") != -1)
	{
		// handle the mountain case
		String filename = file_path.get_file();
		if (should_patch_terrain)
		{
			String assigned_owner = gsg::get_terrain_owner(filename);
			if (assigned_owner.is_empty() == false)
			{
				return assigned_owner;
			}

			UtilityFunctions::print(filename + " is unassigned");
			return "No Owner";
		}
	}
	int value_start = owner_pos + owner_token.length();
	int line_end = content.find("\n", value_start);
	//get ID
	String province_owner = content.substr(value_start, line_end - value_start).strip_edges();
	//strip comments
	int comment_pos = province_owner.find("#");
	if (comment_pos == -1)
	{
		comment_pos = province_owner.find("//");
	}
	if (comment_pos != -1)
	{
		province_owner = province_owner.substr(0, comment_pos).strip_edges();
	}
	return province_owner;
}
Color CountryData::parse_country_color(const String &file_path)
{
	Color color;
	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
	String content = file->get_buffer(file->get_length()).get_string_from_ascii();

	String owner_token = "color = ";
	int owner_pos = content.find(owner_token);
	int value_start = owner_pos + owner_token.length();
	int line_end = content.find("\n", value_start);
	//get ID
	String province_owner = content.substr(value_start, line_end - value_start).strip_edges();
	PackedStringArray fields = province_owner.split(" ");

	// only add numbers
	Array rgb;

	for (auto field : fields)
	{
		//try to remove non numbers
		field = field.replace("}", "");
		field = field.replace("{", "");
		if (field.is_valid_float())
		{
			rgb.push_back(field.to_int());
		}
	}
	if (rgb.size() != 3)
	{
		UtilityFunctions::print("Could not parse", rgb);
		UtilityFunctions::print("Found Strings", fields);
		UtilityFunctions::print("Could not parse file:", file_path);
		return color;
	}
	color = Color::from_rgba8(rgb[0], rgb[1], rgb[2]);

	return color;
}
Array CountryData::get_country_data() const
{
	return country_data;
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

int64_t CountryData::get_country_id_from_name(String name)
{
	for (int64_t i = 0; i < country_color_data.size(); i++)
	{
		Dictionary dict = country_color_data[i];
		if (dict["Name"] == name)
		{
			return i;
		}
	}

	print_error("Did not find country color for: ", name);
	return -1;
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
	//This rewrites the entire file to not corrupt it
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

void CountryData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_provinces_folder", "path"), &CountryData::set_provinces_folder);
	ClassDB::bind_method(D_METHOD("get_provinces_folder"), &CountryData::get_provinces_folder);
	ClassDB::bind_method(D_METHOD("set_countries_folder", "path"), &CountryData::set_countries_folder);
	ClassDB::bind_method(D_METHOD("get_countries_folder"), &CountryData::get_countries_folder);
	ClassDB::bind_method(D_METHOD("set_countries_color_folder", "path"), &CountryData::set_countries_color_folder);
	ClassDB::bind_method(D_METHOD("get_countries_color_folder"), &CountryData::get_countries_color_folder);
	ClassDB::bind_method(D_METHOD("export_color_data"), &CountryData::export_color_data);
	ClassDB::bind_method(D_METHOD("get_country_id_from_name"), &CountryData::get_country_id_from_name);

	ClassDB::bind_method(D_METHOD("get_province_data"), &CountryData::get_province_data);
	ClassDB::bind_method(D_METHOD("set_province_data"), &CountryData::set_province_data);
	ClassDB::bind_method(D_METHOD("parse_all_files"), &CountryData::parse_all_files);
	ClassDB::bind_method(D_METHOD("get_country_provinces"), &CountryData::get_country_provinces);
	ClassDB::bind_method(D_METHOD("get_country_data"), &CountryData::get_country_data);
	ClassDB::bind_method(D_METHOD("get_country_color_data"), &CountryData::get_country_color_data);
	ClassDB::bind_method(D_METHOD("get_country_color", "name"), &CountryData::get_country_color);
	ClassDB::bind_method(D_METHOD("change_province_owner"), &CountryData::change_province_owner);
	ClassDB::bind_method(D_METHOD("get_country_color_from_province_id", "name"), &CountryData::get_country_color_from_province_id);
	ClassDB::bind_method(D_METHOD("populate_color_map_buffers"), &CountryData::populate_color_map_buffers);

	ClassDB::bind_method(D_METHOD("set_country_color_by_name", "name", "color"), &CountryData::set_country_color_by_name);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "provinces_folder", PROPERTY_HINT_DIR), "set_provinces_folder", "get_provinces_folder");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "countries_folder", PROPERTY_HINT_DIR), "set_countries_folder", "get_countries_folder");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "countries_color_folder", PROPERTY_HINT_DIR), "set_countries_color_folder", "get_countries_color_folder");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "map_data"), "set_countries_folder", "get_countries_folder");
}