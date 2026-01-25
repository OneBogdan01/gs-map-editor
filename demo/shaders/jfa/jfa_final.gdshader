shader_type canvas_item;

// filter_nearest is key to reading the JFA coordinate exactly
uniform sampler2D screen_texture : hint_screen_texture, filter_nearest;
uniform vec2 tex_size = vec2(5632.0, 2048.0);
uniform float max_distance = 10.0;

void fragment() {
    // 1. Read the coordinate stored in the last JFA pass
    vec4 jfa_data = texture(screen_texture, SCREEN_UV);
    vec2 nearest_seed_uv = jfa_data.xy;

    // 2. Check for the 'Yellow' null value (99.0)
    // If it's 99.0, it means no border was found within 7 pixels
    if (nearest_seed_uv.x > 2.0) {
        COLOR = vec4(1.0, 1.0, 1.0, 1.0); // Out of range = White
    } else {
        // 3. Calculate distance in PIXELS
        // We multiply both by tex_size to convert 0..1 to 0..5632
        float dist = distance(SCREEN_UV * tex_size, nearest_seed_uv * tex_size);

        // 4. Create the gradient (0.0 at the edge, 1.0 at max_distance)
        float fraction = clamp(dist / max_distance, 0.0, 1.0);

        // 5. Output the result
        COLOR = vec4(vec3(fraction), 1.0);
    }
}