extends Node2D

const MASK_MATERIAL = preload("res://shaders/jfa_generation/mask_material.tres")

@onready var mask: SubViewport = $Mask/Mask

@onready var color_rect: ColorRect = $JFA/JFA/ColorRect
@onready var jfa: SubViewport = $JFA/JFA

@onready var distance: SubViewport = $Distance/Distance

@onready var distance_tex: ColorRect = $Distance/Distance/DistanceTex

@onready var outline: SubViewport = $Outline/Outline

@onready var outline_tex: ColorRect = $Outline/Outline/OutlineTex
@export var downsample:float = 4.0
var full_texture_size

var step_sizes: Array
var index_step = 0
var previous_result
func _render_result(step_size, input_texture):
	var shader_material = color_rect.material as ShaderMaterial
	
	shader_material.set_shader_parameter("input_texture", input_texture)
	shader_material.set_shader_parameter("step_size", step_size)
	jfa.set_update_mode(SubViewport.UPDATE_ONCE)
	await RenderingServer.frame_post_draw
	
	var distance_material = distance_tex.material as ShaderMaterial
	var jfa_output = jfa.get_texture().get_image()
	jfa_output.convert(Image.FORMAT_RG8)
	distance_material.set_shader_parameter("jfa_output",ImageTexture.create_from_image(jfa_output))
	distance.set_update_mode(SubViewport.UPDATE_ONCE)
	await RenderingServer.frame_post_draw
	
	var outline_material = outline_tex.material as ShaderMaterial
	var df = outline.get_texture().get_image()
	df.convert(Image.FORMAT_R8)
	distance_material.set_shader_parameter("input_texture",ImageTexture.create_from_image(df))
	distance.set_update_mode(SubViewport.UPDATE_ONCE)
	
func _increment_jfa():
		# wait until the mask has rendered
		await RenderingServer.frame_post_draw
		
		var step_size = step_sizes[index_step]
		print("Iterate JFA shader with step size:", step_size)
		var tex;
		if index_step == 0:
			tex = mask.get_texture().get_image()
			
		else:
			tex = jfa.get_texture().get_image()
			
		tex.convert(Image.FORMAT_RG8)
		tex = ImageTexture.create_from_image(tex)
		_render_result(step_size,tex)
		index_step += 1
		
func _init_jfa():
		var tex = MASK_MATERIAL.get_shader_parameter("mask_texture")
		full_texture_size = tex.get_image().get_size() /downsample
		mask.size = full_texture_size
		jfa.size =full_texture_size
		distance.size = full_texture_size
	
		step_sizes = get_jfa_step_sizes(max(full_texture_size.x,full_texture_size.y))
		index_step = 0
		# invalid step to reset image
		_render_result(0, mask.get_texture())



func _ready() -> void:
	_init_jfa()
func _input(event: InputEvent) -> void:
	if event.is_action_pressed("change_map"):
		mask.get_texture().get_image().save_png("res://assets/mask.png")
		_init_jfa()
		print("Reinit shader")
		
	if event.is_action_pressed("select"):
		if index_step < step_sizes.size():
			_increment_jfa()	
		else :
			distance.get_texture().get_image().save_png("res://assets/distance_field.png")

	

	

func get_jfa_step_sizes(max_dimension: int) -> Array[float]:
	var steps: Array[float] = []
	var num_passes = ceil(log(max_dimension) / log(2))

	for i in range(1, int(num_passes)+1, 1):
		steps.append(1.0 / pow(2.0, float(i+6)))
	
	return steps
