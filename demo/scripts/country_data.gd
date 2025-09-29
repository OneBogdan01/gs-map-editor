@tool
extends CountryData


func _process(delta):
	pass

	# Code to execute both in editor and in game.

func _on_button_button_down() -> void:
	parse_all_files()
