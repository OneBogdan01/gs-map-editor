
extends Node3D
@onready var sprite_3d: Sprite3D = $"."
@onready var map_data: MapData = $"../ProvinceSelector/MapData"
@onready var country_data: CountryData = $"../ProvinceSelector/CountryData"
@onready var province_selector: ProvinceSelector = $"../ProvinceSelector"
@onready var province_map: Sprite3D = $ProvinceMap

var color_lookup: Image
var color_map: Image
var political_map: Image
var edge_map: Image


var profiler_enabled := true

# Simple timing function
func time_function(name: String, callable: Callable):
	if not profiler_enabled:
		return callable.call()
	
	var start = Time.get_ticks_usec()
	var result = callable.call()
	var time_ms = (Time.get_ticks_usec() - start) / 1000.0
	print("[%s] %.2f ms" % [name, time_ms])
	return result
func _update_shader_parameters(name, parameterVariant):
		sprite_3d.material_override.set_shader_parameter(name, parameterVariant)

func _reload_political_texture():
	sprite_3d.material_override.set_shader_parameter("political_map", ImageTexture.create_from_image(political_map))
	
	
func _generate_map():
	time_function("Color Map", create_color_map_texture)
	#time_function("Distance Field",create_edge_map_texture)
	time_function("Political Map", create_political_map_texture) 
	#time_function("Reload Texture", _reload_political_texture)

	
func _ready():
	# TODO This should be way faster than it is now
	country_data.parse_all_files()
	map_data.load_csv_data()
	create_lookup_texture()
	_generate_map()
	
		
var is_getting_country = false
var selected_country:String
var is_political = true
func _physics_process(delta: float) -> void:
	if Input.is_action_just_pressed("country_select"):
		is_getting_country = true
		province_selector.select_province()
	elif  Input.is_action_just_pressed("select"):
		province_selector.select_province()

func _process(delta: float) -> void:
	# debugging
	if Input.is_action_just_pressed("change_map"):
		if is_political:
			_reload_political_texture()
			is_political = false
		else:
			sprite_3d.texture = preload("res://assets/provinces.bmp")
			is_political = true

static func sort(a, b):
	if a["Id"] < b["Id"]:
		return true
	return false
func create_political_map_texture():
	var rd = RenderingServer.create_local_rendering_device()
	# Create output texture format 
	const TEXTURE_SIZE = Vector2i(5632, 2048)
	var output_format = RDTextureFormat.new()
	output_format.width = TEXTURE_SIZE.x
	output_format.height = TEXTURE_SIZE.y
	output_format.format = RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM
	output_format.usage_bits =  RenderingDevice.TEXTURE_USAGE_STORAGE_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT

	# Create empty output texture
	var lookup_format = RDTextureFormat.new()
	lookup_format.width = TEXTURE_SIZE.x
	lookup_format.height = TEXTURE_SIZE.y
	lookup_format.format = RenderingDevice.DATA_FORMAT_R8G8_UNORM
	lookup_format.usage_bits = RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice.TEXTURE_USAGE_STORAGE_BIT
	

	var look_data = color_lookup
	look_data.convert(Image.FORMAT_RG8)
	
	var fmt = RDTextureFormat.new()
	fmt.width = color_map.get_width()
	fmt.height = color_map.get_height()
	fmt.format = RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM
	fmt.usage_bits = RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice.TEXTURE_USAGE_STORAGE_BIT



	# Create GPU input texture with image data
	var tex = rd.texture_create(fmt, RDTextureView.new(), [color_map.get_data()])
	var look_image = rd.texture_create(lookup_format, RDTextureView.new(), [look_data.get_data()])
	var political_image = rd.texture_create(output_format, RDTextureView.new(), [])
	

	
	var lookup_uniform = RDUniform.new()
	lookup_uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	lookup_uniform.binding = 0
	lookup_uniform.add_id(look_image)

	var sampler_uniform := RDUniform.new()
	sampler_uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	sampler_uniform.binding = 1
	sampler_uniform.add_id(tex)
	
	var political_uniform = RDUniform.new()
	political_uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	political_uniform.binding = 2
	political_uniform.add_id(political_image)
	

	var shader_file = load("res://shaders/political_map.glsl")
	
	var shader_spirv: RDShaderSPIRV = shader_file.get_spirv()
	var shader = rd.shader_create_from_spirv(shader_spirv)
   

	var uniform_set = rd.uniform_set_create([political_uniform, sampler_uniform, lookup_uniform], shader, 0)

 
	# Dispatch compute shader
	var pipeline := rd.compute_pipeline_create(shader)
	var compute_list = rd.compute_list_begin()
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, uniform_set, 0)
   
	# Calculate dispatch groups (8x8 work group size)
	var group_size_x = (TEXTURE_SIZE.x + 7) / 8
	var group_size_y = (TEXTURE_SIZE.y + 7) / 8
   
	rd.compute_list_dispatch(compute_list, group_size_x, group_size_y, 1)
	rd.compute_list_end()
   
	# Submit and wait
	rd.submit()
	rd.sync()
	# Get the result back
	var byte_data: PackedByteArray = rd.texture_get_data(political_image, 0)


	
	# Create new image from the result
	var result_image = Image.create_from_data(TEXTURE_SIZE.x, TEXTURE_SIZE.y, false, Image.FORMAT_RGBA8, byte_data)
	#result_image.save_png("res://assets/political_map.png")
	political_map = result_image
	_update_shader_parameters("country_map", ImageTexture.create_from_image(political_map))



func create_edge_map_texture():
	var rd = RenderingServer.create_local_rendering_device()
	# Create output texture format 
	const TEXTURE_SIZE = Vector2i(5632, 2048)/4
	var lookup_format = RDTextureFormat.new()
	lookup_format.width = TEXTURE_SIZE.x
	lookup_format.height = TEXTURE_SIZE.y
	lookup_format.format = RenderingDevice.DATA_FORMAT_R8G8_UNORM
	lookup_format.usage_bits =  RenderingDevice.TEXTURE_USAGE_STORAGE_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT

	var color_format = RDTextureFormat.new()
	color_format.width = color_map.get_width()
	color_format.height = color_map.get_height()
	color_format.format = RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM
	color_format.usage_bits = RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice.TEXTURE_USAGE_STORAGE_BIT

	# Create empty output texture
	var edge_format = RDTextureFormat.new()
	edge_format.width = TEXTURE_SIZE.x
	edge_format.height = TEXTURE_SIZE.y
	edge_format.format = RenderingDevice.DATA_FORMAT_R8_UNORM
	edge_format.usage_bits = RenderingDevice.TEXTURE_USAGE_STORAGE_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT
	
		# Create empty output texture
	var temp_format = RDTextureFormat.new()
	temp_format.width = TEXTURE_SIZE.x
	temp_format.height = TEXTURE_SIZE.y
	temp_format.format = RenderingDevice.DATA_FORMAT_R8G8_UNORM
	temp_format.usage_bits = RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT | RenderingDevice.TEXTURE_USAGE_STORAGE_BIT
	


	# Create GPU input texture with image data
	var downsampled_lookup = color_lookup
	downsampled_lookup.resize(TEXTURE_SIZE.x,TEXTURE_SIZE.y,Image.INTERPOLATE_BILINEAR)
	var lookup_image = rd.texture_create(lookup_format, RDTextureView.new(), [color_lookup.get_data()])
	var color_image = rd.texture_create(color_format, RDTextureView.new(), [color_map.get_data()])
	
	var edge_image = rd.texture_create(edge_format, RDTextureView.new(), [])
	var buffer_a_image = rd.texture_create(temp_format, RDTextureView.new(), [])
	
	var buffer_b_image = rd.texture_create(temp_format, RDTextureView.new(), [])
	
	

	
	var lookup_uniform = RDUniform.new()
	lookup_uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	lookup_uniform.binding = 0
	lookup_uniform.add_id(lookup_image)

	var color_uniform = RDUniform.new()
	color_uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	color_uniform.binding = 1
	color_uniform.add_id(color_image)
	
	var buffer_a_uniform := RDUniform.new()
	buffer_a_uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	buffer_a_uniform.binding = 2
	buffer_a_uniform.add_id(buffer_a_image)
	
	var buffer_b_uniform := RDUniform.new()
	buffer_b_uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	buffer_b_uniform.binding = 3
	buffer_b_uniform.add_id(buffer_b_image)
	
	var distance_uniform := RDUniform.new()
	distance_uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	distance_uniform.binding =2
	distance_uniform.add_id(edge_image)
	
	

	var shader_file = load("res://shaders/generate_borders.glsl")
	
	var shader_spirv: RDShaderSPIRV = shader_file.get_spirv()
	var shader = rd.shader_create_from_spirv(shader_spirv)
   

	var uniform_set = rd.uniform_set_create([lookup_uniform, color_uniform, distance_uniform], shader, 0)

 
	# Dispatch compute shader
	var pipeline := rd.compute_pipeline_create(shader)
	var compute_list = rd.compute_list_begin()
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, uniform_set, 0)
   
	# Calculate dispatch groups (8x8 work group size)
	var group_size_x = (TEXTURE_SIZE.x + 7) / 8
	var group_size_y = (TEXTURE_SIZE.y + 7) / 8
   
	rd.compute_list_dispatch(compute_list, group_size_x, group_size_y, 1)
	rd.compute_list_end()
   
	# Submit and wait
	rd.submit()
	rd.sync()
	# Get the result back
	var byte_data: PackedByteArray = rd.texture_get_data(edge_image, 0)


	
	# Create new image from the result
	var result_image = Image.create_from_data(TEXTURE_SIZE.x, TEXTURE_SIZE.y, false, Image.FORMAT_R8, byte_data)
	result_image.save_png("res://assets/edge_map.png")
	edge_map = result_image
func create_color_map_texture():
	var rd = RenderingServer.create_local_rendering_device()
	# TODO get the data already sorted and without useless data
	var start = Time.get_ticks_usec()

	
	# calls cpp code
	var buffer:PackedInt32Array = country_data.populate_color_map_buffers()
		
	var data_prep_time = (Time.get_ticks_usec() - start) / 1000.0
	print("[Data Preparation] %.2f ms" % data_prep_time)
	var buffer_bytes := buffer.to_byte_array()
	
	
	var buffer_storage := rd.storage_buffer_create(buffer_bytes.size(), buffer_bytes)
	var uniform_buffer := RDUniform.new()
	uniform_buffer.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform_buffer.binding = 1 # this needs to match the "binding" in our shader file
	uniform_buffer.add_id(buffer_storage)
	
	
	# Create output texture format 
	const TEXTURE_SIZE = Vector2i(256, 256)
	var output_format = RDTextureFormat.new()
	output_format.width = TEXTURE_SIZE.x
	output_format.height = TEXTURE_SIZE.y
	output_format.format = RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM
	output_format.usage_bits = RenderingDevice.TEXTURE_USAGE_STORAGE_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT

	# Create empty output texture
	var output_image = rd.texture_create(output_format, RDTextureView.new(), [])
   
   
	var output_uniform = RDUniform.new()
	output_uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	output_uniform.binding = 0
	output_uniform.add_id(output_image)
	
	var shader_file = load("res://shaders/generate_color_map.glsl")

		
	var shader_spirv: RDShaderSPIRV = shader_file.get_spirv()
	var shader = rd.shader_create_from_spirv(shader_spirv)
   
	var uniform_set = rd.uniform_set_create([output_uniform, uniform_buffer], shader, 0)

 
	# Dispatch compute shader
	var pipeline := rd.compute_pipeline_create(shader)
	var compute_list = rd.compute_list_begin()
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, uniform_set, 0)
   
	# Calculate dispatch groups (8x8 work group size)
	var group_size_x = (TEXTURE_SIZE.x + 7) / 8
	var group_size_y = (TEXTURE_SIZE.y + 7) / 8
   
	rd.compute_list_dispatch(compute_list, group_size_x, group_size_y, 1)
	rd.compute_list_end()
   
	# Submit and wait
	rd.submit()
	rd.sync()
	# Get the result back
	var byte_data: PackedByteArray = rd.texture_get_data(output_image, 0)


	
	# Create new image from the result
	var result_image = Image.create_from_data(TEXTURE_SIZE.x, TEXTURE_SIZE.y, false, Image.FORMAT_RGBA8, byte_data)
	color_map = result_image
	# for debugging
	#result_image.save_png("res://assets/color_map.png")
	_update_shader_parameters("color_map",ImageTexture.create_from_image(color_map))
	
	
	return result_image
	
func create_lookup_texture():
	# Get the rendering device
	var rd = RenderingServer.create_local_rendering_device()
	
	# Variables to track resources for cleanup
	var input_image = null
	var output_image = null
	var shader = null
	var uniform_set = null
	
	# Get input texture
	var input_texture = province_map.texture
	if not input_texture:
		print("No texture found on Sprite3D")
		return
   
	# Get texture dimensions
	var texture_size = input_texture.get_size()
	
	# Use defer for cleanup in case of early returns or errors
	var cleanup = func():
		print("Cleaning up GPU resources...")
		if uniform_set != null and uniform_set.is_valid():
			rd.free_rid(uniform_set)
		if input_image != null and input_image.is_valid():
			rd.free_rid(input_image)
		if output_image != null and output_image.is_valid():
			rd.free_rid(output_image)
		if shader != null and shader.is_valid():
			rd.free_rid(shader)
   
	# Load compute shader
	var shader_file = load("res://shaders/generate_color_lookup.glsl")
	if not shader_file:
		print("Failed to load shader file")
		cleanup.call()
		return
		
	var shader_spirv: RDShaderSPIRV = shader_file.get_spirv()
	shader = rd.shader_create_from_spirv(shader_spirv)
	if not shader.is_valid():
		print("Failed to create shader")
		cleanup.call()
		return
   
	# Create input texture format
	var input_format = RDTextureFormat.new()
	input_format.width = texture_size.x
	input_format.height = texture_size.y
	input_format.format = RenderingDevice.DATA_FORMAT_R8G8B8A8_UINT
	input_format.usage_bits = RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice.TEXTURE_USAGE_STORAGE_BIT
	
	# Get the CPU image and its data
	var cpu_image = input_texture.get_image()
	if cpu_image.is_empty():
		print("Input image is empty")
		cleanup.call()
		return
	
	# Convert image format to match GPU format
	cpu_image.convert(Image.FORMAT_RGBA8)
	var image_data = cpu_image.get_data()
	
	# Create GPU input texture with image data
	input_image = rd.texture_create(input_format, RDTextureView.new(), [image_data])
	if not input_image.is_valid():
		print("Failed to create GPU input texture")
		cleanup.call()
		return
   
	# Create output texture format 
	var output_format = RDTextureFormat.new()
	output_format.width = texture_size.x
	output_format.height = texture_size.y
	output_format.format = RenderingDevice.DATA_FORMAT_R8G8_UNORM
	output_format.usage_bits = RenderingDevice.TEXTURE_USAGE_STORAGE_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT

	# Create empty output texture
	output_image = rd.texture_create(output_format, RDTextureView.new(), [])
	if not output_image.is_valid():
		print("Failed to create output texture")
		cleanup.call()
		return
   
	# Create uniform set
	var input_uniform = RDUniform.new()
	input_uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	input_uniform.binding = 0
	input_uniform.add_id(input_image)
   
	var output_uniform = RDUniform.new()
	output_uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	output_uniform.binding = 1
	output_uniform.add_id(output_image)
	var province_data = map_data.province_data
	var colors := PackedInt32Array()

	for i in province_data.size():
		var dict = province_data[i]
		var color:Color = dict["Color"]
		var id:int = dict["Id"]
		colors.append_array(PackedInt32Array([color.r8, color.g8, color.b8,id]))
			
	var color_bytes := colors.to_byte_array()

	

	# Create a storage buffer that can hold our values.

	var buffer_color := rd.storage_buffer_create(color_bytes.size(), color_bytes)
	var uniform_color := RDUniform.new()
	uniform_color.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform_color.binding = 2 # this needs to match the "binding" in our shader file
	uniform_color.add_id(buffer_color)
	

	uniform_set = rd.uniform_set_create([input_uniform, output_uniform,uniform_color], shader, 0)
	if not uniform_set.is_valid():
		print("Failed to create uniform set")
		cleanup.call()
		return
 
	# Dispatch compute shader
	var pipeline := rd.compute_pipeline_create(shader)
	var compute_list = rd.compute_list_begin()
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, uniform_set, 0)
   
	# Calculate dispatch groups (8x8 work group size)
	var group_size_x = (texture_size.x + 7) / 8
	var group_size_y = (texture_size.y + 7) / 8
   
	rd.compute_list_dispatch(compute_list, group_size_x, group_size_y, 1)
	rd.compute_list_end()
   
	# Submit and wait
	rd.submit()
	rd.sync()
	# Get the result back
	var byte_data: PackedByteArray = rd.texture_get_data(output_image, 0)


	if byte_data.size() == 0:
		print("ERROR: No data retrieved from GPU texture!")
		cleanup.call()
		return
	
	# Create new image from the result
	var result_image = Image.create_from_data(texture_size.x, texture_size.y, false, Image.FORMAT_RG8, byte_data)
	color_lookup = result_image
	# for debugging
	result_image.save_png("res://assets/color_lookup.png")  # In project directory
	_update_shader_parameters("lookup_map",ImageTexture.create_from_image(color_lookup))
	
	print("Texture processing completed successfully!")


	cleanup.call()


func _on_province_selector_province_selected(province_id: int) -> void:
	if is_getting_country == false and selected_country.is_empty() == false:
		country_data.change_province_owner(province_id,selected_country)
		_generate_map()


func _on_province_selector_country_selected(country: String) -> void:
	if is_getting_country:
		selected_country = country
		print("Selected country: ", selected_country)
		is_getting_country = false
	
		
