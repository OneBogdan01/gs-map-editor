shader_type spatial;
render_mode unshaded;

uniform sampler2D subview_image : source_color, filter_linear_mipmap, repeat_enable;

// Parameters
uniform float threshold = 0.00001;
uniform float aa_scale = 17.6;
uniform float base_line_thickness = 0.38197;
const int ML = 0;

// Diagonal line blending logic
bool diag(inout vec4 sum, vec2 ip, vec2 p1, vec2 p2, float thickness)
{
	vec4 v1 = texelFetch(subview_image, ivec2(ip + p1), ML);
	vec4 v2 = texelFetch(subview_image, ivec2(ip + p2), ML);

	if (length(v1 - v2) < threshold)
	{
		vec2 dir = normalize(vec2((p2 - p1).y, -(p2 - p1).x));
		vec2 lp = ip - (floor(ip + p1) + vec2(0.5));
		float l = clamp((thickness - dot(lp, dir)) * aa_scale, 0.0, 1.0);
		sum = mix(sum, v1, l);
		return true;
	}
	return false;
}

void fragment()
{
	// Textures

	vec3 subview_texture = texture(subview_image, UV).rgb;

	// Apply smoothing
	vec2 tex_size = vec2(textureSize(subview_image, 0));
	vec2 ip = UV * tex_size;
	vec4 s = texelFetch(subview_image, ivec2(ip), ML);
	if (diag(s, ip, vec2(-1, 0), vec2(0, 1), base_line_thickness))
	{
		diag(s, ip, vec2(-1, 0), vec2(1, 1), 0.25);
		diag(s, ip, vec2(-1, -1), vec2(0, 1), 0.25);
	}
	if (diag(s, ip, vec2(0, 1), vec2(1, 0), base_line_thickness))
	{
		diag(s, ip, vec2(0, 1), vec2(1, -1), 0.25);
		diag(s, ip, vec2(-1, 1), vec2(1, 0), 0.25);
	}
	if (diag(s, ip, vec2(1, 0), vec2(0, -1), base_line_thickness))
	{
		diag(s, ip, vec2(1, 0), vec2(-1, -1), 0.25);
		diag(s, ip, vec2(1, 1), vec2(0, -1), 0.25);
	}
	if (diag(s, ip, vec2(0, -1), vec2(-1, 0), base_line_thickness))
	{
		diag(s, ip, vec2(0, -1), vec2(-1, 1), 0.25);
		diag(s, ip, vec2(1, -1), vec2(-1, 0), 0.25);
	}

	ALBEDO = s.rgb; //mix(s.rgb, terrain_texture, 0.4);
}

//void light() {
//	// Called for every pixel for every light affecting the material.
//	// Uncomment to replace the default light processing function with this one.
//}
