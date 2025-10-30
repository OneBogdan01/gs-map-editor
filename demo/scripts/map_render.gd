extends ComputeHelper
@export_group("Sub Viewports")
@export var country_field: SubViewport
@export var province_field: SubViewport
@export var output: SubViewport

@export_group("Data")
@export var map_data: MapData
@export var country_data: CountryData
@export var province_selector: ProvinceSelector
@export var province_map: Texture2D

@export_group("Texture & Shader Configuration")
@export var color_map_size: Vector2i
@export_file var lookup_path_shader = "res://shaders/generate_color_lookup.glsl"
@export_file var color_path_shader = "res://shaders/generate_color_map.glsl"
@export_file var mask_political_path_shader = "res://shaders/mask_political_map.glsl"


@export_group("Debugging & Profiling")
@export var profiler_enabled := true
@export var save_images_to_file := true
@export_file var lookup_save_path = "res://assets/color_lookup_map.png"
@export_file var color_map_save_path = "res://assets/color_map.png"
@export_file var mask_political_save_path = "res://assets/mask_political_map.png"


# Update the viewport materials
var output_material: ShaderMaterial
var distance_material: ShaderMaterial
var province_material: ShaderMaterial


# Cached variables
var color_lookup: Image
var color_map: Image
var color_texture: ImageTexture
var political_map: Image

# Gameplay
var is_getting_country = false
var selected_country: String
var is_political = true
# Rudimentary profiling
func time_function(function_name: String, callable: Callable):
	if not profiler_enabled:
		return callable.call()
	
	var start = Time.get_ticks_usec()
	var result = callable.call()
	var time_ms = (Time.get_ticks_usec() - start) / 1000.0
	print("[%s] %.2f ms" % [function_name, time_ms])
	return result

func update_material_dynamic_parameters(parameter_name, parameter_variant):
		output_material.set_shader_parameter(parameter_name, parameter_variant)
		distance_material.set_shader_parameter(parameter_name, parameter_variant)
func update_material_static_parameters(parameter_name, parameter_variant):
		province_material.set_shader_parameter(parameter_name, parameter_variant)
func update_viewports_dynamic():
		country_field.render_target_update_mode = SubViewport.UPDATE_ONCE
		country_field.render_target_clear_mode = SubViewport.CLEAR_MODE_ONCE
		
		await RenderingServer.frame_post_draw
		output.render_target_update_mode = SubViewport.UPDATE_ONCE
		output.render_target_clear_mode = SubViewport.CLEAR_MODE_ONCE


func update_color_map(province_id, new_color):
	var witdh = color_map.get_width()
	var x = province_id % witdh
	var y = province_id / witdh
	color_map.set_pixel(x, y, new_color)
	color_texture.update(color_map)
	update_material_dynamic_parameters("color_map", color_texture)
	update_viewports_dynamic()

	
func _ready():
	province_selector.province_image = province_map.get_image()
	# Initialize compute helper
	create_rd()
	set_output_texture_size(province_map.get_size())
	
	# Get viewport materials to update at runtime
	var output_color: ColorRect = output.get_node("Output")
	output_material = output_color.material
	
	var distance_color: ColorRect = country_field.get_node("Output")
	distance_material = distance_color.material
	
	var province_output: ColorRect = province_field.get_node("Output")
	province_material = province_output.material
	
	
	# Only needs to be created once
	create_lookup_texture()
	create_color_map_texture()
	# Create mask texture once and update the viewport
	create_political_map_mask_texture()

	province_field.render_target_update_mode = SubViewport.UPDATE_ONCE
	await RenderingServer.frame_post_draw
	update_viewports_dynamic()
		

func _physics_process(_delta: float) -> void:
	if Input.is_action_just_pressed("country_select"):
		is_getting_country = true
		province_selector.select_province()
	elif Input.is_action_just_pressed("select"):
		province_selector.select_province()

func colors_equal(a: Color, b: Color, tolerance = 0.01):
	return (abs(a.r - b.r) < tolerance &&
			abs(a.g - b.g) < tolerance &&
			abs(a.b - b.b) < tolerance &&
			abs(a.a - b.a) < tolerance);

func color_map_remove_color(col_map: Image, color_to_remove: Color):
	for y in range(col_map.get_height()):
		for x in range(col_map.get_width()):
			var pixel: Color = col_map.get_pixel(x, y)
			
			if colors_equal(pixel, color_to_remove):
				col_map.set_pixel(x, y, Color(0, 0, 0, 0))

func color_map_remove_non_country_color(col_map: Image):
	var removed_color_map = Image.create_from_data(col_map.get_width(),
	 col_map.get_height(),
	 false,
	 col_map.get_format(),
	 col_map.get_data())
	
	var terrain_colors = country_data.terrain_colors.values()
	for terrain_color in terrain_colors:
		terrain_color.a = 1.
		color_map_remove_color(removed_color_map, terrain_color)
	return removed_color_map
func create_political_map_mask_texture():
	# Create output texture format 
	if not color_lookup:
		push_error("No lookup texture found")
		return
	var texture_size = color_lookup.get_size()
	
	var output_format = texture_format_from_texture_2d(texture_size,
	 RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM,
	 RenderingDevice.TEXTURE_USAGE_STORAGE_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT)

	var lookup_format = texture_format_from_texture_2d(texture_size,
	 RenderingDevice.DATA_FORMAT_R8G8_UNORM,
	 RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice.TEXTURE_USAGE_STORAGE_BIT)

	var color_format = texture_format_from_texture_2d(color_map.get_size(),
	 RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM,
	 RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice.TEXTURE_USAGE_STORAGE_BIT)


	var removed_terrain_colors_map = color_map_remove_non_country_color(color_map)

	var color_tex = create_texture(color_format, RDTextureView.new(), [removed_terrain_colors_map.get_data()])
	var look_image = create_texture(lookup_format, RDTextureView.new(), [color_lookup.get_data()])
	var political_image = create_texture(output_format, RDTextureView.new(), [])
	
	
	var lookup_uniform = create_uniform(look_image, 0, RenderingDevice.UNIFORM_TYPE_IMAGE)

	var color_uniform := create_uniform(color_tex, 1, RenderingDevice.UNIFORM_TYPE_IMAGE)
	
	var political_uniform = create_uniform(political_image, 2, RenderingDevice.UNIFORM_TYPE_IMAGE)

	
	var shader = compile_shader(mask_political_path_shader)
   

	var byte_data: PackedByteArray = compute_result([political_uniform, color_uniform, lookup_uniform], political_image, shader)


	# Create new image from the result
	var result_image = Image.create_from_data(color_lookup.get_width(), color_lookup.get_height(), false, Image.FORMAT_RGBA8, byte_data)
	if save_images_to_file:
		result_image.save_png(mask_political_save_path)
	
	update_material_static_parameters("mask_map", ImageTexture.create_from_image(result_image))
	

func create_color_map_texture():
	# Size of the colormap
	const TEXTURE_SIZE = Vector2i(256, 256)
	var output_format = texture_format_from_texture_2d(TEXTURE_SIZE, RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM, RenderingDevice.TEXTURE_USAGE_STORAGE_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT)

	var output_image = create_texture(output_format, RDTextureView.new(), [])
   
   
	var output_uniform = create_uniform(output_image, 0, RenderingDevice.UNIFORM_TYPE_IMAGE)
	
	var buffer: PackedInt32Array = country_data.populate_color_map_buffers()
	var buffer_bytes := buffer.to_byte_array()
	
	
	var buffer_storage = create_ssbo(buffer_bytes.size(), buffer_bytes)
	var uniform_buffer = create_uniform(buffer_storage, 1, RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER)

	var shader = compile_shader(color_path_shader)
   
	var byte_data: PackedByteArray = compute_result([output_uniform, uniform_buffer], output_image, shader);

	# Create new image from the result
	var result_image = Image.create_from_data(TEXTURE_SIZE.x, TEXTURE_SIZE.y, false, Image.FORMAT_RGBA8, byte_data)
	color_map = result_image
	# for debugging
	if save_images_to_file:
		result_image.save_png(color_map_save_path)
	# store the texture so it can be updated later
	color_texture = ImageTexture.create_from_image(color_map)
	update_material_dynamic_parameters("color_map", color_texture)

	return result_image
	
func create_lookup_texture():
	if not province_map:
		push_error("No province map set.")
		return
	var province_size = province_map.get_size()


	var shader = compile_shader(lookup_path_shader)
   
	var input_format = texture_format_from_texture_2d(province_size,
	 RenderingDevice.DATA_FORMAT_R8G8B8A8_UINT,
	 RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice.TEXTURE_USAGE_STORAGE_BIT)

	
	var image_data = province_map.get_image()
	image_data.convert(Image.FORMAT_RGBA8)
	var input_image = create_texture(input_format, RDTextureView.new(), [image_data.get_data()])

	if not input_image.is_valid():
		print("Failed to create GPU input texture")
		clean_up()
		return
   

	var output_format = texture_format_from_texture_2d(province_size, RenderingDevice.DATA_FORMAT_R8G8_UNORM, RenderingDevice.TEXTURE_USAGE_STORAGE_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT)
	# Create empty output texture
	var output_image = create_texture(output_format, RDTextureView.new(), [])
	
	if not output_image.is_valid():
		print("Failed to create output texture")
		clean_up()
		return
   
	
	var input_uniform = create_uniform(input_image, 0, RenderingDevice.UNIFORM_TYPE_IMAGE)

	var output_uniform = create_uniform(output_image, 1, RenderingDevice.UNIFORM_TYPE_IMAGE)

	var color_keys = map_data.province_color_to_id.keys()
	
	var colors := PackedInt32Array()

	for color in color_keys:
		var id = map_data.province_color_to_id[color]
		colors.append_array(PackedInt32Array([color.r8, color.g8, color.b8, id]))
			
	var color_bytes := colors.to_byte_array()

	
	# Create a storage buffer that can hold our values.

	var buffer_color := create_ssbo(color_bytes.size(), color_bytes)
	var uniform_color = create_uniform(buffer_color, 2, RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER)

	# Get the result back
	var byte_data: PackedByteArray = compute_result([input_uniform, output_uniform, uniform_color], output_image, shader);


	if byte_data.size() == 0:
		push_error("No data retrieved from GPU texture!")
		return
	
	# Create new image from the result
	var texture_size = get_output_texture_size()
	var result_image = Image.create_from_data(texture_size.x, texture_size.y, false, Image.FORMAT_RG8, byte_data)
	color_lookup = result_image
	# for debugging
	if save_images_to_file:
		result_image.save_png(lookup_save_path)
	var result_tex = ImageTexture.create_from_image(color_lookup)
	update_material_dynamic_parameters("lookup_map", result_tex)
	update_material_static_parameters("lookup_map", result_tex)


func on_province_selector_province_selected(province_id: int) -> void:
	if is_getting_country == false and selected_country.is_empty() == false:
		# change the province owner
		country_data.province_id_to_owner[province_id] = selected_country;

		call_deferred("update_color_map", province_id, country_data.country_id_to_color[selected_country])


func on_province_selector_country_selected(country: String) -> void:
	if is_getting_country:
		selected_country = country
		print("Selected country: ", selected_country)
		is_getting_country = false


func on_province_selector_map_change_triggered() -> void:
	var province_id: int = province_selector.province_id
	var country_id: String = province_selector.country_id
	
	time_function("Selector province", on_province_selector_province_selected.bind(province_id))
	time_function("Selector country", on_province_selector_country_selected.bind(country_id))
