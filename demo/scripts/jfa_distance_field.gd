@tool
extends SubViewport
class_name CountryDistanceField
@export var output_size: Vector2i = Vector2i(512, 512)
@export var starting_stride: int = 16

@onready var seed_rect: ColorRect = $Seed
@onready var jfa_generator: JFAGenerator = $JFAGenerator

func _ready() -> void:
	size = output_size
	generate_sdf()

func generate_sdf() -> void:
	jfa_generator.generation_complete.connect(_on_jfa_complete)
	jfa_generator.initialize(output_size, starting_stride)
	await jfa_generator.run_all_passes()

func _on_jfa_complete() -> void:
	print("[CountryDistanceField] JFA generation complete!")
	# Output reads via screen_texture automatically

func generate_sdf_stepped() -> void:
	jfa_generator.initialize(output_size, starting_stride)
	
	for i in jfa_generator.total_steps:
		await jfa_generator.run_single_pass()
		print("Completed pass %d" % (i + 1))

func get_result() -> ViewportTexture:
	return get_texture()
