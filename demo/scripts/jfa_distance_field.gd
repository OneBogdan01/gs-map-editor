extends SubViewport
class_name CountryDistanceField
@export var output_size: Vector2i = Vector2i(512, 512)
@export var starting_stride: int = 16

@onready var seed_rect: ColorRect = $Seed
@onready var jfa_generator: JFAGenerator = $JFAGenerator

func _ready() -> void:
	size = output_size
	jfa_generator.generation_complete.connect(_on_jfa_complete)
	initialize()

func generate_sdf() -> void:
	jfa_generator.initialize(output_size, starting_stride)
	await jfa_generator.run_all_passes()

func _on_jfa_complete() -> void:
	print("[CountryDistanceField] JFA generation complete!")
	# Output reads via screen_texture automatically

func initialize() -> void:
	jfa_generator.initialize(output_size, starting_stride)

func step() -> bool:
	"""Runs a single JFA pass. Returns true if more steps remain."""
	if is_complete():
		return false
	await jfa_generator.run_single_pass()
	return not is_complete()

func is_complete() -> bool:
	return jfa_generator.current_step >= jfa_generator.total_steps
func get_result() -> ViewportTexture:
	return get_texture()
