// border_render.gdshader
shader_type canvas_item;

uniform sampler2D distance_field : filter_linear;
uniform sampler2D lookup_map : filter_nearest;
uniform sampler2D color_map : source_color, filter_nearest;
uniform vec4 border_color : source_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform float border_width : hint_range(0.0, 20.0) = 2.0;
uniform float border_softness : hint_range(0.0, 5.0) = 1.0;

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

	// Get distance from distance field
	float dist = texture(distance_field, uv).r;

	// Convert normalized distance back to pixels
	vec2 texture_size = vec2(textureSize(distance_field, 0));
	dist *= 50.0; // Should match max_distance from distance_field shader

	// Create border mask
	float border_mask = smoothstep(border_width + border_softness,
			border_width - border_softness,
			dist);

	// Get province color
	vec4 province_color = get_province_color(uv);

	// Blend
	COLOR = mix(province_color, border_color, border_mask * border_color.a);
}