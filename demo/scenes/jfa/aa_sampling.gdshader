shader_type canvas_item;

group_uniforms Anti_Aliasing;
uniform float threshold : hint_range(0.0, 0.1, 0.0001) = 0.00001;
uniform float aa_scale : hint_range(0.0, 50.0, 0.1) = 17.6;
uniform float base_line_thickness : hint_range(0.0, 1.0, 0.01) = 0.38197;
uniform float corner_cut : hint_range(0.0, 1.0, 0.01) = 0.5; // Controls how much of the corner to "slice"
uniform float opacity: hint_range(0.0, 1.0, 0.1) = 1.0;

const int ML = 0;

bool diag(inout vec4 sum, vec2 ip, vec2 p1, vec2 p2, float thickness, sampler2D tex) {
	vec4 v1 = texelFetch(tex, ivec2(ip + p1), ML);
	vec4 v2 = texelFetch(tex, ivec2(ip + p2), ML);
	vec4 diff = v1 - v2;
	if (dot(diff, diff) >= threshold * threshold) return false;
	
	vec2 dir = normalize(vec2((p2 - p1).y, -(p2 - p1).x));
	vec2 lp = ip - (floor(ip + p1) + vec2(0.5));
	float l = clamp((thickness - dot(lp, dir)) * aa_scale, 0.0, 1.0);
	sum = mix(sum, v1, l);
	return true;
}

void fragment() {
	vec2 tex_size = vec2(textureSize(TEXTURE, 0));
	vec2 ip = UV * tex_size;
	vec2 f = fract(ip); // Sub-pixel position
	
	vec4 s = texelFetch(TEXTURE, ivec2(ip), ML);
	
	// --- CORNER CUTTING PASS ---
	// We check the 4 immediate neighbors to see if we are at a convex corner tip.
	// If the pixel is surrounded by empty/different space on two adjacent sides, we "slice" the UV.
	vec4 n = texelFetch(TEXTURE, ivec2(ip) + ivec2(0, 1), ML);
	vec4 e = texelFetch(TEXTURE, ivec2(ip) + ivec2(1, 0), ML);
	vec4 w = texelFetch(TEXTURE, ivec2(ip) + ivec2(-1, 0), ML);
	vec4 s_up = texelFetch(TEXTURE, ivec2(ip) + ivec2(0, -1), ML);

	// Detect 90-degree outer corners and apply a diagonal alpha mask
	// Top-Right corner
	if (distance(s, e) > threshold && distance(s, s_up) > threshold) {
		float mask = clamp((1.0 + corner_cut - (f.x + (1.0 - f.y))) * aa_scale, 0.0, 1.0);
		s.a *= mask;
	}
	// Top-Left corner
	if (distance(s, w) > threshold && distance(s, s_up) > threshold) {
		float mask = clamp((1.0 + corner_cut - ((1.0 - f.x) + (1.0 - f.y))) * aa_scale, 0.0, 1.0);
		s.a *= mask;
	}
	// Bottom-Right corner
	if (distance(s, e) > threshold && distance(s, n) > threshold) {
		float mask = clamp((1.0 + corner_cut - (f.x + f.y)) * aa_scale, 0.0, 1.0);
		s.a *= mask;
	}
	// Bottom-Left corner
	if (distance(s, w) > threshold && distance(s, n) > threshold) {
		float mask = clamp((1.0 + corner_cut - ((1.0 - f.x) + f.y)) * aa_scale, 0.0, 1.0);
		s.a *= mask;
	}

	// --- ORIGINAL AA PASS ---
	if (diag(s, ip, vec2(-1, 0), vec2(0, 1), base_line_thickness, TEXTURE)) {
		diag(s, ip, vec2(-1, 0), vec2(1, 1), 0.25, TEXTURE);
		diag(s, ip, vec2(-1, -1), vec2(0, 1), 0.25, TEXTURE);
	}
	if (diag(s, ip, vec2(0, 1), vec2(1, 0), base_line_thickness, TEXTURE)) {
		diag(s, ip, vec2(0, 1), vec2(1, -1), 0.25, TEXTURE);
		diag(s, ip, vec2(-1, 1), vec2(1, 0), 0.25, TEXTURE);
	}
	if (diag(s, ip, vec2(1, 0), vec2(0, -1), base_line_thickness, TEXTURE)) {
		diag(s, ip, vec2(1, 0), vec2(-1, -1), 0.25, TEXTURE);
		diag(s, ip, vec2(1, 1), vec2(0, -1), 0.25, TEXTURE);
	}
	if (diag(s, ip, vec2(0, -1), vec2(-1, 0), base_line_thickness, TEXTURE)) {
		diag(s, ip, vec2(0, -1), vec2(-1, 1), 0.25, TEXTURE);
		diag(s, ip, vec2(1, -1), vec2(-1, 0), 0.25, TEXTURE);
	}
	
	s.a *= opacity;
	COLOR = s;
}