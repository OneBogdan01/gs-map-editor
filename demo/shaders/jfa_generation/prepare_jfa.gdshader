shader_type canvas_item;

uniform sampler2D lookup_map : filter_nearest;
uniform sampler2D color_map : source_color, filter_nearest;
uniform int radius:hint_range(0, 5, 1) = 1;
bool is_same_color(vec3 a, vec3 b) {
	vec3 diff = abs(a - b);
	return  diff.r < 0.000001 &&  diff.g < 0.000001 &&  diff.b < 0.000001;
}
vec4 get_province_color(vec2 uv) {
    vec4 lookup = texture(lookup_map, uv);
    vec2 province_id = lookup.rg * 255.0;
    vec2 color_map_size = vec2(textureSize(color_map, 0));
    vec2 color_uv = province_id / color_map_size;
    return texture(color_map, color_uv);
}

void fragment() {
    vec2 uv = UV;
    vec3 center_color = get_province_color(uv).rgb;
    vec2 pixel_size = 1.0 / vec2(textureSize(lookup_map, 0));
    vec2 tex_size = vec2(textureSize(lookup_map, 0));
    bool found_edge = false;

	float min_distance_x = 1.0;
	float min_distance_y = 1.0;

    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
                vec2 offset = vec2(float(x), float(y)) ;
                vec2 sample_uv = uv + offset * pixel_size;
                vec3 sample_color = get_province_color(sample_uv).rgb;

                if (is_same_color(sample_color, center_color) == false) {
                    min_distance_x = min(min_distance_x, abs(offset.x));
                    min_distance_y = min(min_distance_y, abs(offset.y));
                    found_edge = true;
                }
            }
        }


 	if (found_edge) {
        COLOR = vec4((min_distance_x + 0.5)/ tex_size.x, (min_distance_y+0.5) / tex_size.y, 0.0, 1.0);
    } else {
        // Non-edge pixels get marker value
        COLOR = vec4(1.0, 1.0, 0.0, 1.0);
    }
    COLOR = vec4(min_distance_x, min_distance_y, 0.0, 1.0);
}