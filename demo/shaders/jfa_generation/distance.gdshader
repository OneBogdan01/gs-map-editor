shader_type canvas_item;
render_mode unshaded;
uniform sampler2D jfa_output :source_color, filter_linear;
void fragment() {
	vec2 color = texture(jfa_output, UV).rg;
	float dist = distance(UV, color);
	COLOR = vec4(dist,0 , 0, 1.0);
}

