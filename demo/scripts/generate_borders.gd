extends Node2D
# Viewport references
@onready var seed_viewport = $Seed
@onready var jfa_viewport_a = $JFABufferA
@onready var jfa_viewport_b = $JFABufferB
@onready var distance_viewport = $Distance
@onready var final_viewport = $Borders
var full_texture_size = Vector2i(5632, 2048)
var downsampled_size = Vector2i(5632, 2048)   # JFA works at 1/4 resolution

func _ready():
	setup_viewports()
	generate_borders()

func setup_viewports():
	# Configure all viewports to the same size
	for vp in [seed_viewport, jfa_viewport_a, jfa_viewport_b, distance_viewport, final_viewport]:
		vp.size = downsampled_size
		vp.render_target_clear_mode = SubViewport.CLEAR_MODE_ONCE
		vp.render_target_update_mode = SubViewport.UPDATE_DISABLED
		
		# Each viewport needs a ColorRect to render the shader
		var rect = ColorRect.new()
		rect.name = "ColorRect"
		rect.size = downsampled_size
		vp.add_child(rect)
	
	final_viewport.size = full_texture_size
	final_viewport.render_target_clear_mode = SubViewport.CLEAR_MODE_ONCE
	final_viewport.render_target_update_mode = SubViewport.UPDATE_DISABLED
	# Set up materials
	$Seed/ColorRect.material = load("res://shaders/seed_material.tres")
	$JFABufferA/ColorRect.material = load("res://shaders/jfa_material.tres")
	$JFABufferB/ColorRect.material = load("res://shaders/jfa_material.tres").duplicate()
	$Distance/ColorRect.material = load("res://shaders/distance_material.tres")
	$Borders/ColorRect.material = load("res://shaders/border_material.tres")

func generate_borders():
	# Step 1: Generate seed texture
	var seed_mat = $Seed/ColorRect.material as ShaderMaterial
	var look_up = preload("res://assets/color_lookup.png")
	seed_mat.set_shader_parameter("lookup_map", look_up)
	seed_mat.set_shader_parameter("color_map", preload("res://assets/color_map.png"))
	
	seed_viewport.render_target_update_mode = SubViewport.UPDATE_ONCE
	await RenderingServer.frame_post_draw
	
	# Step 2: JFA iterations with ping-pong
	var step_sizes = get_jfa_step_sizes(max(full_texture_size.x, full_texture_size.y))
	var input_texture = seed_viewport.get_texture()
	var use_a = true  # Start by writing to A
	
	print("Starting JFA with step sizes: ", step_sizes)
	
	for step_size in step_sizes:
		var read_vp = jfa_viewport_b if use_a else jfa_viewport_a
		var write_vp = jfa_viewport_a if use_a else jfa_viewport_b
		var write_mat = write_vp.get_child(0).material as ShaderMaterial
		
		# Set input texture and step size (pass actual pixel jump distance)
		write_mat.set_shader_parameter("input_texture", input_texture)
		write_mat.set_shader_parameter("step_size", float(step_size))
		
		print("JFA pass with step_size: ", step_size)
		
		# Render this iteration
		write_vp.render_target_update_mode = SubViewport.UPDATE_ONCE
		await RenderingServer.frame_post_draw
		#await get_tree().create_timer(0.5).timeout  # Wait to see each pass
		
		# Output becomes input for next iteration
		input_texture = write_vp.get_texture()
		use_a = !use_a  # Flip the buffer
	
	# Step 3: Generate distance field from final JFA result
	var dist_mat = $Distance/ColorRect.material as ShaderMaterial
	dist_mat.set_shader_parameter("jfa_result", input_texture)
	
	distance_viewport.render_target_update_mode = SubViewport.UPDATE_ONCE
	await RenderingServer.frame_post_draw
	
	# Step 4: Render final borders
	var final_mat = $Borders/ColorRect.material as ShaderMaterial
	final_mat.set_shader_parameter("distance_field", distance_viewport.get_texture())
	final_mat.set_shader_parameter("lookup_map", preload("res://assets/color_lookup.png"))
	final_mat.set_shader_parameter("color_map", preload("res://assets/color_map.png"))
	
	final_viewport.render_target_update_mode = SubViewport.UPDATE_ONCE
	await RenderingServer.frame_post_draw
	
	# Use the final result
	var border_texture = final_viewport.get_texture()
	# Apply to your map sprite or wherever you need it

func get_jfa_step_sizes(max_dimension: int) -> Array[float]:
	var steps: Array[float] = []
	var num_passes = ceil(log(max_dimension) / log(2))
	
	# Generate proper JFA sequence: start at half texture size, halve each time
	for i in range(int(num_passes) - 1, -1, -1):
		steps.append(pow(2.0, float(i)))
	
	return steps
