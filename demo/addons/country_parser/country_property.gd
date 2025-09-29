extends EditorProperty

var file_dialog: FileDialog
var line_edit: LineEdit
var browse_button: Button

func _init():
	line_edit = LineEdit.new()
	browse_button = Button.new()
	browse_button.text = "..."
	browse_button.custom_minimum_size.x = 30
	
	var hbox = HBoxContainer.new()
	hbox.add_child(line_edit)
	hbox.add_child(browse_button)
	add_child(hbox)
	
	# Create file dialog
	file_dialog = FileDialog.new()
	file_dialog.file_mode = FileDialog.FILE_MODE_OPEN_DIR  # For folder selection
	add_child(file_dialog)
	
	# Connect signals
	line_edit.text_changed.connect(_on_text_changed)
	browse_button.pressed.connect(_on_browse_pressed)
	file_dialog.dir_selected.connect(_on_folder_selected)

func _update_property():
	line_edit.text = get_edited_object()[get_edited_property()]

func _on_text_changed(new_text: String):
	emit_changed(get_edited_property(), new_text)

func _on_browse_pressed():
	file_dialog.popup_centered(Vector2i(800, 600))

func _on_folder_selected(path: String):
	line_edit.text = path
	emit_changed(get_edited_property(), path)
