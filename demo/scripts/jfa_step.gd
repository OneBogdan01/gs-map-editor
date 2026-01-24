@tool
extends Node2D

@export var source_texture: Texture2D:
	set(value):
		source_texture = value
		if Engine.is_editor_hint() and is_node_ready():
			_update_editor_preview()

@export var lookup_map: Texture2D
@export var max_distance: float = 63.0

@export_group("Shaders")
@export_file("*.gdshader") var jfa_seed_shader_path := "res://shaders/jfa/jfa_seed.gdshader"
@export_file("*.gdshader") var jfa_pass_shader_path := "res://shaders/jfa/jfa_pass.gdshader"
@export_file("*.gdshader") var jfa_final_shader_path := "res://shaders/jfa/jfa_final.gdshader"

@export_group("Editor Controls")
@export var run_all_passes: bool = false:
	set(value):
		if value and Engine.is_editor_hint():
			_run_all_passes()

@export var reset_preview: bool = false:
	set(value):
		if value and Engine.is_editor_hint():
			reset()

var jfa_seed_shader: Shader
var jfa_pass_shader: Shader
var jfa_final_shader: Shader

@onready var sub_viewport: SubViewport = $SubViewport
@onready var final_viewport: SubViewport = $FinalViewport

var tex_size: Vector2
var starting_stride: int
var current_stride: int
var current_step: int = 0
var total_steps: int
var is_complete: bool = false

enum PassType { SOURCE, SEED, JFA, FINAL }
var pass_names: Array[String] = []

func _ready() -> void:
	if not _load_shaders():
		return
	
	if not source_texture:
		printerr("[JFA] No source texture assigned!")
		return
	
	tex_size = Vector2(source_texture.get_width(), source_texture.get_height())
	starting_stride = calculate_starting_stride(max_distance)
	total_steps = 2 + int(log(starting_stride) / log(2)) + 1 + 1
	
	print("[JFA] Initialized:")
	print("  Texture size: %s" % tex_size)
	print("  Max distance: %s" % max_distance)
	print("  Starting stride: %s" % starting_stride)
	print("  Total steps: %s" % total_steps)
	
	setup_viewports()
	build_pass_names()
	
	print("[JFA] Pass sequence: %s" % [pass_names])
	
	reset()

func _load_shaders() -> bool:
	jfa_seed_shader = load(jfa_seed_shader_path) as Shader
	jfa_pass_shader = load(jfa_pass_shader_path) as Shader
	jfa_final_shader = load(jfa_final_shader_path) as Shader
	
	if not jfa_seed_shader:
		printerr("[JFA] Failed to load seed shader: %s" % jfa_seed_shader_path)
		return false
	if not jfa_pass_shader:
		printerr("[JFA] Failed to load pass shader: %s" % jfa_pass_shader_path)
		return false
	if not jfa_final_shader:
		printerr("[JFA] Failed to load final shader: %s" % jfa_final_shader_path)
		return false
	
	print("[JFA] Shaders loaded successfully")
	return true

func _update_editor_preview() -> void:
	if not source_texture:
		return
	print("[JFA Editor] Updating preview...")
	_run_all_passes()

func _run_all_passes() -> void:
	if not is_node_ready():
		await ready
	
	if not _load_shaders():
		return
	
	if not source_texture:
		printerr("[JFA] No source texture!")
		return
	
	tex_size = Vector2(source_texture.get_width(), source_texture.get_height())
	starting_stride = calculate_starting_stride(max_distance)
	total_steps = 2 + int(log(starting_stride) / log(2)) + 1 + 1
	build_pass_names()
	setup_viewports()
	
	reset()
	await get_tree().process_frame
	
	print("[JFA] Running all passes...")
	while not is_complete:
		step_forward()
		await get_tree().process_frame
	
	print("[JFA] All passes complete!")

func setup_viewports() -> void:
	sub_viewport.size = Vector2i(tex_size)
	final_viewport.size = Vector2i(tex_size)
	print("[JFA] Viewports sized to %s" % Vector2i(tex_size))

func build_pass_names() -> void:
	pass_names.clear()
	pass_names.append("Source")
	pass_names.append("Seed")
	
	var stride := starting_stride
	while stride >= 1:
		pass_names.append("JFA (stride %d)" % stride)
		stride /= 2
	
	pass_names.append("Final")

func _input(event: InputEvent) -> void:
	if Engine.is_editor_hint():
		return
	
	if event is InputEventKey and event.pressed:
		match event.keycode:
			KEY_SPACE:
				step_forward()
			KEY_R:
				reset()

func reset() -> void:
	print("[JFA] Resetting...")
	
	for child in sub_viewport.get_children():
		child.queue_free()
	for child in final_viewport.get_children():
		child.queue_free()
	
	await get_tree().process_frame
	
	current_step = 0
	current_stride = starting_stride
	is_complete = false
	
	print("[JFA] Reset complete. Ready for step 0.")

func step_forward() -> void:
	if is_complete:
		print("[JFA] Already complete, ignoring step.")
		return
	
	var pass_type := get_pass_type(current_step)
	var pass_name := pass_names[current_step] if current_step < pass_names.size() else "Unknown"
	
	print("[JFA] Step %d/%d: %s" % [current_step, total_steps - 1, pass_name])
	
	match pass_type:
		PassType.SOURCE:
			add_source()
		PassType.SEED:
			add_pass(create_seed_material())
		PassType.JFA:
			print("[JFA]   Stride: %d" % current_stride)
			add_pass(create_jfa_material(current_stride))
			current_stride /= 2
		PassType.FINAL:
			add_final_pass()
			is_complete = true
			print("[JFA] === COMPLETE ===")
	
	current_step += 1

func get_pass_type(step: int) -> PassType:
	if step == 0:
		return PassType.SOURCE
	elif step == 1:
		return PassType.SEED
	elif step == total_steps - 1:
		return PassType.FINAL
	else:
		return PassType.JFA

func add_source() -> void:
	var source_rect := TextureRect.new()
	source_rect.texture = source_texture
	source_rect.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	source_rect.set_anchors_preset(Control.PRESET_FULL_RECT)
	sub_viewport.add_child(source_rect)

func add_pass(shader_mat: ShaderMaterial) -> void:
	var bbc := BackBufferCopy.new()
	bbc.copy_mode = BackBufferCopy.COPY_MODE_VIEWPORT
	sub_viewport.add_child(bbc)
	
	var rect := create_color_rect(shader_mat)
	sub_viewport.add_child(rect)

func add_final_pass() -> void:
	add_pass(create_final_material())
	
	await get_tree().process_frame
	
	var final_display := TextureRect.new()
	final_display.texture = sub_viewport.get_texture()
	final_display.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	final_display.set_anchors_preset(Control.PRESET_FULL_RECT)
	final_viewport.add_child(final_display)

func calculate_starting_stride(dist: float) -> int:
	var stride := 1
	while stride < int(dist + 1.0) / 2:
		stride *= 2
	return stride

func create_color_rect(shader_mat: ShaderMaterial) -> ColorRect:
	var rect := ColorRect.new()
	rect.material = shader_mat
	rect.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	rect.set_anchors_preset(Control.PRESET_FULL_RECT)
	return rect

func create_seed_material() -> ShaderMaterial:
	var mat := ShaderMaterial.new()
	mat.shader = jfa_seed_shader
	mat.set_shader_parameter("tex_size", tex_size)
	mat.set_shader_parameter("lookup_map", lookup_map)
	return mat

func create_jfa_material(stride: int) -> ShaderMaterial:
	var mat := ShaderMaterial.new()
	mat.shader = jfa_pass_shader
	mat.set_shader_parameter("tex_size", tex_size)
	mat.set_shader_parameter("stride", float(stride))
	return mat

func create_final_material() -> ShaderMaterial:
	var mat := ShaderMaterial.new()
	mat.shader = jfa_final_shader
	mat.set_shader_parameter("tex_size", tex_size)
	mat.set_shader_parameter("max_distance", max_distance)
	return mat
