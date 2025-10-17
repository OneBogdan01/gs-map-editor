#pragma once
#include "godot_cpp/classes/camera3d.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/sprite2d.hpp"
#include "godot_cpp/classes/sprite3d.hpp"
#include "godot_cpp/classes/texture2d.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/vector2.hpp"

#include <godot_cpp/classes/input_event.hpp>

namespace godot
{
class MapData;
class CountryData;

class ProvinceSelector : public Node3D
{
	GDCLASS(ProvinceSelector, Node3D);

public:
	ProvinceSelector();
	~ProvinceSelector();
	// setters getters
	MapData *get_province_parser() const;
	void set_province_parser(MapData *value);
	CountryData *get_country_data() const;
	void set_country_data(CountryData *value);
	Sprite3D *get_province_map() const;
	void set_province_map(Sprite3D *value);
	Camera3D *get_current_camera() const;
	void _input(const Ref<InputEvent> &event) override;

	void select_province();

protected:
	static void _bind_methods();

private:
	// might be batter to use node paths instead
	MapData *province_parser{ nullptr };
	CountryData *country_data{ nullptr };
	Sprite3D *province_map{ nullptr };
	Ref<Texture2D> political_map;
	Vector2 last_mouse_position{};
	bool clicked{ false };
	const float RAY_LENGTH{ 1000.0f };
};
} // namespace godot