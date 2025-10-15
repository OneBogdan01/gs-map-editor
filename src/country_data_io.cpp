#include "country_data.h"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/core/print_string.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "utility.h"
#include <cstdint>

using namespace godot;
String CountryData::parse_province_owner(const String &file_path)
{
	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
	String content = file->get_buffer(file->get_length()).get_string_from_ascii();

	String owner_token = "owner =";
	int owner_pos = content.find(owner_token);
	// check if the token is at the beginning of the file or line
	bool on_start_of_the_line = (owner_pos - 1) < 0 || content.substr(owner_pos - 1, 1).contains("\n");
	// patch the EU4 data, the owner needs to be at the beginning of the line.

	if (on_start_of_the_line == false || owner_pos == -1 || content.find("tribal_owner") != -1)
	{
		// handle the terrain case
		String filename = file_path.get_file();
		if (should_patch_terrain)
		{
			String assigned_owner = gsg::get_terrain_owner(filename);
			if (assigned_owner.is_empty() == false)
			{
				return assigned_owner;
			}
		}
		UtilityFunctions::print(filename + " is unassigned");
		return "No Owner";
	}

	int value_start = owner_pos + owner_token.length();
	int line_end = content.find("\n", value_start);
	// get ID
	String province_owner = content.substr(value_start, line_end - value_start).strip_edges();
	// strip comments
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
	// get ID
	String province_owner = content.substr(value_start, line_end - value_start).strip_edges();
	PackedStringArray fields = province_owner.split(" ");

	// only add numbers
	Array rgb;

	for (auto field : fields)
	{
		// try to remove non numbers
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

void CountryData::export_color_data(int64_t color_index)
{
	Dictionary color_data = country_color_data.get(color_index);
	String country_name = color_data["Name"];
	Color country_color = color_data["Color"];

	String file_path = country_colors_folder_path.path_join(country_name + ".txt");

	PackedByteArray raw_bytes = gsg::read_file_bytes(file_path);
	if (raw_bytes.size() == 0)
	{
		print_error("Failed to read file for: " + country_name);
		return;
	}
	PackedByteArray modified_file = gsg::find_replace_in_file(raw_bytes, "color =", gsg::color_to_string(country_color));
	if (modified_file.size() == 0)
	{
		print_error("Failed to find 'color = ' in file for: " + country_name);
		return;
	}
	// Write the new file
	if (gsg::write_file_bytes(file_path, modified_file) == false)
	{
		print_error("Failed to write color for: " + country_name);
		return;
	}

	UtilityFunctions::print("Successfully updated color for: " + country_name);
}
void CountryData::export_owner_data(int64_t province_id)
{
	String province_name = province_id_to_name[province_id];
	String file_path = provinces_folder_path.path_join(itos(province_id) + " - " + province_name + ".txt");

	PackedByteArray raw_bytes = gsg::read_file_bytes(file_path);
	if (raw_bytes.size() == 0)
	{
		print_error("Failed to read file for: " + province_name);
		return;
	}
	String new_owner = province_id_to_owner[province_id];

	PackedByteArray modified_file = gsg::find_replace_in_file(raw_bytes, "owner =", " " + new_owner);
	if (modified_file.size() == 0)
	{
		print_error("Failed to find 'owner = ' in file for: " + province_name);
		return;
	}
	// Write the new file
	if (gsg::write_file_bytes(file_path, modified_file) == false)
	{
		print_error("Failed to write owner for: " + province_name);
		return;
	}

	UtilityFunctions::print("Successfully updated owner for: " + province_name);
}