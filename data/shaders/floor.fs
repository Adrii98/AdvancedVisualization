
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_floor_mask;
uniform sampler2D u_grass_texture;
uniform sampler2D u_rock_texture;
uniform sampler2D u_soil_texture;
uniform float u_time;

void main()
{
	vec2 uv = v_uv;

	if(texture2D( u_floor_mask, uv ).rgb == vec3(0,1.0,0))
		gl_FragColor = u_color * texture2D( u_grass_texture, uv );
	else if(texture2D( u_floor_mask, uv ).rgb == vec3(1.0,0,0))
		gl_FragColor = u_color * texture2D( u_rock_texture, uv );
	else 
		gl_FragColor = u_color * texture2D( u_soil_texture, uv );
}
