extends Node3D
class_name ProvinceSelector

const RAY_LENGTH = 1000.0

@export var map_data: MapData
@export var country_data: CountryData
@export var province_map: Sprite3D 

var province_image: Image:
	set(value):
		province_image = value
signal map_change_triggered()
var country_id: String
var province_id: int
var last_mouse_position: Vector2 = Vector2.ZERO


	
func get_current_camera() -> Camera3D:
	return get_viewport().get_camera_3d()


func select_province() -> void:

	
	var space: PhysicsDirectSpaceState3D = get_world_3d().get_direct_space_state()
	var camera: Camera3D = get_current_camera()
	
	if not camera:
		return
	

	
	var from: Vector3 = camera.project_ray_origin(last_mouse_position)
	var to: Vector3 = from + camera.project_ray_normal(last_mouse_position) * RAY_LENGTH
	var query: PhysicsRayQueryParameters3D = PhysicsRayQueryParameters3D.create(from, to)
	var result: Dictionary = space.intersect_ray(query)
	

	
	if not result.is_empty():
		var world_position: Vector3 = result["position"]
		var texture = province_image
		
		if not texture:
			return
		
	
		var texture_size: Vector2 = texture.get_size()
		var pixel_size: float = province_map.get_pixel_size()
		
		# Convert to relative coordinate space
		var half_width: float = texture_size.x * pixel_size * 0.5
		var half_height: float = texture_size.y * pixel_size * 0.5
		
		var texture_x: int = int((world_position.x + half_width) / pixel_size)
		var texture_y: int = int((world_position.z + half_height) / pixel_size)
		
		texture_x = clampi(texture_x, 0, int(texture_size.x) - 1)
		texture_y = clampi(texture_y, 0, int(texture_size.y) - 1)
	
		

		var province_color: Color = texture.get_pixel(texture_x, texture_y)

		
		print("Texture coordinates: ", texture_x, ", ", texture_y)
		print("World position: ", world_position)
		print("Province Color Value: ", province_color)
		

		var province_name: String = map_data.province_color_to_name[province_color]

		print("Province Name: ", province_name)
		

		province_id = map_data.province_color_to_id[province_color]
		print("Province ID: ", province_id)
		

		country_id = country_data.province_id_to_owner[province_id]
		print("country_id ID: ", country_id)
		
		map_change_triggered.emit()


func _input(event: InputEvent) -> void:
	if event is InputEventMouseButton:
		var mouse_event: InputEventMouseButton = event as InputEventMouseButton
		if mouse_event.is_pressed():
			last_mouse_position = mouse_event.get_position()
