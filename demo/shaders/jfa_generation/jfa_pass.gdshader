shader_type canvas_item;
render_mode unshaded;

uniform sampler2D input_texture : source_color,filter_nearest;
uniform float step_size : hint_range(0.0, 16.0, 1.0) = 0.0;

void fragment() {
    vec2 current = texture(input_texture, UV).rg;
	float dist = length(current);
    float best_dist_x = current.r;
    float best_dist_y = current.g;
	vec2 pixel_size = 1.0 / vec2(textureSize(input_texture, 0));
	for (int y = -1; y <= 1; y++){
		for(int x = -1; x <= 1; x++){
			vec2 uv_offset = vec2( float(x), float(y)) * pixel_size * step_size;
			float dist_neighbor = length(uv_offset);
			vec2 value_neighbor = texture(input_texture, uv_offset + UV).rg;
			 if (value_neighbor.r >= 1.0 && value_neighbor.g >= 1.0) {
                continue;
            }
            
            
            // Add the offset distance to the neighbor's stored distance
            float total_dist_x = value_neighbor.r + abs(uv_offset.x);
            float total_dist_y = value_neighbor.g + abs(uv_offset.y);
            
            best_dist_x = min(best_dist_x, total_dist_x);
			best_dist_y = min(best_dist_y, total_dist_y);
	   		
		}
	}
	COLOR = vec4(best_dist_x, best_dist_y, 0.0, 1.0);
}
