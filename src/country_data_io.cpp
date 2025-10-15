#include "country_data.h"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "utility.h"

using namespace godot;
String CountryData::parse_province_owner(const String &file_path)
{
	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
	String content = file->get_buffer(file->get_length()).get_string_from_ascii();

	String owner_token = "owner =";
	int owner_pos = content.find(owner_token);
	bool on_start_of_the_line = content.substr(owner_pos - 1, 1).contains("\n");
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
