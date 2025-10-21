shader_type canvas_item;

uniform sampler2D lookup_map : filter_nearest;
uniform sampler2D color_map : source_color, filter_nearest;
uniform float max_distance : hint_range(1.0, 50.0, 1.0) = 10.0;

vec4 get_province_color(vec2 uv)
{
	vec4 lookup = texture(lookup_map, uv);
	vec2 province_id = lookup.rg * 255.0;
	vec2 color_map_size = vec2(textureSize(color_map, 0));
	vec2 color_uv = province_id / color_map_size;
	return texture(color_map, color_uv);
}

void fragment()
{
	vec2 uv = UV;
	vec4 center_color = get_province_color(uv);
	vec2 pixel_size = 1.0 / vec2(textureSize(lookup_map, 0));

	float min_distance = max_distance;
	bool found_edge = false;

	int max_radius = int(max_distance);
	for (int radius = 1; radius <= max_radius; radius++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			for (int y = -radius; y <= radius; y++)
			{
				if (abs(x) == radius || abs(y) == radius)
				{
					vec2 offset = vec2(float(x), float(y));
					vec2 sample_uv = uv + offset * pixel_size;
					vec4 sample_color = get_province_color(sample_uv);

					if (sample_color != center_color)
					{
						float dist = length(offset);
						min_distance = min(min_distance, dist);
						found_edge = true;
					}
				}
			}
		}

		if (found_edge && float(radius) > min_distance)
		{
			break;
		}
	}

	// Normalize distance to 0-1 range and output
	float normalized_distance = min_distance / max_distance;
	COLOR = vec4(vec3(normalized_distance), 1.0);
}