shader_type canvas_item;

group_uniforms Map_Params;
uniform sampler2D map : source_color, filter_nearest;
uniform vec3 exclude_color : source_color = vec3(1.0, 1.0, 0.0); // Yellow default
uniform float color_threshold : hint_range(0.0, 1.0) = 0.01; // Tolerance for color matching

group_uniforms Color_Tweaks;
uniform float min_brightness : hint_range(0.0, 1.0) = 0.4; // Minimum brightness
uniform float max_brightness : hint_range(0.0, 1.0) = 1.0; // Maximum brightness
uniform float saturation : hint_range(0.0, 1.0) = 0.5; // How yellow vs gray (0 = gray, 1 = full yellow)

group_uniforms State_Selection;
uniform sampler2D color_palette : source_color, filter_nearest;
uniform int focused_state_id = -1;
uniform int selected_state_id = -1;
uniform vec4 focus_color : source_color = vec4(1.0, 1.0, 0.0, 1.0);
uniform float focus_intensity : hint_range(0.0, 1.0) = 0.45;
uniform vec4 select_color : source_color = vec4(0.0, 1.0, 0.0, 1.0);
uniform float select_intensity : hint_range(0.0, 1.0) = 0.55;


// Simple hash function for randomness based on position
float random(vec2 st) {
    return fract(sin(dot(st, vec2(12.9898, 78.233))) * 43758.5453123);
}

void fragment() {
    vec4 color = texture(map, UV);
	// 1. Get raw data and reconstruct the ID
    int r = int(round(color.r * 255.0));
    int g = int(round(color.g * 255.0));
    int current_pixel_id = r + (g * 256);
    
    if (color.a < 0.01) {
        discard;
    }
    
    // Check if the color matches the exclude color (within threshold)
    vec3 diff = abs(color.rgb - exclude_color);
    if (diff.r < color_threshold && diff.g < color_threshold && diff.b < color_threshold) {
        discard;
    }
    
    // Use the original color as a seed for consistent randomness per region
    float seed = random(color.rb);
    float rand_val = random(vec2(seed, color.g));
    
    // Map random value to brightness range (never too dark)
    float brightness = mix(min_brightness, max_brightness, rand_val);
    
    // Create yellow shade (yellow = high R, high G, low B)
    vec3 yellow = vec3(brightness, brightness * mix(0.85, 1.0, rand_val), 0.0);
    
    // Gray version
    vec3 gray = vec3(brightness);
    
    // Mix between gray and yellow based on saturation
    //vec3 final_color = mix(gray, yellow, saturation);
	vec4 final_color = vec4(mix(gray, yellow, saturation), color.a);
	
	if (current_pixel_id > -1) {
	    if (current_pixel_id == selected_state_id) {
	        final_color = mix(final_color, select_color, select_intensity);
	    } else if (current_pixel_id == focused_state_id) {
	        final_color = mix(final_color, focus_color, focus_intensity);
	    }
		COLOR = final_color;
	} else {
		COLOR = final_color;
	}
}