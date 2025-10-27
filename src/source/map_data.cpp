#include "map_data.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/core/print_string.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <cstdint>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

namespace
{
// godot provided color comparison is too strict
bool colors_equal(const Color &a, const Color &b, float tolerance = 0.001f)
{
	return (abs(a.r - b.r) < tolerance &&
			abs(a.g - b.g) < tolerance &&
			abs(a.b - b.b) < tolerance &&
			abs(a.a - b.a) < tolerance);
}
} // namespace

void MapData::_ready()
{
	load_csv_data();
}
void MapData::_bind_methods()
{
	// csv
	ClassDB::bind_method(D_METHOD("get_csv_path"), &MapData::get_csv_path);
	ClassDB::bind_method(D_METHOD("set_csv_path", "p_path"), &MapData::set_csv_path);
	ClassDB::bind_method(D_METHOD("load_csv_data"), &MapData::load_csv_data);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "csv_file_path",
						 PROPERTY_HINT_FILE, "*.csv"),
			"set_csv_path", "get_csv_path");

	// Circumstantial booleans
	ClassDB::bind_method(D_METHOD("get_should_skip_first_row"), &MapData::get_should_skip_first_row);
	ClassDB::bind_method(D_METHOD("set_should_skip_first_row", "data"), &MapData::set_should_skip_first_row);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "should_skip_first_row"),
			"set_should_skip_first_row", "get_should_skip_first_row");

	// Dictionaries
	ClassDB::bind_method(D_METHOD("get_province_color_to_name"), &MapData::get_province_color_to_name);
	ClassDB::bind_method(D_METHOD("set_province_color_to_name", "data"), &MapData::set_province_color_to_name);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "province_color_to_name"),
			"set_province_color_to_name", "get_province_color_to_name");

	ClassDB::bind_method(D_METHOD("get_province_color_to_id"), &MapData::get_province_color_to_id);
	ClassDB::bind_method(D_METHOD("set_province_color_to_id", "data"), &MapData::set_province_color_to_id);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "province_color_to_id"),
			"set_province_color_to_id", "get_province_color_to_id");
}

void MapData::set_csv_path(const String &p_path)
{
	csv_file_path = p_path;
}

String MapData::get_csv_path() const
{
	return csv_file_path;
}

TypedDictionary<Color, String> MapData::get_province_color_to_name() const
{
	return province_color_to_name;
}

void MapData::set_province_color_to_name(const TypedDictionary<Color, String> &data)
{
	province_color_to_name = data;
}

TypedDictionary<Color, int32_t> MapData::get_province_color_to_id() const
{
	return province_color_to_id;
}

void MapData::set_province_color_to_id(const TypedDictionary<Color, int32_t> &data)
{
	province_color_to_id = data;
}
bool MapData::get_should_skip_first_row() const
{
	return should_skip_first_row;
}
void MapData::set_should_skip_first_row(bool value)
{
	should_skip_first_row = value;
}
void MapData::build_color_lookup(const Array &province_data)
{
	province_color_to_name.clear();
	province_color_to_id.clear();

	for (const Dictionary &dict : province_data)
	{
		Color province_color = dict["Color"];

		province_color_to_name[province_color] = dict["Name"];
		province_color_to_id[province_color] = dict["Id"];
	}

	UtilityFunctions::print_verbose("Built color lookup with ", province_color_to_name.size());
}
void MapData::load_csv_data()
{
	Array province_data;
	if (csv_file_path.is_empty())
	{
		print_error("CSV file path is empty", csv_file_path);
		return;
	}
	Ref<FileAccess> file = FileAccess::open(csv_file_path, FileAccess::READ);

	if (file.is_null())
	{
		UtilityFunctions::print("Error: Cannot open file");
		return;
	}

	// Decodes correctly rather than failing for UTF8.
	String buffer = file->get_buffer(file->get_length()).get_string_from_ascii();

	PackedStringArray rows = buffer.split("\n");

	for (const String &row : rows)
	{
		// The first line in my EU4 needs to be skipped
		if (should_skip_first_row && row == rows[0])
		{
			continue;
		}

		PackedStringArray fields = row.split(";");

		if (fields.size() >= 5)
		{
			Dictionary province_dict;
			province_dict["Id"] = fields[0].to_int();
			province_dict["Color"] = Color::from_rgba8(fields[1].to_int(), fields[2].to_int(), fields[3].to_int());
			province_dict["Name"] = fields[4];
			province_dict["Flag"] = fields[5];
			province_data.push_back(province_dict);
		}
	}
	UtilityFunctions::print_verbose("Loaded ", province_data.size(), " provinces");
	build_color_lookup(province_data);
}