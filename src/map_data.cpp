#include "map_data.h"
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
} //namespace

void MapData::_bind_methods()
{
	//csv
	ClassDB::bind_method(D_METHOD("get_csv_path"), &MapData::get_csv_path);
	ClassDB::bind_method(D_METHOD("set_csv_path", "p_path"), &MapData::set_csv_path);
	ClassDB::bind_method(D_METHOD("load_csv_data"), &MapData::load_csv_data);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "csv_file_path",
						 PROPERTY_HINT_FILE, "*.csv"),
			"set_csv_path", "get_csv_path");

	ClassDB::bind_method(D_METHOD("get_should_skip_first_row"), &MapData::get_should_skip_first_row);
	ClassDB::bind_method(D_METHOD("set_should_skip_first_row", "data"), &MapData::set_should_skip_first_row);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "should_skip_first_row"),
			"set_should_skip_first_row", "get_should_skip_first_row");

	//province data
	ClassDB::bind_method(D_METHOD("get_province_data"), &MapData::get_province_data);
	ClassDB::bind_method(D_METHOD("set_province_data", "data"), &MapData::set_province_data);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "province_data"),
			"set_province_data", "get_province_data");
}

MapData::MapData() = default;
MapData::~MapData()
{
}

void MapData::set_csv_path(const String &p_path)
{
	csv_file_path = p_path;
}

String MapData::get_province_from_color(Color color)
{
	for (int i = 0; i < province_data.size(); i++)
	{
		const Dictionary &dict = province_data[i];
		Color province_color = dict["Color"];

		// default epsilon value is too big
		if (colors_equal(province_color, color))
		{
			return dict["Name"];
		}
	}
	return "nope";
}
int32_t MapData::get_province_id_from_color(Color color)
{
	for (int i = 0; i < province_data.size(); i++)
	{
		const Dictionary &dict = province_data[i];
		Color province_color = dict["Color"];

		// default epsilon value is too big
		if (colors_equal(province_color, color))
		{
			return dict["Id"];
		}
	}
	print_error("Did not find province with color: ", color);
	return -1;
}
String MapData::get_csv_path() const
{
	return csv_file_path;
}
void MapData::set_province_data(const Array &data)
{
	province_data = data;
}
Array MapData::get_province_data() const
{
	return province_data;
}

bool MapData::get_should_skip_first_row() const
{
	return should_skip_first_row;
}
void MapData::set_should_skip_first_row(bool value)
{
	should_skip_first_row = value;
}

void MapData::load_csv_data()
{
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

	//Decodes correctly rather than failing for UTF8
	String buffer = file->get_buffer(file->get_length()).get_string_from_ascii();

	//Parse data
	PackedStringArray rows = buffer.split("\n");

	for (const String &row : rows)
	{
		// The first line in my data needs to be skipped
		if (should_skip_first_row && row == rows[0])
		{
			continue;
		}

		// UtilityFunctions::print(row.strip_edges());
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
	UtilityFunctions::print("Loaded ", province_data.size(), " provinces");
}