#include "province_selector.h"
#include "country_data.h"
#include "godot_cpp/classes/camera3d.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/image.hpp"
#include "godot_cpp/classes/input.hpp"
#include "godot_cpp/classes/input_event_mouse_button.hpp"
#include "godot_cpp/classes/physics_direct_space_state3d.hpp"
#include "godot_cpp/classes/physics_ray_query_parameters3d.hpp"
#include "godot_cpp/classes/viewport.hpp"
#include "godot_cpp/classes/world3d.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/vector2i.hpp"
#include "map_data.h"
#include <cstdint>

using namespace godot;

ProvinceSelector::ProvinceSelector() = default;
ProvinceSelector::~ProvinceSelector() {}

MapData *ProvinceSelector::get_province_parser() const
{
	return province_parser;
}
void ProvinceSelector::set_province_parser(MapData *value)
{
	province_parser = value;
}

CountryData *ProvinceSelector::get_country_data() const
{
	return country_data;
}
void ProvinceSelector::set_country_data(CountryData *value)
{
	country_data = value;
}
Sprite3D *ProvinceSelector::get_province_map() const
{
	return province_map;
}
void ProvinceSelector::set_province_map(Sprite3D *value)
{
	province_map = value;
}
void ProvinceSelector::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_province_parser"), &ProvinceSelector::get_province_parser);
	ClassDB::bind_method(D_METHOD("set_province_parser", "value"), &ProvinceSelector::set_province_parser);

	ClassDB::bind_method(D_METHOD("get_country_data"), &ProvinceSelector::get_country_data);
	ClassDB::bind_method(D_METHOD("set_country_data", "value"), &ProvinceSelector::set_country_data);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "province_parser", PROPERTY_HINT_NODE_TYPE, "MapData"),
			"set_province_parser", "get_province_parser");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "country_data", PROPERTY_HINT_NODE_TYPE, "CountryData"),
			"set_country_data", "get_country_data");
	ClassDB::bind_method(D_METHOD("get_province_map"), &ProvinceSelector::get_province_map);
	ClassDB::bind_method(D_METHOD("select_province"), &ProvinceSelector::select_province);
	ClassDB::bind_method(D_METHOD("set_province_map", "value"), &ProvinceSelector::set_province_map);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "ProvinceMap", PROPERTY_HINT_NODE_TYPE, "Sprite3D"),
			"set_province_map", "get_province_map");

	ADD_SIGNAL(MethodInfo("province_selected", PropertyInfo(Variant::INT, "province_id")));
	ADD_SIGNAL(MethodInfo("country_selected", PropertyInfo(Variant::STRING, "country")));
}

Camera3D *ProvinceSelector::get_current_camera() const
{
	return get_viewport()->get_camera_3d();
}
// TODO make faster
void ProvinceSelector::select_province()
{
	PhysicsDirectSpaceState3D *space = get_world_3d()->get_direct_space_state();
	Camera3D *camera = get_current_camera();
	if (camera == nullptr)
	{
		return;
	}

	Vector3 from = camera->project_ray_origin(last_mouse_position);
	Vector3 to = from + camera->project_ray_normal(last_mouse_position) * RAY_LENGTH;
	Ref<PhysicsRayQueryParameters3D> query = PhysicsRayQueryParameters3D::create(from, to);
	Dictionary result = space->intersect_ray(query);
	if (result.is_empty() == false)
	{
		Vector3 world_position = result["position"];
		Ref<Texture2D> texture = province_map->get_texture();
		Vector2 texture_size = texture->get_size();
		float pixel_size = province_map->get_pixel_size();
		// Convert to relative coordinate space
		float half_width = texture_size.x * pixel_size * 0.5f;
		float half_height = texture_size.y * pixel_size * 0.5f;

		int32_t texture_x = static_cast<int32_t>((world_position.x + half_width) / pixel_size);
		int32_t texture_y = static_cast<int32_t>((world_position.z + half_height) / pixel_size);

		texture_x = Math::clamp(texture_x, 0, (int32_t)texture_size.x - 1);
		texture_y = Math::clamp(texture_y, 0, (int32_t)texture_size.y - 1);

		Color province_color = texture->get_image()->get_pixel(texture_x, texture_y);
		UtilityFunctions::print("Texture coordinates: ", texture_x, ", ", texture_y);
		UtilityFunctions::print("World position: ", world_position);
		UtilityFunctions::print("Province Color Value: ", province_color);
		String province_name = province_parser->get_province_from_color(province_color);
		UtilityFunctions::print("Province Name: ", province_name);
		int32_t province_id = province_parser->get_province_id_from_color(province_color);
		UtilityFunctions::print("Province ID: ", province_id);

		UtilityFunctions::print("Country: ", country_data->get_country_from_province(province_id));
		emit_signal("province_selected", province_id);
		emit_signal("country_selected", country_data->get_country_from_province(province_id));
	}
}

void ProvinceSelector::_input(const Ref<InputEvent> &event)
{
	Ref<InputEventMouseButton> mouse_event = event;
	if (mouse_event.is_valid() && mouse_event->is_pressed())
	{
		last_mouse_position = mouse_event->get_position();
	}
}