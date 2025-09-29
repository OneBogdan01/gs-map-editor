@tool
extends EditorPlugin

var plugin
# A class member to hold the dock during the plugin life cycle.

#var country_editor
var province_selector: ProvinceSelector
var map_data: MapData 

func province_click(tex: Vector2i):
	print("Texture coordinates are ", tex)


func _enter_tree():
	plugin = preload("res://addons/country_parser/country_inspector.gd").new()
	add_inspector_plugin(plugin)

			
	if !Engine.is_editor_hint():
		province_selector=$ProvinceSelector
		print("Country Editor", province_selector)
		province_selector.province_selected.connect(province_click)
		
	

func _exit_tree():
	remove_inspector_plugin(plugin)
