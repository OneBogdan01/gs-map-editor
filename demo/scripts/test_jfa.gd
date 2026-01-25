@tool
extends Node2D

@export var auto_start: bool = false
@export var step_delay: float = 0.1

@export_group("Editor Controls")
@export var run_jfa: bool = false:
	set(v):
		if v and Engine.is_editor_hint():
			start()

@export var reset_jfa: bool = false:
	set(v):
		if v and Engine.is_editor_hint():
			reset()

@export var step_jfa: bool = false:
	set(v):
		if v and Engine.is_editor_hint():
			step()

@export var run_stepped_jfa: bool = false:
	set(v):
		if v and Engine.is_editor_hint():
			run_stepped()

@export_group("Debug Info")
@export var current_step: int = 0
@export var total_steps: int = 0
@export var current_stride: int = 0

var country_distance_field: CountryDistanceField
var is_running: bool = false

func _ready() -> void:
	_update_reference()
	
	if not Engine.is_editor_hint() and auto_start:
		start()

func _update_reference() -> void:
	if not is_inside_tree():
		return
	var node := get_node_or_null("SubViewportContainer/CountryDistanceField")
	if node:
		country_distance_field = node as CountryDistanceField

func _update_debug_info() -> void:
	if not country_distance_field:
		return
	var jfa := country_distance_field.jfa_generator
	if jfa:
		current_step = jfa.current_step
		total_steps = jfa.total_steps
		current_stride = jfa.current_stride

func _unhandled_input(event: InputEvent) -> void:
	if Engine.is_editor_hint():
		return
	
	if event.is_action_pressed("ui_accept"):
		start()
	elif event.is_action_pressed("ui_cancel"):
		reset()
	elif event.is_action_pressed("ui_right"):
		step()
	elif event.is_action_pressed("ui_up"):
		add_iterations(1)
	elif event.is_action_pressed("ui_down"):
		run_stepped()

func start() -> void:
	_update_reference()
	if not country_distance_field:
		push_error("[Controller] CountryDistanceField not found")
		return
	if is_running:
		print("[Controller] Already running")
		return
	
	is_running = true
	print("[Controller] Starting JFA generation...")
	await country_distance_field.generate_sdf()
	is_running = false
	_update_debug_info()
	print("[Controller] Done")

func reset() -> void:
	_update_reference()
	if not country_distance_field:
		return
	
	print("[Controller] Resetting...")
	is_running = false
	
	var jfa := country_distance_field.jfa_generator
	await jfa.clear()
	jfa.initialize(
		country_distance_field.output_size,
		country_distance_field.starting_stride
	)
	_update_debug_info()
	print("[Controller] Reset complete")

func step() -> void:
	_update_reference()
	if not country_distance_field or is_running:
		return
	
	var jfa := country_distance_field.jfa_generator
	
	if jfa.current_step >= jfa.total_steps:
		print("[Controller] All steps complete")
		return
	
	is_running = true
	await jfa.run_single_pass()
	is_running = false
	_update_debug_info()

func add_iterations(count: int) -> void:
	_update_reference()
	if not country_distance_field or is_running:
		return
	
	var jfa := country_distance_field.jfa_generator
	
	is_running = true
	for i in count:
		if jfa.current_step >= jfa.total_steps:
			print("[Controller] No more steps")
			break
		await jfa.run_single_pass()
		_update_debug_info()
	is_running = false

func run_stepped() -> void:
	_update_reference()
	if not country_distance_field or is_running:
		return
	
	var jfa := country_distance_field.jfa_generator
	
	is_running = true
	while jfa.current_step < jfa.total_steps:
		await jfa.run_single_pass()
		_update_debug_info()
		await get_tree().create_timer(step_delay).timeout
	is_running = false
	print("[Controller] Stepped run complete")
