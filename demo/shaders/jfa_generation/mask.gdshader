shader_type canvas_item;
uniform sampler2D mask_texture : source_color, filter_nearest;
uniform float threshold : hint_range(0.001, 1.0, 0.001) = 0.2;

void fragment() {

	vec4 center = texture(mask_texture, UV);
	vec2 pixel_size = 1.0 / vec2(textureSize(mask_texture, 0));
	bool edge = false;
	const vec2 offsets[4] = {
		vec2(0.0, -1.0), 
		vec2(0.0, 1.0),   
		vec2(-1.0, 0.0), 
		vec2(1.0, 0.0)    
	};
	
	for (int i = 0; i < 4; i++) {
		vec2 sample_uv = UV + offsets[i] * pixel_size;
		vec4 neighbor = texture(mask_texture, sample_uv);
		float diff = length(center - neighbor);
		
		if (diff > threshold) {
			edge = true;
			break;
		}
	}
	if (edge) {
		COLOR = vec4(UV, 0.0, 1.0);
	} else {
		COLOR = vec4(1.0, 1.0, 0.0, 1.0);
	}
}
