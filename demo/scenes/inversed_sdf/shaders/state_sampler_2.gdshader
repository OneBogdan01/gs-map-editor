shader_type canvas_item;

uniform sampler2D state_renderer : filter_linear;

group_uniforms Anti_Aliasing;
uniform float threshold : hint_range(0.0, 0.1, 0.0001) = 0.00001;
uniform float aa_scale : hint_range(0.0, 50.0, 0.1) = 17.6;
uniform float base_line_thickness : hint_range(0.0, 1.0, 0.01) = 0.38197;

group_uniforms Global_Settings;
uniform float global_alpha : hint_range(0.0, 1.0, 0.01) = 0.6;

group_uniforms Distance_Field_Effect;
uniform sampler2D distance_field_texture : filter_linear; 
uniform float transparency_radius : hint_range(0.0, 1.0) = 0.965;
uniform float center_transparency : hint_range(0.0, 1.0) = 0.2;

group_uniforms Terrain_Reference;
// Your terrain.bmp texture
uniform sampler2D terrain_texture : filter_linear;
// The exact color of your ocean in the terrain.bmp
uniform vec4 ocean_color : source_color = vec4(0.031, 0.122, 0.510, 1.0);
uniform vec4 ocean_tiles_tint : source_color = vec4(0.146, 0.385, 0.9, 1.0);
// Sensitivity for detecting the ocean color
uniform float ocean_tolerance : hint_range(0.0, 0.1) = 0.005;
uniform float ocean_alpha : hint_range(0.0, 1.0) = 0.25;

const int ML = 0;

// --- AA UPSCALING LOGIC ---
bool diag(inout vec4 sum, vec2 ip, vec2 p1, vec2 p2, float thickness) {
    vec4 v1 = texelFetch(state_renderer, ivec2(ip + p1), ML);
    vec4 v2 = texelFetch(state_renderer, ivec2(ip + p2), ML);
    vec4 diff = v1 - v2;
    if (dot(diff, diff) >= threshold * threshold) return false;
    vec2 dir = normalize(vec2((p2 - p1).y, -(p2 - p1).x));
    vec2 lp = ip - (floor(ip + p1) + vec2(0.5));
    float l = clamp((thickness - dot(lp, dir)) * aa_scale, 0.0, 1.0);
    sum = mix(sum, v1, l);
    return true;
}

vec4 sample_aa(vec2 ip) {
    vec4 s = texelFetch(state_renderer, ivec2(ip), ML);
    if (diag(s, ip, vec2(-1, 0), vec2(0, 1), base_line_thickness)) {
        diag(s, ip, vec2(-1, 0), vec2(1, 1), 0.25);
        diag(s, ip, vec2(-1, -1), vec2(0, 1), 0.25);
    }
    if (diag(s, ip, vec2(0, 1), vec2(1, 0), base_line_thickness)) {
        diag(s, ip, vec2(0, 1), vec2(1, -1), 0.25);
        diag(s, ip, vec2(-1, 1), vec2(1, 0), 0.25);
    }
    if (diag(s, ip, vec2(1, 0), vec2(0, -1), base_line_thickness)) {
        diag(s, ip, vec2(1, 0), vec2(-1, -1), 0.25);
        diag(s, ip, vec2(1, 1), vec2(0, -1), 0.25);
    }
    if (diag(s, ip, vec2(0, -1), vec2(-1, 0), base_line_thickness)) {
        diag(s, ip, vec2(0, -1), vec2(-1, 1), 0.25);
        diag(s, ip, vec2(1, -1), vec2(-1, 0), 0.25);
    }
    return s;
}

void fragment() {
    vec2 tex_size = vec2(textureSize(state_renderer, 0));
    vec2 ip = UV * tex_size;
    
    // 1. Get upscaled province color
    vec4 color = sample_aa(ip);
    
    if (color.a < 0.01) discard;

    // 2. Sample terrain.bmp at the same UV to check for ocean
    vec3 terrain_sample = texture(terrain_texture, UV).rgb;
    float dist_to_ocean = distance(terrain_sample, ocean_color.rgb);
    
    float target_alpha = 1.0;

    // 3. Apply SDF transparency ONLY if it's NOT ocean
    if (dist_to_ocean > ocean_tolerance) {
        float sdf_val = texture(distance_field_texture, UV).r;
		
        float influence = smoothstep(1.0, transparency_radius, sdf_val);
		
		// --- SMOOTHING CHANGE START ---
        // Instead of a narrow smoothstep window, we normalize the SDF 
        // and use a power function to create a "curved" falloff.
        //float influence = clamp(1.0 - sdf_val, 0.0, 1.0);
		//// Raising to a power stretches the curve. 
        //// Higher transparency_radius = much softer, more "bloomed" center.
        //influence = pow(influence, transparency_radius * 5.0);
		float smoothed_influence = influence * influence * influence;
		
        target_alpha = mix(1.0, center_transparency, smoothed_influence);
    } else {
		float sdf_val = texture(distance_field_texture, UV).r;
        float influence = smoothstep(1.0, transparency_radius, sdf_val);
        target_alpha = mix(1.0, center_transparency, influence);
		//color = ocean_tiles_tint;
		color *= ocean_tiles_tint;
		target_alpha = ocean_alpha;
	}

    COLOR = vec4(color.rgb, color.a * target_alpha * global_alpha);
}