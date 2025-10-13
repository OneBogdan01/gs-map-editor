@tool
extends EditorPlugin

var plugin

func _enter_tree():
	plugin = preload("res://addons/country_parser/country_inspector.gd").new()
	add_inspector_plugin(plugin)

		
	

func _exit_tree():
	remove_inspector_plugin(plugin)
