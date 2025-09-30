shader_type spatial;
render_mode unshaded;
uniform sampler2D subview_image : source_color, filter_nearest, repeat_enable; 

void vertex() {
	// Called for every vertex the material is visible on.
}
void fragment() {
	// Called for every pixel the material is visible on.
	vec2 uv = UV;  


    // Sample the color
    vec4 color = texture(subview_image, uv);

	ALBEDO = color.rgb;
	ALPHA = 1.0;

}

