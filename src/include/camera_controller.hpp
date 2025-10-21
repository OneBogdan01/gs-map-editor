#pragma once

#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/variant/vector3.hpp"
namespace godot
{

// very basic camera used for the demo of this plugin, can be replaced
class CameraController : public Node3D
{
	GDCLASS(CameraController, Node3D);

public:
	void _process(double delta) override;
	float get_acceleration();
	void set_acceleration(float value);
	float get_deceleration();
	void set_deceleration(float value);
	Vector3 get_max_speed();
	void set_max_speed(Vector3 value);

protected:
	static void _bind_methods();

private:
	Vector3 direction;
	Vector3 speed;

	bool local = false;
	float acceleration{ 1.0 };
	float deceleration{ 0.4 };
	Vector3 max_speed{ 1.0, 1.0, 1.0 };
};
} // namespace godot