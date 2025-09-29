#pragma once

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/string.hpp"
#include <cstdint>
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

	//setter and getters
	void set_csv_path(const String &p_path);
	String get_csv_path() const;
	void load_csv_data();
	Array get_province_data() const;
	void set_province_data(const Array &p_array);

protected:
	static void _bind_methods();

private:
	String csv_file_path = "res://assets/definition.csv";
	Array province_data;
};

} //namespace godot
