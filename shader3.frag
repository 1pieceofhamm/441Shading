#version 120

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 inverseTrans;

struct lightStruct
{
	vec3 position;
	vec3 color;
};

#define NUM_LIGHTS 2

uniform lightStruct lights[NUM_LIGHTS];

uniform vec3 eye;
uniform int lMode;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

varying vec3 color;
varying vec3 posFrag;
varying vec3 normFrag;
vec3 illum = vec3(0.0f,0.0f,0.0f);

void main()
{
	//vec3 lightVec = normalize(lights[0].position - posFrag);
	
	vec3 eyeVec = normalize(eye - posFrag);
	float angle = dot(normFrag,eyeVec);
	angle = angle * 180 / 3.1415;
	angle = abs(angle);
	if(angle > 16){
		//gl_FragColor = vec4(1.0f,1.0f,1.0f,1.0f);
		gl_FragColor = vec4(0.0f,0.0f,0.0f,1.0f);
	}
	else{
		//gl_FragColor = vec4(0.0f,0.0f,0.0f,1.0f);
		gl_FragColor = vec4(1.0f,1.0f,1.0f,1.0f);
	}

	//vec3 rVec = normalize(((2 * dot(lightVec, normFrag)) * normFrag) - lightVec);
	//illum+= (lights[0].color * ((kd * max(0, dot(lightVec, normFrag))) + (ks * (pow(max(0, dot(rVec, eyeVec)), s)))));
	
	//vec3 lightVec2 = normalize(lights[1].position - posFrag);
	//vec3 rVec2 = normalize(((2 * dot(lightVec2, normFrag)) * normFrag) - lightVec2);
	//illum+= (lights[1].color * ((kd * max(0, dot(lightVec2, normFrag))) + (ks * (pow(max(0, dot(rVec2, eyeVec)), s)))));
	
	//
	//vec3 colorFrag = ka+illum;
	//color = ka + illum;
	//gl_FragColor = vec4(colorFrag, 1.0f);
}
