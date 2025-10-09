#pragma once
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
namespace godot::gsg
{
//used for data that is incomplete, not defining how the non-owning provinces should be handled, this tries to assign provinces the ocean or various countries.
//used AI to try to get as many provinces as possible, this should be disabled if the data used is correct.
String get_terrain_owner(const String &filename);
} //namespace godot::gsg