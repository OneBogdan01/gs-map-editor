#[compute]
#version 450

// Define local work group size
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(set = 0, binding = 0, r8) restrict uniform image2D edge_map;
layout(set = 0, binding = 1, rg8) restrict uniform image2D buffer_a;
layout(set = 0, binding = 2, rg8) restrict uniform image2D buffer_b;

void main() {
    // Get current pixel coordinates
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    


}