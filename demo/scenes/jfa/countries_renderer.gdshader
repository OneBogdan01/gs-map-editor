shader_type canvas_item;

uniform sampler2D lookup_map : filter_nearest;
uniform sampler2D color_map : filter_nearest;

uniform int focused_province_id = -1;
uniform int selected_province_id = -1; // New: Persistent selection

// Toggles
uniform bool highlight_country = true;           
uniform bool highlight_province = true;      
uniform bool highlight_all_country_provinces = false; 

// VISUAL STYLES
uniform vec4 country_tint : source_color = vec4(1.0, 1.0, 1.0, 0.1);
uniform vec4 province_tint : source_color = vec4(1.0, 1.0, 1.0, 0.25);
uniform vec4 selected_province_tint : source_color = vec4(1.0, 0.9, 0.0, 0.4); // New: Yellowish tint

void fragment() {
    vec4 lookup = texture(lookup_map, UV);
    
    // 1. Optimization: Early exit if pixel is transparent
    if (lookup.a == 0.0) {
        COLOR = texture(color_map, lookup.rg);
    } else {
        vec4 base_color = texture(color_map, lookup.rg);
        uint current_id = uint(lookup.r * 255.5) + (uint(lookup.g * 255.5) * 256u);
        vec4 out_color = base_color;

        // 2. Identify target country color (via focused province)
        if (focused_province_id >= 0) {
            vec2 target_uv = vec2(float(focused_province_id % 256) / 255.0, float(focused_province_id / 256) / 255.0);
            vec4 target_country_color = texture(color_map, target_uv);
            bool is_target_country = (distance(base_color.rgb, target_country_color.rgb) < 0.001);

            if (is_target_country) {
                // LAYER 1: Country Glow
                if (highlight_country) {
                    out_color = mix(out_color, country_tint, country_tint.a);
                }

                // LAYER 2: Focused/Hovered Province Tinting
                bool is_focused = (current_id == uint(focused_province_id));
                if ((highlight_province && is_focused) || highlight_all_country_provinces) {
                    out_color = mix(out_color, province_tint, province_tint.a);
                }
            }
        }

        // LAYER 3: Selected Province Tinting (Overwrites previous layers)
        if (selected_province_id >= 0) {
            if (current_id == uint(selected_province_id)) {
                out_color = mix(out_color, selected_province_tint, selected_province_tint.a);
            }
        }
        
        COLOR = out_color;
    }
}