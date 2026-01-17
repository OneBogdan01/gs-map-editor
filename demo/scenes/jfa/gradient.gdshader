/**
* ============================================================
* Location: lite_map/lite_shaders/gradient.gshader
* ============================================================
*/
shader_type canvas_item;

uniform sampler2D country_renderer : filter_linear;
uniform sampler2D distance_field : filter_linear;

group_uniforms Anti_Aliasing;
uniform float threshold : hint_range(0.0, 0.1, 0.0001) = 0.00001;
uniform float aa_scale : hint_range(0.0, 50.0, 0.1) = 17.6;
uniform float base_line_thickness : hint_range(0.0, 1.0, 0.01) = 0.38197;

group_uniforms Alpha_Gradient;
uniform bool use_smooth_gradient = true;
uniform float gradient_start : hint_range(0.0, 1.0, 0.01) = 0.17;
uniform float gradient_end : hint_range(0.0, 1.0, 0.01) = 0.62;
uniform float gradient_opacity : hint_range(0.0, 1.0, 0.01) = 0.5;
uniform vec4 gradient_color_modifier : source_color = vec4(0.5, 0.5, 0.5, 1.0);

const int ML = 0;

// Internal AA helper
bool diag(inout vec4 sum, vec2 ip, vec2 p1, vec2 p2, float thickness) {
	vec4 v1 = texelFetch(country_renderer, ivec2(ip + p1), ML);
	vec4 v2 = texelFetch(country_renderer, ivec2(ip + p2), ML);
	vec4 diff = v1 - v2;
	if (dot(diff, diff) >= threshold * threshold) return false;
	vec2 dir = normalize(vec2((p2 - p1).y, -(p2 - p1).x));
	vec2 lp = ip - (floor(ip + p1) + vec2(0.5));
	float l = clamp((thickness - dot(lp, dir)) * aa_scale, 0.0, 1.0);
	sum = mix(sum, v1, l);
	return true;
}

// Internal AA main function
vec4 sample_aa(vec2 ip) {
	vec4 s = texelFetch(country_renderer, ivec2(ip), ML);
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
	// Optimization: Fetch Distance Field first to see if we even need to run AA
	float df = texture(distance_field, UV).r;

	if (df <= 0.001) {
		COLOR = vec4(0.0);
	} else {
		vec2 tex_size = vec2(textureSize(country_renderer, 0));
		vec2 ip = UV * tex_size;
		vec4 aa_color = sample_aa(ip);

		float gradient_alpha;
		if (use_smooth_gradient) {
			float gradient_range = gradient_end - gradient_start;
			float gradient_position = clamp((df - gradient_start) / gradient_range, 0.0, 1.0);
			gradient_alpha = (1.0 - gradient_position) * gradient_opacity;
		} else {
			float fade_in = smoothstep(gradient_start, gradient_start + 0.05, df);
			float fade_out = 1.0 - smoothstep(gradient_end - 0.05, gradient_end, df);
			gradient_alpha = fade_in * fade_out * gradient_opacity;
		}

		vec3 modified_color = aa_color.rgb * gradient_color_modifier.rgb;
		COLOR = vec4(modified_color, gradient_alpha);
	}
}