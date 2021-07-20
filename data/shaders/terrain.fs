
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D backgroundTexture;
uniform sampler2D rTexture;
uniform sampler2D gTexture;
uniform sampler2D bTexture;
uniform sampler2D blendMap;

uniform float u_time;

void main()
{
	vec2 uv = v_uv;

	vec4 blendMapColour = texture(blendMap, v_uv);

	float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b );
	vec2 tiledCoords = v_uv * 40.0;
	vec4 backgroundTextureColour = texture(backgroundTexture, tiledCoords ) * backTextureAmount;
	vec4 rTextureColour = texture(rTexture, tiledCoords) * blendMapColour.r;
	vec4 gTextureColour = texture(gTexture, tiledCoords) * blendMapColour.g;
	vec4 bTextureColour = texture(bTexture, tiledCoords) * blendMapColour.b;
	
	vec4 totalColour = backgroundTextureColour + rTextureColour + gTextureColour + bTextureColour;

	gl_FragColor = u_color * totalColour;

}

//https://stackoverflow.com/questions/45948338/java-lwjgl-shader-error