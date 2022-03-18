#version 120

attribute vec3 vPositionModel; // in object space
attribute vec3 vNormalModel; // in object space

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
vec3 illum = vec3(0.0f,0.0f,0.0f);

void main()
{
	gl_Position = projection * view * model * vec4(vPositionModel, 1.0);

	//color is I in our shading formula
	
	vec4 model4D = model * vec4(vPositionModel, 1); //homogenize
	vec3 model3D = vec3(model4D/model4D.w); //unhomogenize

	vec4 modelNorm4D = inverseTrans * vec4(vNormalModel,0); //homogenize
	vec3 modelNorm3D = normalize(vec3(modelNorm4D.x, modelNorm4D.y, modelNorm4D.z)); //unhomogenize

	vec3 lightVec = normalize(lights[0].position - model3D);
	vec3 eyeVec = normalize(eye - model3D);

	vec3 rVec = normalize(((2 * dot(lightVec, modelNorm3D)) * modelNorm3D) - lightVec);
	illum+= (lights[0].color * ((kd * max(0, dot(lightVec, modelNorm3D))) + (ks * (pow(max(0, dot(rVec, eyeVec)), s)))));
	
	vec3 lightVec2 = normalize(lights[1].position - model3D);
	vec3 rVec2 = normalize(((2 * dot(lightVec2, modelNorm3D)) * modelNorm3D) - lightVec2);
	illum+= (lights[1].color * ((kd * max(0, dot(lightVec2, modelNorm3D))) + (ks * (pow(max(0, dot(rVec2, eyeVec)), s)))));
	
	
	//
	color = ka + illum;

	//color = vec3(1.0f, 0.0f, 0.0f);
}
