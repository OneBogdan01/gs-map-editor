shader_type canvas_item;
render_mode unshaded;

uniform sampler2D input_texture : source_color, filter_linear;
uniform sampler2D lookup_map : filter_nearest;
uniform sampler2D color_map : source_color, filter_nearest;

uniform float edge_smoothness : hint_range(0.0, 2., 0.001) = 0.01;
uniform float edge_threshold : hint_range(0.0, 1.0, 0.01) = 0.5;
uniform float border_width : hint_range(0.0, 0.1, 0.001) = 0.02;
uniform vec4 border_color : source_color = vec4(0.0, 0.0, 0.0, 1.0);

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
    vec4 province_color = get_province_color(uv);
    
    // Smooth edge
    float alpha = smoothstep(edge_threshold - edge_smoothness, 
                            edge_threshold + edge_smoothness, 
                            df);
    
   
    

    
    COLOR = vec4(province_color.rgb, alpha);
}