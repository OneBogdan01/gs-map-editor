shader_type canvas_item;

uniform sampler2D map : source_color, filter_nearest;
//uniform sampler2D centers_map : source_color, filter_nearest;
//uniform sampler2D noise_img : repeat_enable; // Your FastNoiseLite texture
//uniform float spread : hint_range(0.0, 1.0) = 0.5; // Controls how far the transparency reaches
//uniform float noise_strength : hint_range(0.0, 1.0) = 0.5;

void fragment() {
    vec4 color = texture(map, UV);
	//vec4 center_tex = texture(centers_map, UV);
    //float noise_val = texture(noise_img, UV).r;
	
    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    
    COLOR = vec4(vec3(gray), color.a);
}