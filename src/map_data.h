#pragma once

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/string.hpp"
#include <godot_cpp/classes/sprite2d.hpp>

namespace godot
{

class MapData : public Node
{
	GDCLASS(MapData, Node)

public:
	MapData();
	~MapData();

	String get_province_from_color(Color color);
	int32_t get_province_id_from_color(Color color);

	void load_csv_data();
	//setter and getters
	Array get_province_data() const;
	void set_province_data(const Array &p_data);
	void set_csv_path(const String &p_path);
	String get_csv_path() const;
	bool get_should_skip_first_row() const;
	void set_should_skip_first_row(bool value);

protected:
	static void _bind_methods();

private:
	String csv_file_path{ "res://assets/definition.csv" };
	Array province_data;
	bool should_skip_first_row{ true };
};
} //namespace godot
