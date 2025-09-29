@tool
extends EditorPlugin


var is_enabled = false

func _enter_tree():
	is_enabled = !EditorInterface.is_plugin_enabled("country_parser")	


func _input(event: InputEvent) -> void:
	if event is InputEventKey and event.is_pressed():
		if event.keycode == KEY_P:
			if event.shift_pressed:
				EditorInterface.set_plugin_enabled("country_parser", is_enabled)
				is_enabled = !is_enabled
				
		
		
