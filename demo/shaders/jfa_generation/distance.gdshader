shader_type canvas_item;
render_mode unshaded;
uniform sampler2D jfa_output :source_color, filter_nearest;
uniform float max_dist;
void fragment() {
	vec2 color = texture(jfa_output, UV).rg;
	
	float dist = distance(UV, color);
	COLOR = vec4(dist*max_dist,0.0 , 0.0, 1.0);
}
