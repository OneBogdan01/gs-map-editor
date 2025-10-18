#include "compute_helper.hpp"
#include "godot_cpp/classes/image.hpp"
#include "godot_cpp/classes/rd_shader_file.hpp"
#include "godot_cpp/classes/rd_shader_spirv.hpp"
#include "godot_cpp/classes/rd_texture_format.hpp"
#include "godot_cpp/classes/rd_texture_view.hpp"
#include "godot_cpp/classes/rd_uniform.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/rendering_device.hpp"
#include "godot_cpp/classes/rendering_server.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/texture2d.hpp"
#include "godot_cpp/core/print_string.hpp"
#include "godot_cpp/core/type_info.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/rid.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <cstdint>
using namespace godot;

void ComputeHelper::create_rd()
{
	rd = RenderingServer::get_singleton()->create_local_rendering_device();
}
void ComputeHelper::clean_up()
{
	for (auto &resource : resources_to_delete)
	{
		rd->free_rid(resource);
	}
	resources_to_delete.clear();
}
// Image ComputeHelper::create_lookup_texture()
// {
// 	return {};
// }
RID ComputeHelper::compile_shader(const String &path)
{
	Ref<RDShaderFile> shader_file = ResourceLoader::get_singleton()->load(path);
	if (shader_file.is_null())
	{
		print_error("Failed to load shader file: ", path);
		return {};
	}
	Ref<RDShaderSPIRV> shader_spirv = shader_file->get_spirv();
	RID shader = rd->shader_create_from_spirv(shader_spirv);
	if (shader.is_valid() == false)
	{
		print_error("Failed to compiler shader file: ", path);
		return {};
	}
	// Add to the front so it works like a queue
	resources_to_delete.push_front(shader);

	return shader;
}
Ref<RDTextureFormat> ComputeHelper::texture_format_from_texture_2d(Vector2i texture_size, RenderingDevice::DataFormat format, BitField<RenderingDevice::TextureUsageBits> bits)
{
	Ref<RDTextureFormat> fm;
	fm.instantiate();

	fm->set_width(output_texture_size.x);
	fm->set_height(output_texture_size.y);
	fm->set_format(format);
	fm->set_usage_bits(bits);
	return fm;
}
RID ComputeHelper::create_texture(const Ref<RDTextureFormat> &format, const Ref<RDTextureView> &view, const TypedArray<PackedByteArray> &data)
{
	RID texture = rd->texture_create(format, view, data);
	if (!texture.is_valid())
	{
		UtilityFunctions::push_error("Failed to create texture");
		return {};
	}
	resources_to_delete.push_front(texture);
	return texture;
}
Ref<RDUniform> ComputeHelper::create_uniform(const RID &id, int32_t binding, RenderingDevice::UniformType uniform_type)
{
	Ref<RDUniform> uf;
	uf.instantiate();
	uf->add_id(id);
	uf->set_binding(binding);
	uf->set_uniform_type(uniform_type);
	return uf;
}
RID ComputeHelper::create_ssbo(uint32_t size_bytes, const PackedByteArray &data)
{
	RID ssbo = rd->storage_buffer_create(size_bytes, data);
	resources_to_delete.push_front(ssbo);
	return ssbo;
}
PackedByteArray ComputeHelper::compute_result(const TypedArray<Ref<RDUniform>> &uniforms, const RID &texture_id, const RID &shader)
{
	RID uniform_set = rd->uniform_set_create(uniforms, shader, 0);
	resources_to_delete.push_front(uniform_set);

	RID pipeline = rd->compute_pipeline_create(shader);
	resources_to_delete.push_front(pipeline);
	int64_t compute_list = rd->compute_list_begin();
	rd->compute_list_bind_compute_pipeline(compute_list, pipeline);
	rd->compute_list_bind_uniform_set(compute_list, uniform_set, 0);

	rd->compute_list_dispatch(compute_list, compute_group_size.x, compute_group_size.y, 1);
	rd->compute_list_end();
	rd->submit();
	rd->sync();
	PackedByteArray data = rd->texture_get_data(texture_id, 0);

	clean_up();
	emit_signal("texture_updated", data);
	return data;
}
void ComputeHelper::_bind_methods()
{
	// RID
	ClassDB::bind_method(D_METHOD("compile_shader"), &ComputeHelper::compile_shader);
	ClassDB::bind_method(D_METHOD("create_ssbo", "size_bytes", "data"), &ComputeHelper::create_ssbo);
	ClassDB::bind_method(D_METHOD("create_uniform", "size_bytes", "id", "binding"), &ComputeHelper::create_uniform);
	ClassDB::bind_method(D_METHOD("create_texture", "format", "view", "data"), &ComputeHelper::create_texture);
	// others

	ClassDB::bind_method(D_METHOD("create_rd"), &ComputeHelper::create_rd);
	ClassDB::bind_method(D_METHOD("texture_format_from_texture_2d", "texure_size", "format", "bits"), &ComputeHelper::texture_format_from_texture_2d);
	ClassDB::bind_method(D_METHOD("compute_result", "uniforms", "texture_id", "shader"), &ComputeHelper::compute_result);

	// Getters setters
	ClassDB::bind_method(D_METHOD("set_output_texture_size", "size"), &ComputeHelper::set_output_texture_size);
	ClassDB::bind_method(D_METHOD("get_output_texture_size"), &ComputeHelper::get_output_texture_size);
	ClassDB::bind_method(D_METHOD("clean_up"), &ComputeHelper::clean_up);

	// Signals
	ADD_SIGNAL(MethodInfo("texture_updated", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "texture_data")));
}
