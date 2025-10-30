#[compute]
#version 450

// Define local work group size
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Input texture (read-only)
layout(set = 0, binding = 0, rg8) restrict readonly uniform image2D lookup_map;
layout(set = 0, binding = 1, rgba8) restrict readonly uniform image2D color_map;

layout(set = 0, binding = 2, r8) restrict writeonly uniform image2D edge_map;

void main()
{
	// Get current pixel coordinates
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
	// Read UVs from input texture
	vec4 uv = imageLoad(lookup_map, coords);
	// start from the first pixel
	ivec2 lookup_uv = ivec2(uv.x * 255, uv.y * 255);
	// Get the color of that pixel from the normalized UVs
	vec4 output_color = imageLoad(color_map, lookup_uv);

	vec4 top = imageLoad(lookup_map, coords + ivec2(0, -1));
	vec4 left = imageLoad(lookup_map, coords + ivec2(-1, 0));
	vec4 right = imageLoad(lookup_map, coords + ivec2(1, 0));
	vec4 bottom = imageLoad(lookup_map, coords + ivec2(0, 1));

	bool isEdge = any(notEqual(uv, top)) ||
			any(notEqual(uv, left)) ||
			any(notEqual(uv, right)) ||
			any(notEqual(uv, bottom));

	float edgeValue = isEdge ? 1.0 : 0.0;
	imageStore(edge_map, coords, vec4(edgeValue));
}