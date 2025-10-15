extends CountryInspector
const INSPECTOR_THEME = preload("res://assets/inspector_theme.tres")

func _can_handle(object):
	return object is CountryData

func _parse_begin(object: Object) -> void:
	var country_data = object as CountryData
	create_containers(country_data)

	var parse_button = Button.new()
	parse_button.text = "Parse All Files"
	parse_button.pressed.connect(on_parse_button_pressed.bind(country_data))
	add_custom_control(parse_button)
	

	var search_container = HBoxContainer.new()
	
	var search_label = Label.new()
	search_label.text = "Filter:"
	search_label.custom_minimum_size.x = 50
	search_container.add_child(search_label)
	
	var search_line_edit = get_search_line_edit()
	search_line_edit.placeholder_text = "Search entities or provinces..."
	search_line_edit.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	search_line_edit.text_changed.connect(on_search_text_changed)
	search_container.add_child(search_line_edit)
	
	var clear_button = Button.new()
	clear_button.text = "Clear"
	clear_button.pressed.connect(on_clear_search)
	search_container.add_child(clear_button)
	
	add_custom_control(search_container)
	
	# Data display container with size constraints
	var scroll_container = ScrollContainer.new()
	scroll_container.set_custom_minimum_size(Vector2i(200, 600))
	scroll_container.size_flags_vertical = Control.SIZE_EXPAND_FILL
	
	var data_container = get_data_container()
	data_container.name = "ParsedDataContainer"
	data_container.custom_minimum_size = Vector2(400, 0)
	data_container.size_flags_horizontal = Control.SIZE_EXPAND_FILL

	
	scroll_container.add_child(data_container)
	update_display("")
	add_custom_control(scroll_container)
	scroll_container.theme = INSPECTOR_THEME
	search_container.theme = INSPECTOR_THEME
	parse_button.theme = INSPECTOR_THEME
