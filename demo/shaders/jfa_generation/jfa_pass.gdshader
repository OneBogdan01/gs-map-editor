shader_type canvas_item;
render_mode unshaded;

uniform sampler2D input_texture : filter_nearest;
uniform float step_size : hint_range(0.0, 16.0, 1.0) = 0.0;

void fragment() {
	vec2 tex_size = vec2(textureSize(input_texture, 0));
    vec2 current = texture(input_texture, UV).rg;

    float best_dist_x = current.r * tex_size.x;
    float best_dist_y = current.g * tex_size.y;
	vec2 pixel_size = 1.0 / vec2(textureSize(input_texture, 0));
	for (int y = -1; y <= 1; y++){
		for(int x = -1; x <= 1; x++){
			vec2 uv_offset = vec2( float(x), float(y)) ;
			vec2 value_neighbor = texture(input_texture, uv_offset* pixel_size * step_size + UV).rg;
            if (value_neighbor.r >= 0.99 && value_neighbor.g >= 0.99) {
                continue;
            }
            
            float neighbor_dist_x = value_neighbor.r * tex_size.x;
            float neighbor_dist_y = value_neighbor.g * tex_size.y;
            
            vec2 offset_pixels = vec2(float(x), float(y)) * step_size;
            float total_dist_x = neighbor_dist_x + abs(offset_pixels.x);
            float total_dist_y = neighbor_dist_y + abs(offset_pixels.y);
            
            best_dist_x = min(best_dist_x, total_dist_x);
            best_dist_y = min(best_dist_y, total_dist_y);
	   		
		}
	}
	COLOR = vec4((best_dist_x+0.5) / tex_size.x, (best_dist_y+0.5) / tex_size.y, 0.0, 1.0);
}
