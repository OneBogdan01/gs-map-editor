shader_type spatial;
render_mode unshaded;
uniform sampler2D lookup_map : filter_nearest; 
uniform sampler2D color_map : source_color, filter_nearest;
uniform sampler2D country_map : source_color, filter_nearest;



uniform vec4 border_color : source_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform float border_thickness: hint_range(0.0, 10.0, 0.1) = 0.6;
void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
	// Called for every pixel the material is visible on.
	vec2 uv = UV;  
    vec2 texture_size = vec2(textureSize(country_map, 0));
    vec2 pixel_size = 1.0 / texture_size;
    //vec4 center_lookup = texture(lookup_map, uv);
    //vec2 province_id_xy = vec2(center_lookup.r * 255.0 , center_lookup.g * 255.0);
    
	//vec2 color_map_size = vec2(textureSize(color_map, 0));
    //vec2 color_uv = province_id_xy / color_map_size;
    
    // Sample the color
    vec4 color = texture(country_map, uv);
    // Sample neighboring pixels

    vec4 top         = texture(country_map, uv + vec2( 0, -pixel_size.y* border_thickness));
    vec4 left        = texture(country_map, uv + vec2(-pixel_size.x* border_thickness,  0));
    vec4 right       = texture(country_map, uv + vec2( pixel_size.x*border_thickness,  0));
    vec4 bottom      = texture(country_map, uv + vec2( 0,  pixel_size.y*border_thickness));

    bool isEdge = any(notEqual(color, top)) ||
                  any(notEqual(color, left)) ||
                  any(notEqual(color, right)) ||
                  any(notEqual(color, bottom));

	float alpha = 1.0;
	if (isEdge){
		color= border_color;
		alpha = border_color.a;
	}
    vec3 final_color = color.rgb;
	ALBEDO = final_color.rgb;
	ALPHA = alpha;
}

