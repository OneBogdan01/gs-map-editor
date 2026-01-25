shader_type canvas_item;

uniform sampler2D screen_texture : hint_screen_texture, filter_nearest;
uniform vec2 tex_size;
uniform float stride; // Change this for each pass (e.g., 512, 256, 128...)

void fragment() {
    vec2 pixel_size = 1.0 / tex_size;
    float best_dist = 99999.0;
    vec2 best_coord = vec2(20.0);
    
    // Sample 9 neighbors at the current stride distance
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            
            // Calculate vector to neighbor
            vec2 offset = vec2(float(x), float(y)) * stride * pixel_size;
            vec2 sample_uv = SCREEN_UV + offset;
            
            // Read the seed coordinate stored at that neighbor
            // (We read from SCREEN_TEXTURE to get the result of the previous pass)
            vec4 sample_data = textureLod(screen_texture, sample_uv, 0);
            vec2 seed_coord = sample_data.xy;
            
            // If this neighbor points to a valid seed (not 100)
            if (seed_coord.x < 20.0 && seed_coord.y < 20.0) {
                // Calculate distance from CURRENT pixel to that SEED
                // Correct for aspect ratio if necessary, here assuming square UVs for simplicity
                float dist = distance(SCREEN_UV * tex_size, seed_coord * tex_size);
                
                if (dist < best_dist) {
                    best_dist = dist;
                    best_coord = seed_coord;
                }
            }
        }
    }
    
    // Output the closest seed found so far
    COLOR = vec4(best_coord.x, best_coord.y, 0.0, 1.0);
}