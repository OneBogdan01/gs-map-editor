#include "register_types.hpp"

#include "camera_controller.hpp"
#include "compute_helper.hpp"
#include "country_data.hpp"
#include "country_inspector.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "map_data.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_example_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		GDREGISTER_VIRTUAL_CLASS(CountryInspector);
		return;
	}
	else if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		return;
	}
	// used for province selecting
	GDREGISTER_CLASS(MapData);
	GDREGISTER_CLASS(CountryData);

	GDREGISTER_CLASS(ComputeHelper);
	// can be done in gd
	GDREGISTER_RUNTIME_CLASS(CameraController);
}

void uninitialize_example_module(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		return;
	}
}

extern "C"
{
	// Initialization.
	GDExtensionBool GDE_EXPORT map_editor_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

		init_obj.register_initializer(initialize_example_module);
		init_obj.register_terminator(uninitialize_example_module);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}