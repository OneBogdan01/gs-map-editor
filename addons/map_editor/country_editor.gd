@tool
extends EditorPlugin

var plugin: CountryInspector

func _enter_tree():
	plugin = preload("res://addons/map_editor/country_inspector.gd").new()

	add_inspector_plugin(plugin)

		
func _exit_tree():
	remove_inspector_plugin(plugin)
