#pragma once
#include "godot_cpp/classes/image.hpp"
#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/classes/rd_texture_format.hpp"
#include "godot_cpp/classes/rd_texture_view.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/sprite3d.hpp"
#include "godot_cpp/classes/texture2d.hpp"

#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/rid.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "godot_cpp/variant/vector2i.hpp"
namespace godot
{
class RenderingDevice;
class ComputeHelper : public Sprite3D
{
	GDCLASS(ComputeHelper, Sprite3D);

public:
	// Image create_lookup_texture();
	// resources
	void create_rd();
	RID compile_shader(const String &path);
	RID create_texture(const Ref<RDTextureFormat> &format, const Ref<RDTextureView> &view, const TypedArray<PackedByteArray> &data);
	RID create_ssbo(uint32_t size_bytes, const PackedByteArray &data);
	Ref<RDUniform> create_uniform(const RID &id, int32_t binding, RenderingDevice::UniformType uniform_type = RenderingDevice::UNIFORM_TYPE_IMAGE);
	Ref<RDTextureFormat> texture_format_from_texture_2d(Vector2i texture_size, RenderingDevice::DataFormat format, BitField<RenderingDevice::TextureUsageBits> bits);

	PackedByteArray compute_result(const TypedArray<Ref<RDUniform>> &uniforms, const RID &texture_id, const RID &shader);

	void clean_up();
	// Getters and setters
	void set_output_texture_size(Vector2i size)
	{
		output_texture_size = size;
		compute_group_size.x = (output_texture_size.x + 7) / 8;
		compute_group_size.y = (output_texture_size.y + 7) / 8;
	}
	Vector2i get_output_texture_size()
	{
		return output_texture_size;
	}

protected:
	static void _bind_methods();

private:
	Vector2i output_texture_size{ 5632, 2048 };
	Vector2i compute_group_size{ (5632 + 7) / 8, (2048 + 7) / 8 };
	RenderingDevice *rd{ nullptr };

	Array resources_to_delete;
};
} // namespace godot