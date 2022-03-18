#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "Program.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define NUM_LIGHTS 2
#define NUM_MATERIALS 3

GLFWwindow *window;

//additional variables
int matMode = 0;
int pro = 0;
int lMode = 0;

Program program[2];
std::vector<float> posBuff;
std::vector<float> norBuff;
std::vector<float> texBuff;

glm::vec3 eye(0.0f, 0.0f, 4.0f);

struct materialStruct {
	glm::vec3 ka, kd, ks;
	float s;
};


struct lightStruct {
	glm::vec3 position;
	glm::vec3 color;
};

materialStruct m1 = materialStruct{ glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.7, 0.7), glm::vec3(1.0, 1.0, 1.0), 10.0 };
materialStruct m2 = materialStruct{ glm::vec3(0.0, 0.2, 0.2), glm::vec3(0.5, 0.7, 0.2), glm::vec3(0.1, 1.0, 0.1), 100.0 };
materialStruct m3 = materialStruct{ glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.1, 0.3, 0.9), glm::vec3(0.1, 0.1, 0.1), 1.0 };
materialStruct materials[NUM_MATERIALS] = { {m1},{m2},{m3} };

lightStruct l1 = lightStruct{glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.5, 0.5, 0.5)};
lightStruct l2 = lightStruct{glm::vec3(0.0, 3.0, 0.0), glm::vec3(0.2, 0.2, 0.2)};
lightStruct lights[NUM_LIGHTS] = { {l1},{l2} };


void Display()
{		
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, -1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 transModel = transpose(modelMatrix);
	glm::mat4 inverseTrans = inverse(transModel);

	program[pro].Bind();
	
	program[pro].SendUniformData(inverseTrans, "inverseTrans");
	program[pro].SendUniformData(eye, "eye");
	program[pro].SendUniformData(lMode, "lMode");

	program[pro].SendUniformData(materials[matMode].ka, "ka");
	program[pro].SendUniformData(materials[matMode].kd, "kd");
	program[pro].SendUniformData(materials[matMode].ks, "ks");
	program[pro].SendUniformData(materials[matMode].s, "s");

	program[pro].SendUniformData(lights[0].position, "lights[0].position");
	program[pro].SendUniformData(lights[0].color, "lights[0].color");
	program[pro].SendUniformData(lights[1].position, "lights[1].position");
	program[pro].SendUniformData(lights[1].color, "lights[1].color");

	program[pro].SendUniformData(modelMatrix, "model");
	program[pro].SendUniformData(viewMatrix, "view");
	program[pro].SendUniformData(projectionMatrix, "projection");
	glDrawArrays(GL_TRIANGLES, 0, posBuff.size() / 3);
	program[pro].Unbind();

}

// Keyboard character callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	switch (key) 
	{
	case 'q':
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case 'm':
		matMode -= 1;
		if (matMode < 0) {
			matMode = 0;
		}
		break;
	case 'M':
		matMode += 1;
		if (matMode > 2) {
			matMode = 2;
		}
		break;
	case '1':
		pro = 0;
		break;
	case '2':
		pro = 1;
		break;
	case '3':
		pro = 2;
		break;
	case 'l':
		lMode = 0;
		break;
	case 'L':
		lMode = 1;
		break;
	case 'x':
		lights[lMode].position += glm::vec3(-1.0f, 0.0f, 0.0f);
		break;
	case 'X':
		lights[lMode].position += glm::vec3(1.0f, 0.0f, 0.0f);
		break;
	case 'y':
		lights[lMode].position += glm::vec3(0.0f, -1.0f, 0.0f);
		break;
	case 'Y':
		lights[lMode].position += glm::vec3(0.0f, 1.0f, 0.0f);
		break;
	case 'z':
		lights[lMode].position += glm::vec3(0.0f, 0.0f, -1.0f);
		break;
	case 'Z':
		lights[lMode].position += glm::vec3(0.0f, 0.0f, 1.0f);
		break;
	default:
		break;
	}
}


void FrameBufferSizeCallback(GLFWwindow* lWindow, int width, int height)
{
	glViewport(0, 0, width, height);
}

void LoadModel(char* name)
{
	// Taken from Shinjiro Sueda with slight modification
	std::string meshName(name);
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if (!rc) {
		std::cerr << errStr << std::endl;
	}
	else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
					if (!attrib.normals.empty()) {
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 0]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 1]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 2]);
					}
					if (!attrib.texcoords.empty()) {
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
}

void Init()
{
	glfwInit();
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment4 - Caleb Hammack", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetCharCallback(window, CharacterCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	LoadModel("../obj/bunny.obj");
	
	program[0].SetShadersFileName("../shaders/shader1.vert", "../shaders/shader1.frag");
	program[0].Init();
	program[0].SendAttributeData(posBuff, "vPositionModel");
	program[0].SendAttributeData(norBuff, "vNormalModel");

	program[1].SetShadersFileName("../shaders/shader2.vert", "../shaders/shader2.frag");
	program[1].Init();
	program[1].SendAttributeData(posBuff, "vPositionModel");
	program[1].SendAttributeData(norBuff, "vNormalModel");

	program[2].SetShadersFileName("../shaders/shader3.vert", "../shaders/shader3.frag");
	program[2].Init();
	program[2].SendAttributeData(posBuff, "vPositionModel");
	program[2].SendAttributeData(norBuff, "vNormalModel");
}


int main()
{	
	Init();
	while ( glfwWindowShouldClose(window) == 0) 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}