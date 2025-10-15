extends CountryInspector

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

# Add this function to your class:

func _on_country_color_changed(new_color: Color, country_data: CountryData, country_name: String):
	# Update the color in C++ using the bound method
	var success = country_data.set_country_color_by_name(country_name, new_color)
	if success == false:
		print("Failed to update ", country_name)
	else:
		var id: int = country_data.get_country_id_from_name(country_name)
		country_data.export_color_data(id)

#func _update_data_display(container: VBoxContainer, search_term: String = ""):
	## delete all 
	#for child in container.get_children():
		#child.queue_free()
	#
	#var province_data = current_country_data.get_province_data()
	#var country_data_array = current_country_data.get_country_data()
	#var color_data = current_country_data.get_country_color_data()
	#
	#if province_data.is_empty():
		#var no_data_label = Label.new()
		#no_data_label.text = "No data parsed, click 'Parse All Files' to load data."
		#no_data_label.add_theme_color_override("font_color", Color.RED)
		#container.add_child(no_data_label)
		#return
	#
	## Filter countries based on search term
	#var filtered_countries = []
	#var total_provinces_shown = 0
	#
	#for i in range(country_data_array.size()):
		#var country = country_data_array[i]
		#var country_name = country.get("Name", "Unknown").to_lower()
		#var country_id = country.get("Id", "").to_lower()
		#var provinces = current_country_data.get_country_provinces(i)
		#
		#var country_matches = false
		#var filtered_provinces = []
		#
		## Check if country name or Id matches search
		#if search_term.is_empty() or country_name.contains(search_term) or country_id.contains(search_term):
			#country_matches = true
			#filtered_provinces = provinces
		#else:
			## Check if any province matches search
			#for province_text in provinces:
				#if province_text.to_lower().contains(search_term):
					#filtered_provinces.append(province_text)
			#
			#if not filtered_provinces.is_empty():
				#country_matches = true
		#
		#if country_matches:
			#filtered_countries.append({
				#"index": i,
				#"country": country,
				#"provinces": filtered_provinces
			#})
			#total_provinces_shown += filtered_provinces.size()
	#
	## Summary stats
	#var stats_text = ""
	#if search_term.is_empty():
		#stats_text = "Countries: %d | Provinces: %d" % [country_data_array.size(), province_data.size()]
	#else:
		#stats_text = "Found: %d countries | %d provinces (filtered by: \"%s\")" % [filtered_countries.size(), total_provinces_shown, search_term]
	#
	#var stats_label = Label.new()
	#stats_label.text = stats_text
	#stats_label.add_theme_color_override("font_color", Color.CYAN)
	#container.add_child(stats_label)
	#
	## Show "no results" message if nothing found
	#if filtered_countries.is_empty() and not search_term.is_empty():
		#var no_results_label = Label.new()
		#no_results_label.text = "No countries or provinces match your search."
		#no_results_label.add_theme_color_override("font_color", Color.YELLOW)
		#container.add_child(no_results_label)
		#return
	#
	## Add separator
	#var separator = HSeparator.new()
	#container.add_child(separator)
	#
	#
	## Create UI for each filtered country
	#for j in range(filtered_countries.size()):
		#var filtered_country = filtered_countries[j]
		#var country = filtered_country.country
		#var provinces = filtered_country.provinces
		#var country_name = country.get("Name", "Unknown")
		#var country_id = country.get("Id", "")
		#
		## Country container
		#var country_container = VBoxContainer.new()
		#
		#var country_button = Button.new()
		#var country_text = "%s (%s) Province Count:%d" % [country_name, country_id, provinces.size()]
		#
		## Get country color 
		#var country_color = current_country_data.get_country_color(country_name)
#
#
		## Highlight search term in country name if it matches
		#if not search_term.is_empty() and (country_name.to_lower().contains(search_term) or country_id.to_lower().contains(search_term)):
			#country_text += " [MATCH]"
		#
#
		## Create a container that allows absolute positioning
		#var button_container = Control.new()
		#button_container.custom_minimum_size = Vector2(400, 32)
		#button_container.size_flags_horizontal = Control.SIZE_EXPAND_FILL
#
		## Country button
		#country_button.text = "▼ " + country_text
		#country_button.alignment = HORIZONTAL_ALIGNMENT_LEFT
		#country_button.toggle_mode = provinces.size() > 0
		#country_button.disabled = provinces.size() == 0
		#country_button.button_pressed = false
		## country_button.add_theme_font_size_override("font_size", 16)
		#country_button.anchor_right = 0.85 # Take up 85% of width
		#country_button.anchor_bottom = 1.0
		#button_container.add_child(country_button)
#
		## Color picker button
		#var color_picker = ColorPickerButton.new()
		#color_picker.color = country_color
		#color_picker.custom_minimum_size = Vector2(32, 32)
		#color_picker.anchor_left = 0.85 # Start where country button ends
		#color_picker.anchor_right = 1.0 # Take remaining space
		#color_picker.anchor_bottom = 1.0
		#color_picker.edit_alpha = false # Disable alpha editing if you don't need it
#
		## TODO
		#color_picker.color_changed.connect(_on_country_color_changed.bind(current_country_data, country_name))
#
		#button_container.add_child(color_picker)
		#country_container.add_child(button_container)
		## Provinces container with proper scrolling
		#var provinces_scroll = ScrollContainer.new()
		#provinces_scroll.visible = false
		#
		#provinces_scroll.custom_minimum_size = Vector2(300, 150)
		#provinces_scroll.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		#provinces_scroll.size_flags_vertical = Control.SIZE_SHRINK_CENTER
		#
		#var calculated_height = min(250, provinces.size() * 35 + 20)
		#provinces_scroll.custom_minimum_size.y = calculated_height
		#
		#provinces_scroll.horizontal_scroll_mode = ScrollContainer.SCROLL_MODE_AUTO
		#provinces_scroll.vertical_scroll_mode = ScrollContainer.SCROLL_MODE_AUTO
		#
		#var provinces_container = VBoxContainer.new()
		#provinces_container.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		#provinces_container.size_flags_vertical = Control.SIZE_SHRINK_CENTER
		#provinces_scroll.add_child(provinces_container)
		#country_container.add_child(provinces_scroll)
		#
		## Toggle visibility
		#country_button.toggled.connect(_on_country_toggled.bind(provinces_scroll, country_button))
		#
		## Add filtered provinces
		#for province_text in provinces:
			#var province_label = Label.new()
			#var display_text = province_text
			#
			## Highlight search term in province text if it matches
			#if not search_term.is_empty() and province_text.to_lower().contains(search_term):
				#display_text += " [MATCH]"
			#
			#province_label.text = display_text
			#province_label.add_theme_color_override("font_color", Color.LIGHT_GRAY)
			## province_label.add_theme_font_size_override("font_size", 20)
			#province_label.size_flags_horizontal = Control.SIZE_EXPAND_FILL
			#province_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
			#province_label.custom_minimum_size.y = 30
			#provinces_container.add_child(province_label)
		#
		#container.add_child(country_container)
		#
		## Add separator between countries
		#if j < filtered_countries.size() - 1:
			#var mini_sep = HSeparator.new()
			#mini_sep.modulate = Color(1, 1, 1, 0.3)
			#container.add_child(mini_sep)

func _on_country_toggled(pressed: bool, scroll_container: ScrollContainer, button: Button):
	scroll_container.visible = pressed
	if pressed:
		button.text = button.text.replace("▼", "▲")
		scroll_container.call_deferred("queue_redraw")
		scroll_container.get_v_scroll_bar().call_deferred("set_value", 0)
	else:
		button.text = button.text.replace("▲", "▼")

func _get_country_name_by_id(country_data: CountryData, country_id: String) -> String:
	var countries = country_data.get_country_data()
	
	for country in countries:
		if country.get("Id", "") == country_id:
			return country.get("Name", "Unknown")
	
	return "Unknown"

func _find_containers_by_name(node: Node, name: String) -> Array:
	var found = []
	if node.name == name:
		found.append(node)
	
	for child in node.get_children():
		found.append_array(_find_containers_by_name(child, name))
	
	return found
