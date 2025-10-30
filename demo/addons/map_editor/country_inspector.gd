extends CountryInspector
const INSPECTOR_THEME = preload("res://addons/map_editor/inspector_theme.tres")


func _can_handle(object):
	var is_country_data = object is CountryData

	return is_country_data

func set_ui_theme() -> void:
	scroll_container.theme = INSPECTOR_THEME
	search_container.theme = INSPECTOR_THEME
	parse_button.theme = INSPECTOR_THEME
func _parse_begin(object: Object) -> void:
	# this allocates nodes to the custom tree structure, it will be freed automatically
	create_containers()
	set_ui_theme()
	var country_data = object as CountryData
	set_country_data(country_data)
