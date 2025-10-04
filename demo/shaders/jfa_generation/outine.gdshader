shader_type canvas_item;

uniform sampler2D input_texture : source_color, filter_linear_mipmap;
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
    vec4 province_color = get_province_color(uv);
    
    // Calculate border regions based on distance field
    float total_width = border_width + inner_glow_width + outer_glow_width;
    
    // Inner glow (inside the province, before the border)
    float inner_glow_start = 0.0;
    float inner_glow_end = inner_glow_width;
    
    // Main border
    float border_start = inner_glow_end;
    float border_end = border_start + border_width;
    
    // Outer glow (outside the border)
    float outer_glow_start = border_end;
    float outer_glow_end = outer_glow_start + outer_glow_width;
    
    vec4 final_color = province_color;
    
    if (df < outer_glow_end) {
        if (df < inner_glow_end) {
            // Inner glow - smooth gradient from province color to glow color
            float t = smoothstep(inner_glow_start, inner_glow_end, df);
            final_color = mix(province_color, inner_glow_color, t);
        }
        else if (df < border_end) {
            // Main border - solid color with slight gradient for anti-aliasing
            float t = smoothstep(border_start, border_start + 0.5, df);
            final_color = mix(inner_glow_color, border_color, t);
        }
        else {
            // Outer glow - fade out to transparent or background
            float t = smoothstep(outer_glow_start, outer_glow_end, df);
            vec4 bg_color = vec4(0.0, 0.0, 0.0, 1.0);
            final_color = mix(border_color, mix(outer_glow_color, bg_color, t), 
                            smoothstep(outer_glow_start, outer_glow_end, df));
        }
    }
    
    COLOR = final_color;
}