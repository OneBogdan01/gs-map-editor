shader_type canvas_item;

uniform sampler2D lookup_map : filter_nearest;
uniform sampler2D color_map : source_color, filter_nearest;

vec4 get_province_color(vec2 uv)
{
	vec4 lookup = texture(lookup_map, uv);
	vec2 province_id = lookup.rg * 255.0;
	vec2 color_map_size = vec2(textureSize(color_map, 0));
	vec2 color_uv = province_id / color_map_size;
	return texture(color_map, color_uv);
}

void fragment()
{
	vec2 uv = UV;

	COLOR = get_province_color(uv);
}