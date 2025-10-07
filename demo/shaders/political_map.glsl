#[compute]
#version 450

// Define local work group size
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Input texture (read-only)
layout(set = 0, binding = 0, rg8) restrict readonly uniform image2D color_lookup;
layout(set = 0, binding = 1, rgba8) restrict readonly uniform image2D color_map;

layout(set = 0, binding = 2, rgba8) restrict writeonly uniform image2D political_map;

void main()
{
	// Get current pixel coordinates
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);

	// Read UVs from input texture
	vec4 uv = imageLoad(color_lookup, coords);
	// start from the first pixel
	ivec2 lookup_uv = ivec2(uv.x * 255, uv.y * 255);
	// Get the color of that pixel from the normalized UVs
	vec4 output_color = imageLoad(color_map, lookup_uv);

	// Write to output texture
	imageStore(political_map, coords, output_color);
}