shader_type spatial;
render_mode unshaded;
uniform sampler2D subview_image : source_color, filter_nearest;


void fragment()
{
	ALBEDO = texture(subview_image, UV).rgb;
}