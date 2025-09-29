#[compute]
#version 450

// Define local work group size
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;


// Input texture (read-only)
layout(set = 0, binding = 0, rgba8ui) restrict readonly uniform uimage2D input_image;

// Output texture (write-only)  
layout(set = 0, binding = 1, rg8) restrict writeonly uniform image2D color_lookup;

layout(set = 0, binding = 2, std430) restrict readonly buffer ColorIdBuffer {
    uint color_id[];
} color_id_buffer;



void main() {
    // Get current pixel coordinates
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    
    
    // Read pixel from input texture
    uvec4 input_color = imageLoad(input_image, coords);
    uvec3 pixel_rgb = input_color.rgb;
    uint province_id = 0u;

    int num_provinces = color_id_buffer.color_id.length() /4;
    for (int i = 0 ; i < num_provinces; i++){
        int step_i = i*4;
        uvec3 stored_color = uvec3(
        color_id_buffer.color_id[step_i],    
        color_id_buffer.color_id[step_i + 1], 
        color_id_buffer.color_id[step_i + 2]  
    );
    
        if (pixel_rgb == stored_color) {
            province_id = color_id_buffer.color_id[step_i + 3]; 
            break;
        }
    }
    vec4 output_color=vec4(1.0, 1.0, 1.0, 1.0);
    if(province_id > 0u){
        uint x = province_id % 256u;
        uint y = province_id / 256u;
        output_color = vec4 (float(x)/255, float(y)/255, 0.0, 1.0);
    }
    imageStore(color_lookup, coords, output_color);
    
}