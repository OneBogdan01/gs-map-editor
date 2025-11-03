shader_type canvas_item;
render_mode unshaded;

uniform sampler2D lookup_map : filter_nearest;
uniform sampler2D color_map : source_color, filter_nearest;

vec4 get_province_color(vec2 uv)
{
	vec4 lookup = texture(lookup_map, uv);
	vec2 province_id = lookup.rg;
	return texture(color_map, province_id);
}

void fragment()
{

	vec4 province_color = get_province_color(UV);

	COLOR = province_color;

}