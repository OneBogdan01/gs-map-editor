shader_type canvas_item;

uniform sampler2D lookup_map : filter_nearest;
uniform sampler2D color_map : source_color, filter_nearest;

vec4 get_color(const in vec2 uv) {
	// UV in range [0-1]
    vec4 lookup = texture(lookup_map, uv);
	// convert to the value of the 
    vec2 province_uv = lookup.rg;
    return texture(color_map, province_uv);
}

void fragment() {
    vec2 uv = UV;
	
    vec4 color = get_color(uv);

    COLOR = color;
}
