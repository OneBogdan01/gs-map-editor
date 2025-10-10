#include "camera_controller.h"
#include "godot_cpp/classes/input.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/variant/vector3.hpp"

using namespace godot;

CameraController::CameraController() = default;
CameraController::~CameraController() = default;

float CameraController::get_acceleration()
{
	return acceleration;
}

void CameraController::set_acceleration(float value)
{
	acceleration = value;
}
float CameraController::get_deceleration()
{
	return deceleration;
}

void CameraController::set_deceleration(float value)
{
	deceleration = value;
}
Vector3 CameraController::get_max_speed()
{
	return max_speed;
}

void CameraController::set_max_speed(Vector3 value)
{
	max_speed = value;
}
void CameraController::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_acceleration"), &CameraController::get_acceleration);
	ClassDB::bind_method(D_METHOD("set_acceleration", "value"), &CameraController::set_acceleration);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "acceleration"), "set_acceleration", "get_acceleration");

	ClassDB::bind_method(D_METHOD("get_deceleration"), &CameraController::get_deceleration);
	ClassDB::bind_method(D_METHOD("set_deceleration", "value"), &CameraController::set_deceleration);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "deceleration"), "set_deceleration", "get_deceleration");
}
void CameraController::_process(double delta)
{
	Input *input = Input::get_singleton();
	direction.zero();
	speed.zero();
	direction.x = input->get_action_strength("ui_right") - input->get_action_strength("ui_left");

	direction.z = input->get_action_strength("ui_down") - input->get_action_strength("ui_up");

	if (input->is_action_just_released("zoom_out"))
	{
		direction.y = 20.0;
	}
	else if (input->is_action_just_released("zoom_in"))
	{
		direction.y = -20.0;
	}

	Vector3 step = max_speed * acceleration * direction;
	// UtilityFunctions::print(direction);
	speed = Math::clamp<Vector3>(speed + step, -max_speed, max_speed);

	global_translate(speed * delta);
}
