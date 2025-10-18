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


@export_group("Debugging & Profiling")
@export var profiler_enabled := true
@export var save_images_to_file := true


# Update the viewport materials
var output_material: ShaderMaterial
var distance_material: ShaderMaterial

var color_lookup: Image
var color_map: Image
var political_map: Image

var is_getting_country = false
var selected_country: String
var is_political = true
# Rudimentary profiling
func time_function(name: String, callable: Callable):
	if not profiler_enabled:
		return callable.call()
	
	var start = Time.get_ticks_usec()
	var result = callable.call()
	var time_ms = (Time.get_ticks_usec() - start) / 1000.0
	print("[%s] %.2f ms" % [name, time_ms])
	return result
func update_material_parameters(name, parameterVariant):
		output_material.set_shader_parameter(name, parameterVariant)
		distance_material.set_shader_parameter(name, parameterVariant)

func update_viewports():
		country_field.render_target_update_mode = SubViewport.UPDATE_ONCE
		#output.render_target_update_mode = SubViewport.UPDATE_ONCE

func update_color_map(province_id, new_color):
	var x = province_id % color_map.get_width()
	var y = province_id / color_map.get_height()
	
	color_map.set_pixel(x, y, new_color)

	
func _ready():
	# Initialize compute helper
	create_rd()
	set_output_texture_size(province_map.get_size())
	
	# Get viewport materials to update at runtime
	var output_color: ColorRect = output.get_node("Output")
	var distance_color: ColorRect = country_field.get_node("Output")
	output_material = output_color.material
	distance_material = distance_color.material
	
	# Load data
	country_data.parse_all_files()
	map_data.load_csv_data()
	# Only needs to be created once
	create_lookup_texture()
	create_color_map_texture()
	update_viewports()
		

func _physics_process(delta: float) -> void:
	if Input.is_action_just_pressed("country_select"):
		is_getting_country = true
		province_selector.select_province()
	elif Input.is_action_just_pressed("select"):
		province_selector.select_province()

func create_political_map_texture():
	var rd = RenderingServer.create_local_rendering_device()
	# Create output texture format 
	const TEXTURE_SIZE = Vector2i(5632, 2048)
	var output_format = RDTextureFormat.new()
	output_format.width = TEXTURE_SIZE.x
	output_format.height = TEXTURE_SIZE.y
	output_format.format = RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM
	output_format.usage_bits = RenderingDevice.TEXTURE_USAGE_STORAGE_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT

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
	rd.sync ()
	# Get the result back
	var byte_data: PackedByteArray = rd.texture_get_data(political_image, 0)


	# Create new image from the result
	var result_image = Image.create_from_data(TEXTURE_SIZE.x, TEXTURE_SIZE.y, false, Image.FORMAT_RGBA8, byte_data)
	result_image.save_png("res://assets/political_map.png")
	political_map = result_image
	#update_material_parameters("country_map", ImageTexture.create_from_image(political_map))


func create_edge_map_texture():
	var rd = RenderingServer.create_local_rendering_device()
	# Create output texture format 
	const TEXTURE_SIZE = Vector2i(5632, 2048) / 4
	var lookup_format = RDTextureFormat.new()
	lookup_format.width = TEXTURE_SIZE.x
	lookup_format.height = TEXTURE_SIZE.y
	lookup_format.format = RenderingDevice.DATA_FORMAT_R8G8_UNORM
	lookup_format.usage_bits = RenderingDevice.TEXTURE_USAGE_STORAGE_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT

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
	downsampled_lookup.resize(TEXTURE_SIZE.x, TEXTURE_SIZE.y, Image.INTERPOLATE_BILINEAR)
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
	distance_uniform.binding = 2
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
	rd.sync ()
	# Get the result back
	var byte_data: PackedByteArray = rd.texture_get_data(edge_image, 0)


	# Create new image from the result
	var result_image = Image.create_from_data(TEXTURE_SIZE.x, TEXTURE_SIZE.y, false, Image.FORMAT_R8, byte_data)
	result_image.save_png("res://assets/edge_map.png")

func create_color_map_texture():
	var rd = RenderingServer.create_local_rendering_device()
	var start = Time.get_ticks_usec()

	
	# calls cpp code
	var buffer: PackedInt32Array = country_data.populate_color_map_buffers()
		
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
	rd.sync ()
	# Get the result back
	var byte_data: PackedByteArray = rd.texture_get_data(output_image, 0)


	# Create new image from the result
	var result_image = Image.create_from_data(TEXTURE_SIZE.x, TEXTURE_SIZE.y, false, Image.FORMAT_RGBA8, byte_data)
	color_map = result_image
	# for debugging
	result_image.save_png("res://assets/color_map.png")
	update_material_parameters("color_map", ImageTexture.create_from_image(color_map))
	
	
	return result_image
	
func create_lookup_texture():
	if not province_map:
		push_error("No province map set.")
		return
	var province_size = province_map.get_size()


	var shader = compile_shader("res://shaders/generate_color_lookup.glsl")
   
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

	var province_data = map_data.province_data
	
	var colors := PackedInt32Array()

	for i in province_data.size():
		var dict = province_data[i]
		var color: Color = dict["Color"]
		var id: int = dict["Id"]
		colors.append_array(PackedInt32Array([color.r8, color.g8, color.b8, id]))
			
	var color_bytes := colors.to_byte_array()

	
	# Create a storage buffer that can hold our values.

	var buffer_color := create_ssbo(color_bytes.size(), color_bytes)
	var uniform_color = create_uniform(buffer_color, 2, RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER)

	# Get the result back
	var byte_data: PackedByteArray = compute_result([input_uniform, output_uniform, uniform_color], output_image, shader);


	if byte_data.size() == 0:
		print("ERROR: No data retrieved from GPU texture!")
		return
	
	# Create new image from the result
	var texture_size = get_output_texture_size()
	var result_image = Image.create_from_data(texture_size.x, texture_size.y, false, Image.FORMAT_RG8, byte_data)
	color_lookup = result_image
	# for debugging
	if save_images_to_file:
		result_image.save_png("res://assets/color_lookup.png")
	update_material_parameters("lookup_map", ImageTexture.create_from_image(color_lookup))
	
	print("Texture lookup created successfully!")


func on_province_selector_province_selected(province_id: int) -> void:
	if is_getting_country == false and selected_country.is_empty() == false:
		country_data.change_province_owner(province_id, selected_country)
		update_color_map(province_id, country_data.get_country_color(selected_country))


func on_province_selector_country_selected(country: String) -> void:
	if is_getting_country:
		selected_country = country
		print("Selected country: ", selected_country)
		is_getting_country = false
