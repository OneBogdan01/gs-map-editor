shader_type canvas_item;
render_mode unshaded;
uniform sampler2D input_texture : source_color, filter_linear;
uniform sampler2D lookup_map : filter_nearest;
uniform sampler2D color_map : source_color, filter_nearest;

// Border configuration
uniform float border_width : hint_range(0.0, 10.0, 0.1) = 3.0;
uniform float inner_glow_width : hint_range(0.0, 5.0, 0.1) = 1.5;
uniform float outer_glow_width : hint_range(0.0, 5.0, 0.1) = 1.5;

// Border colors
uniform vec4 border_color : source_color = vec4(0.2, 0.2, 0.2, 1.0);
uniform vec4 inner_glow_color : source_color = vec4(1.0, 1.0, 1.0, 0.8);
uniform vec4 outer_glow_color : source_color = vec4(0.0, 0.0, 0.0, 0.6);

vec4 get_province_color(vec2 uv) {
    vec4 lookup = texture(lookup_map, uv);
    vec2 province_id = lookup.rg * 255.0;
    vec2 color_map_size = vec2(textureSize(color_map, 0));
    vec2 color_uv = province_id / color_map_size;
    return texture(color_map, color_uv);
}

void fragment() {
    vec2 uv = UV;
    float df = texture(input_texture, UV).r;
   
    
    COLOR = vec4 (df, df, df , 1.0);
}