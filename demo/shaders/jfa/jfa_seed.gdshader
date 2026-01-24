shader_type canvas_item;

// This should be your Province Map texture
uniform sampler2D lookup_map : filter_nearest;
uniform vec2 tex_size = vec2(5632.0, 2048.0);

void fragment() {
    vec2 pixel_size = 1.0 / tex_size;
    vec4 center_color = texture(lookup_map, UV);

    bool is_edge = false;

    // Check 4-way neighbors (up, down, left, right)
    // We check neighbors to see if we are at a boundary
    vec2 offsets[4] = vec2[](
        vec2(1.0, 0.0), vec2(-1.0, 0.0),
        vec2(0.0, 1.0), vec2(0.0, -1.0)
    );

    for(int i = 0; i < 4; i++) {
        vec4 neighbor_color = texture(lookup_map, UV + offsets[i] * pixel_size);
        if (neighbor_color != center_color) {
            is_edge = true;
            break;
        }
    }

    if (is_edge) {
        // This pixel is a border. Store its own UV coordinate.
        // Red = UV.x, Green = UV.y
        COLOR = vec4(UV.x, UV.y, 0.0, 1.0);
    } else {
        // Not a border. Store an "invalid" coordinate far outside 0.0-1.0.
        // We use 99.0 so that the JFA shader knows to ignore this pixel.
        COLOR = vec4(99.0, 99.0, 0.0, 1.0);
    }
}