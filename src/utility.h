#pragma once
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"
namespace godot::gsg
{
// Used for data that is incomplete, not defining how the non-owning provinces should be handled, this tries to assign provinces the ocean or various countries.
String get_terrain_owner(const String &filename);
PackedStringArray get_txt_files_in_folder(const String &folder_path);
// Returns the file contents modified by with the new value. The strings used should passed as a regular string, not as any kind of buffer.
PackedByteArray find_replace_in_file(const PackedByteArray &file_in_bytes,
		const String &search_key,
		const String &new_value);
String color_to_string(Color color);
PackedByteArray read_file_bytes(const String &file_path);
bool write_file_bytes(const String &file_path, const PackedByteArray &data);
} // namespace godot::gsg