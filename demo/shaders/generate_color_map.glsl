#[compute]
#version 450

// Define local work group size
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

 
layout(set = 0, binding = 0, rgba8) restrict writeonly uniform image2D color_map;

layout(set = 0, binding = 1, std430) restrict readonly buffer ColorBuffer {
    vec4 color_id[];
} color_id_buffer;

layout(set = 0, binding = 2, std430) restrict readonly buffer IdBuffer {
    uint id[];
} id_buffer;



void main() {
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    int index = coords.x + coords.y * 256;
    
    vec4 output_color = vec4(0.0, 0.0, 0.0, 1.0); // Default to black
    
    output_color = color_id_buffer.color_id[index];
    
    uint id = id_buffer.id[index];
    ivec2 coords_tex = ivec2(id%256u, id/256u);
    imageStore(color_map, coords_tex, output_color);
    
}