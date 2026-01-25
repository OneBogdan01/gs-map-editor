@tool
extends Node2D
class_name JFAGenerator

signal generation_complete

@export var jfa_pass_shader: Shader

var tex_size: Vector2i
var starting_stride: int
var current_stride: int
var current_step: int = 0
var total_steps: int

func initialize(size: Vector2i, stride: int = -1) -> void:
	tex_size = size
	
	if stride > 0:
		starting_stride = stride
	else:
		starting_stride = maxi(size.x, size.y) / 2
	
	current_stride = starting_stride
	current_step = 0
	total_steps = int(log(starting_stride) / log(2)) + 1
	
	print("[JFA] Texture size: %s" % tex_size)
	print("[JFA] Starting stride: %s" % starting_stride)
	print("[JFA] Total passes: %s" % total_steps)

func clear() -> void:
	for child in get_children():
		child.queue_free()
	await get_tree().process_frame

func run_all_passes() -> void:
	for i in total_steps:
		await run_single_pass()
	generation_complete.emit()

func run_single_pass() -> void:
	await RenderingServer.frame_post_draw
	
	var bbc := BackBufferCopy.new()
	bbc.copy_mode = BackBufferCopy.COPY_MODE_VIEWPORT
	add_child(bbc)
	
	var rect := create_color_rect(create_jfa_material(current_stride))
	add_child(rect)
	
	print("[JFA] Pass %d/%d - stride: %d" % [current_step + 1, total_steps, current_stride])
	
	current_step += 1
	current_stride = maxi(1, current_stride / 2)
	
	await RenderingServer.frame_post_draw

func create_color_rect(shader_mat: ShaderMaterial) -> ColorRect:
	var rect := ColorRect.new()
	rect.material = shader_mat
	rect.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	rect.set_anchors_preset(Control.PRESET_FULL_RECT)
	return rect

func create_jfa_material(stride: int) -> ShaderMaterial:
	var mat := ShaderMaterial.new()
	mat.shader = jfa_pass_shader
	mat.set_shader_parameter("tex_size", Vector2(tex_size))
	mat.set_shader_parameter("stride", float(stride))
	return mat
