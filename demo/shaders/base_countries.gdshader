shader_type canvas_item;

uniform sampler2D lookup_map : filter_nearest;
uniform sampler2D color_map : source_color, filter_nearest;

vec4 get_province_color(vec2 uv) {
    vec4 lookup = texture(lookup_map, uv);
    vec2 province_id = lookup.rg * 255.0;
    vec2 color_map_size = vec2(textureSize(color_map, 0));
    vec2 color_uv = province_id / color_map_size;
    return texture(color_map, color_uv);
}

void fragment() {
    vec2 uv = UV;
    vec2 texture_size = vec2(textureSize(lookup_map, 0));
    vec2 pixel_size = 1.0 / texture_size;

    vec4 center = get_province_color(uv);
    bool found_edge = false;

    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            vec2 offset = vec2(float(x), float(y)) * pixel_size;
            vec4 neighbor = get_province_color(uv + offset);

            if (neighbor.rgb != center.rgb) {
                found_edge = true;
                break;
            }
        }
        if (found_edge) break;
    }

    if (found_edge) {

        COLOR = vec4(uv, 0.0, 1.0);
    } else {
        COLOR = vec4(1.0, 1.0, 0.0, 1.0);
    }
}