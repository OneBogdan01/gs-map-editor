shader_type canvas_item;
render_mode unshaded;

uniform sampler2D input_texture : source_color,filter_nearest;
uniform float step_size : hint_range(0.0, 16.0, 1.0) = 0.0;

void fragment() {
	float best_dist = 99999.0;
	vec2 best_uv = vec2(1.0, 1.0);
	vec2 pixel_size = 1.0 / vec2(textureSize(input_texture, 0));
	for (int y = -1; y <= 1; y++){
		for(int x = -1; x <= 1; x++){
			vec2 uv_offset = UV + vec2( float(x), float(y)) * pixel_size * step_size;

			vec2 uv_value = texture(input_texture, uv_offset).rg;

			float dist = length(uv_value - UV);
			if(uv_value.x < 1.0 && uv_value.y < 1.0 && dist < best_dist){
				best_dist = dist;
				best_uv = uv_value;
			}
		}
	}
	COLOR = vec4(best_uv, 0.0, 1.0);
}

//void light() {
//	// Called for every pixel for every light affecting the CanvasItem.
//	// Uncomment to replace the default light processing function with this one.
//}
