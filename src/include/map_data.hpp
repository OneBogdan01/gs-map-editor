#pragma once
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_dictionary.hpp"

namespace godot
{

class MapData : public Node
{
	GDCLASS(MapData, Node)

public:
	void build_color_lookup(const Array &province_data);
	void load_csv_data();
	// setter and getters
	TypedDictionary<Color, String> get_province_color_to_name() const;
	void set_province_color_to_name(const TypedDictionary<Color, String> &data);
	TypedDictionary<Color, int32_t> get_province_color_to_id() const;
	void set_province_color_to_id(const TypedDictionary<Color, int32_t> &data);

	void set_csv_path(const String &p_path);
	String get_csv_path() const;
	bool get_should_skip_first_row() const;
	void set_should_skip_first_row(bool value);

	void _ready() override;

protected:
	static void _bind_methods();

private:
	TypedDictionary<Color, String> province_color_to_name;
	TypedDictionary<Color, int32_t> province_color_to_id;

	String csv_file_path{ "res://assets/definition.csv" };

	bool should_skip_first_row{ true };
};
} // namespace godot
