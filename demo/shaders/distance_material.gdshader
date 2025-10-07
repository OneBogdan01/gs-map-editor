// debug_jfa.gdshader
shader_type canvas_item;

uniform sampler2D jfa_result : filter_nearest;

void fragment()
{
	vec2 uv = UV;
	vec4 sample = texture(jfa_result, uv);

	vec2 seed_uv = sample.rg;

	COLOR = vec4(seed_uv.x, seed_uv.y, 0.0, 1.0);
}
