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

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

varying vec3 color;
varying vec3 posFrag;
varying vec3 normFrag;

void main()
{
	gl_Position = projection * view * model * vec4(vPositionModel, 1.0);
	vec4 model4D = model * vec4(vPositionModel, 1); //homogenize
	posFrag = vec3(model4D/model4D.w); //unhomogenize

	vec4 modelNorm4D = inverseTrans * vec4(vNormalModel,0); //homogenize
	normFrag = normalize(vec3(modelNorm4D.x, modelNorm4D.y, modelNorm4D.z)); //unhomogenize

	color = vec3(1.0f, 0.0f, 0.0f);
}
