shader_type canvas_item;
uniform sampler2D jfa_output : filter_nearest;
uniform float max_distance : hint_range(1, 20.0) = 1.0;
void fragment() {
	vec2 color = texture(jfa_output, UV).rg;
	float dist = distance(UV, color);
	COLOR = vec4(dist/max_distance,0 , 0, 1.0);
}

