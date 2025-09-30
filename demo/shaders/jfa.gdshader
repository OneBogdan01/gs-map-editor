shader_type canvas_item;

uniform sampler2D input_texture : filter_nearest;
uniform float step_size = 1.0; 

void fragment() {
    vec2 uv = UV;
    vec2 texture_size = vec2(textureSize(input_texture, 0));
    vec2 pixel_size = 1.0 / texture_size;
    

    vec4 current = texture(input_texture, uv);
    vec2 best_seed_uv = current.rg;
    float best_distance = 99999.0;
    
    

    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {

            vec2 jump_offset = vec2(float(x), float(y)) * step_size * pixel_size;
            vec2 sample_uv = uv + jump_offset;
           
            
            vec4 neighbor = texture(input_texture, sample_uv);
            vec2 neighbor_seed_uv = neighbor.rg;
            
            if (neighbor_seed_uv == vec2(1.0, 1.0)) continue;
            
            float dist = length((neighbor_seed_uv - uv) * texture_size);
            
            if (dist < best_distance) {
                best_distance = dist;
                best_seed_uv = neighbor_seed_uv;
            }
        }
    }
    
    COLOR = vec4(best_seed_uv, 0.0, 1.0);
}