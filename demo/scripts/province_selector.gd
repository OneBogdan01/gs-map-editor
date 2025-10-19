extends Node3D
class_name ProvinceSelector

const RAY_LENGTH = 1000.0

@export var province_parser: MapData
@export var country_data: CountryData
@export var province_map: Sprite3D 
@export var province_tex: Texture2D
var province_image
signal province_selected(province_id: int)
signal country_selected(country_id: String)


var last_mouse_position: Vector2 = Vector2.ZERO


func _ready() -> void:
	province_image = province_tex.get_image()
	
func get_current_camera() -> Camera3D:
	return get_viewport().get_camera_3d()


func select_province() -> void:
	var time_start = Time.get_ticks_usec()
	
	var space: PhysicsDirectSpaceState3D = get_world_3d().get_direct_space_state()
	var camera: Camera3D = get_current_camera()
	
	if not camera:
		return
	
	var time_after_camera = Time.get_ticks_usec()
	
	var from: Vector3 = camera.project_ray_origin(last_mouse_position)
	var to: Vector3 = from + camera.project_ray_normal(last_mouse_position) * RAY_LENGTH
	var query: PhysicsRayQueryParameters3D = PhysicsRayQueryParameters3D.create(from, to)
	var result: Dictionary = space.intersect_ray(query)
	
	var time_after_raycast = Time.get_ticks_usec()
	
	if not result.is_empty():
		var world_position: Vector3 = result["position"]
		var texture = province_image
		
		if not texture:
			return
		
		var time_before_coord_calc = Time.get_ticks_usec()
		var texture_size: Vector2 = texture.get_size()
		var pixel_size: float = province_map.get_pixel_size()
		
		# Convert to relative coordinate space
		var half_width: float = texture_size.x * pixel_size * 0.5
		var half_height: float = texture_size.y * pixel_size * 0.5
		
		var texture_x: int = int((world_position.x + half_width) / pixel_size)
		var texture_y: int = int((world_position.z + half_height) / pixel_size)
		
		texture_x = clampi(texture_x, 0, int(texture_size.x) - 1)
		texture_y = clampi(texture_y, 0, int(texture_size.y) - 1)
		var time_after_coord_calc = Time.get_ticks_usec()
		
		var time_before_get_pixel = Time.get_ticks_usec()
		var province_color: Color = texture.get_pixel(texture_x, texture_y)
		var time_after_get_pixel = Time.get_ticks_usec()
		
		print("Texture coordinates: ", texture_x, ", ", texture_y)
		print("World position: ", world_position)
		print("Province Color Value: ", province_color)
		
		var time_before_province_name = Time.get_ticks_usec()
		var province_name: String = province_parser.province_color_to_name[province_color]
		var time_after_province_name = Time.get_ticks_usec()
		print("Province Name: ", province_name)
		
		var time_before_province_id = Time.get_ticks_usec()
		var province_id: int = province_parser.province_color_to_id[province_color]
		var time_after_province_id = Time.get_ticks_usec()
		print("Province ID: ", province_id)
		
		var time_before_country_lookup = Time.get_ticks_usec()
		var country_id: String = country_data.province_id_to_owner[province_id]
		var time_after_country_lookup = Time.get_ticks_usec()
		print("country_id ID: ", country_id)
		
		var time_before_signals = Time.get_ticks_usec()
		province_selected.emit(province_id)
		country_selected.emit(country_id)
		var time_after_signals = Time.get_ticks_usec()
		
		var time_end = Time.get_ticks_usec()
		
		# Print timing breakdown
		print("=== TIMING BREAKDOWN ===")
		print("Camera setup: ", (time_after_camera - time_start) / 1000.0, " ms")
		print("Raycast: ", (time_after_raycast - time_after_camera) / 1000.0, " ms")
		print("Coordinate calculation: ", (time_after_coord_calc - time_before_coord_calc) / 1000.0, " ms")
		print("get_pixel(): ", (time_after_get_pixel - time_before_get_pixel) / 1000.0, " ms")
		print("get_province_from_color(): ", (time_after_province_name - time_before_province_name) / 1000.0, " ms")
		print("get_province_id_from_color(): ", (time_after_province_id - time_before_province_id) / 1000.0, " ms")
		print("Country lookup: ", (time_after_country_lookup - time_before_country_lookup) / 1000.0, " ms")
		print("Signal emissions: ", (time_after_signals - time_before_signals) / 1000.0, " ms")
		print("Total: ", (time_end - time_start) / 1000.0, " ms")
		print("========================")

func _input(event: InputEvent) -> void:
	if event is InputEventMouseButton:
		var mouse_event: InputEventMouseButton = event as InputEventMouseButton
		if mouse_event.is_pressed():
			last_mouse_position = mouse_event.get_position()
