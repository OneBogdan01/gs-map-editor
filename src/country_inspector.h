#pragma once

#include "godot_cpp/classes/editor_inspector_plugin.hpp"
namespace godot
{
class CountryData;
class CountryInspector : public EditorInspectorPlugin
{
	GDCLASS(CountryInspector, EditorInspectorPlugin);

public:
	CountryInspector() = default;
	~CountryInspector() = default;

protected:
	static void _bind_methods();
	virtual void _create_containers(CountryData *data);
	virtual void _on_parse_button_pressed(CountryData *data);
	virtual void _on_search_text_changed();
	virtual void _on_clear_search();

	CountryData *country_data{ nullptr };
};
} // namespace godot
