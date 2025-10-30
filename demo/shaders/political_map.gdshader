shader_type canvas_item;
render_mode unshaded;
uniform vec4 ocean_color : source_color;
const float EPSILON = 0.0001;
uniform sampler2D country_distance_field : source_color, filter_linear;
uniform sampler2D province_distance_field : source_color, filter_linear_mipmap;

uniform sampler2D lookup_map : filter_nearest;
uniform sampler2D color_map : source_color, filter_nearest;
uniform float edge_smoothness : hint_range(0.0, 2., 0.001) = 0.01;
uniform float edge_threshold : hint_range(0.0, 1.0, 0.01) = 0.5;

uniform vec4 border_color : source_color = vec4(0.0, 0.0, 0.0, 1.0);
uniform float gradient_strength : hint_range(0.0, 1.0, 0.01) = 1.0;
uniform float edge_size : hint_range(0.0, 1.0, 0.001) = 0.1;
uniform float province_size_size : hint_range(0.0, 1.0, 0.00001) = 0.1;
uniform vec4 province_border_color : source_color = vec4(0.0, 0.0, 0.0, 0.80);
bool colors_equal(vec4 a, vec4 b)
{
	return all(lessThan(abs(a - b), vec4(EPSILON)));
}
vec4 get_province_color(vec2 uv)
{
	vec4 lookup = texture(lookup_map, uv);
	vec2 province_id = lookup.rg;
	return texture(color_map, province_id);
}

void fragment()
{
	vec2 uv = UV;
	float df = texture(country_distance_field, UV).r;
	float df_province = texture(province_distance_field, UV).r;

	vec4 province_color = get_province_color(uv);

	float gradient_factor = smoothstep(edge_threshold - edge_smoothness,
			edge_threshold + edge_smoothness,
			df);


	vec3 final_color = mix(border_color.rgb, province_color.rgb, gradient_factor);

	final_color = mix(province_color.rgb, final_color, gradient_strength);
	if (df < edge_size)
	{
		final_color = border_color.rgb;
	}
	vec4 output = vec4(final_color.rgb, province_color.a);
	if (df_province < province_size_size)
	{
		output = mix(output, province_border_color, province_border_color.a);
	}
	if(colors_equal(province_color, ocean_color)){
			output = ocean_color;
		}
	COLOR = output;

}