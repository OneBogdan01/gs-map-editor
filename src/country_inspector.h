#pragma once

#include "godot_cpp/classes/editor_inspector_plugin.hpp"
#include "godot_cpp/classes/line_edit.hpp"
#include "godot_cpp/classes/v_box_container.hpp"
#include "godot_cpp/variant/string.hpp"
namespace godot
{
class CountryData;
class CountryInspector : public EditorInspectorPlugin
{
	GDCLASS(CountryInspector, EditorInspectorPlugin);

public:
	CountryInspector() = default;
	~CountryInspector() = default;
	// call on parse begin
	void create_containers(CountryData *data);
	void on_parse_button_pressed(CountryData *data);
	void on_search_text_changed(const String &search_term);
	void on_clear_search();
	void update_display(const String &search_term);
	// Getters and setters
	LineEdit *get_search_line_edit();
	void set_search_line_edit(LineEdit *value);
	VBoxContainer *get_data_container();
	void set_data_container(VBoxContainer *value);
	CountryData *get_country_data();

protected:
	static void _bind_methods();

	CountryData *country_data{ nullptr };
	LineEdit *search_line_edit{ nullptr };
	VBoxContainer *data_container{ nullptr };
};
} // namespace godot
