// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <map>
#include <vector> // STL dynamic memory.
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>

// Project includes
#include "maths_funcs.h"
#define GLT_IMPLEMENTATION
#include "gltext.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define MESH_CONE "./models/cone.dae"
#define MESH_CUBE "./models/cube.dae"
#define MESH_SPHERE "./models/sphere.dae"
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/

struct ModelData
{
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
};

ModelData mesh_cone, mesh_sphere, mesh_cube;

using namespace std;
GLuint SkyBoxID, PhongID;

ModelData mesh_data;
unsigned int mesh_vao = 0;
int width = 1600;
int height = 1200;

glm::mat4 persp_proj = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
glm::mat4 model;

// Camera pos
glm::vec3 camera_pos = glm::vec3(8.0f, 0.0f, 0.0f);
glm::vec3 camera_dir = glm::vec3(-8.0f, 0.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
// simple model
GLfloat Length = 1.2f;
GLfloat Pos1_x = 0.0f, Pos1_y = -1.0f, Pos1_z = 1.8f, Ang1 = 10.0f;
GLfloat Pos2_x = 0.0f, Pos2_y = -1.0f, Pos2_z = 0.0f, Ang2 = 0.0f;
GLfloat Pos3_x = 0.0f, Pos3_y = -1.0f, Pos3_z = 0.0f, Ang3 = 0.0f;
GLfloat Pos4_x = 0.0f, Pos4_y = -0.0f, Pos4_z = 1.8f, Ang4 = 0.0f;
GLfloat Pos5_x = 0.0f, Pos5_y = -0.0f, Pos5_z = 0.0f, Ang5 = 0.0f;
GLfloat Pos6_x = 0.0f, Pos6_y = -0.0f, Pos6_z = 0.0f, Ang6 = 0.0f;
GLfloat S_end_x = 0.0f,  S_end_z = 0.0f,   S_end_y = 0.0f;
boolean Simple_Mode = false;
boolean Cant_reach = false;

GLfloat shininess = 20.0f;
GLfloat ks = 0.5f;
GLuint loc1, loc2;

// ------------ SKYBOX ------------
unsigned int skyboxVAO, skyboxVBO;
unsigned int cubemapTexture;
vector<std::string> faces
{
	"./skybox/px2.jpg",
	"./skybox/nx2.jpg",
	"./skybox/py2.jpg",
	"./skybox/ny2.jpg",
	"./skybox/pz2.jpg",
	"./skybox/nz2.jpg"
};

float skyboxVertices[] = {
	-200.0f,  200.0f, -200.0f,
	-200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,
	 200.0f,  200.0f, -200.0f,
	-200.0f,  200.0f, -200.0f,

	-200.0f, -200.0f,  200.0f,
	-200.0f, -200.0f, -200.0f,
	-200.0f,  200.0f, -200.0f,
	-200.0f,  200.0f, -200.0f,
	-200.0f,  200.0f,  200.0f,
	-200.0f, -200.0f,  200.0f,

	 200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,

	-200.0f, -200.0f,  200.0f,
	-200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f, -200.0f,  200.0f,
	-200.0f, -200.0f,  200.0f,

	-200.0f,  200.0f, -200.0f,
	 200.0f,  200.0f, -200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	-200.0f,  200.0f,  200.0f,
	-200.0f,  200.0f, -200.0f,

	-200.0f, -200.0f, -200.0f,
	-200.0f, -200.0f,  200.0f,
	 200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,
	-200.0f, -200.0f,  200.0f,
	 200.0f, -200.0f,  200.0f
};
#pragma region MESH LOADING
/*----------------------------------------------------------------------------
MESH LOADING FUNCTION
----------------------------------------------------------------------------*/

ModelData load_mesh(const char* file_name) {
	ModelData modelData;

	const aiScene* scene = aiImportFile(
		file_name,
		aiProcess_Triangulate | aiProcess_PreTransformVertices
	);

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return modelData;
	}

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", mesh->mNumVertices);
		modelData.mPointCount += mesh->mNumVertices;
		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(vec2(vt->x, vt->y));
			}
			if (mesh->HasTangentsAndBitangents()) {
			}
		}
	}

	aiReleaseImport(scene);
	return modelData;
}

#pragma endregion MESH LOADING

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS
char* readShaderSource(const char* shaderFile) {
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");
	if (fp == NULL) { return NULL; }
	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';
	fclose(fp);
	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		std::cerr << "Error creating shader..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024] = { '\0' };
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		std::cerr << "Error compiling "
			<< (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader program: " << InfoLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders(const char* vshadername, const char* fshadername)
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	GLuint shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		std::cerr << "Error creating shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, vshadername, GL_VERTEX_SHADER);
	AddShader(shaderProgramID, fshadername, GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { '\0' };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int skyboxTextureID;
	glGenTextures(1, &skyboxTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return skyboxTextureID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
void generateObjectBufferMesh(GLuint& ID, ModelData mesh_data) {
	unsigned int vp_vbo = 0;
	loc1 = glGetAttribLocation(ID, "vertex_position");
	loc2 = glGetAttribLocation(ID, "vertex_normal");

	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mNormals[0], GL_STATIC_DRAW);

	unsigned int vao = 0;
	glBindVertexArray(vao);

	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void generateSkybox() {
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}
#pragma endregion VBO_FUNCTIONS

void drawText(const char* str, GLfloat size, glm::vec3 pos) {
	// Initialize glText
	gltInit();
	// Creating text
	GLTtext* text = gltCreateText();
	gltSetText(text, str);
	// Begin text drawing (this for instance calls glUseProgram)
	gltBeginDraw();
	// Draw any amount of text between begin and end
	gltColor(1.0f, 0.0f, 0.0f, 1.0f);
	gltDrawText2DAligned(text, 70 * (pos.x + 1), 450 - pos.y * 70, size, GLT_CENTER, GLT_CENTER);
	// Finish drawing text
	gltEndDraw();
	// Deleting text
	gltDeleteText(text);
	// Destroy glText
	gltTerminate();
}

void computeSimpleFK() {
	Pos2_y = Pos1_y + Length * sin(glm::radians(Ang1));
	Pos2_z = Pos1_z - Length * cos(glm::radians(Ang1));

	S_end_y = Pos2_y + Length * sin(glm::radians(Ang1 + Ang2));
	S_end_z = Pos2_z - Length * cos(glm::radians(Ang1 + Ang2));
}

void computeComplex45() {
	GLuint loop_num = 0;
	while (loop_num < 50) {
		GLfloat dis6 = sqrt((S_end_y - Pos6_y) * (S_end_y - Pos6_y) + (S_end_z - Pos6_z) * (S_end_z - Pos6_z));
		if (dis6 < 0.1) { break; }
		// move bone 3
		Ang6 = glm::degrees(acos((Pos6_z - S_end_z) / dis6)) - Ang4 - Ang5;
		if (S_end_y < Pos6_y) { Ang6 = -glm::degrees(acos((Pos6_z - S_end_z) / dis6)) - Ang4 - Ang5; }
		dis6 = sqrt((S_end_y - Pos6_y) * (S_end_y - Pos6_y) + (S_end_z - Pos6_z) * (S_end_z - Pos6_z));
		if (dis6 < 0.1) { break; }
		// move bone 2
		GLfloat dis5 = sqrt((S_end_y - Pos5_y) * (S_end_y - Pos5_y) + (S_end_z - Pos5_z) * (S_end_z - Pos5_z));
		Ang5 = glm::degrees(acos((Pos5_z - S_end_z) / dis5)) - Ang4 - 0.5f * Ang6;
		if (S_end_y < Pos5_y) { Ang5 = -glm::degrees(acos((Pos5_z - S_end_z) / dis5)) - Ang4 - 0.5f * Ang6; }
		Pos6_y = Pos5_y + Length * sin(glm::radians(Ang4 + Ang5));
		Pos6_z = Pos5_z - Length * cos(glm::radians(Ang4 + Ang5));
		dis6 = sqrt((S_end_y - Pos6_y) * (S_end_y - Pos6_y) + (S_end_z - Pos6_z) * (S_end_z - Pos6_z));
		if (dis6 < 0.1) { break; }
		// move bone 1
		GLfloat T_end_y = Pos6_y + Length * sin(glm::radians(Ang4 + Ang5 + Ang6));
		GLfloat T_end_z = Pos6_z - Length * cos(glm::radians(Ang4 + Ang5 + Ang6));
		GLfloat dis_t = sqrt((T_end_y - Pos4_y) * (T_end_y - Pos4_y) + (T_end_z - Pos4_z) * (T_end_z - Pos4_z));
		GLfloat t = glm::degrees(acos((Pos4_z - T_end_z) / dis_t)) - Ang4;
		if (T_end_y < Pos4_y) { t = -glm::degrees(acos((Pos4_z - T_end_z) / dis_t)) - Ang4; }
		GLfloat dis4 = sqrt((S_end_y - Pos4_y) * (S_end_y - Pos4_y) + (S_end_z - Pos4_z) * (S_end_z - Pos4_z));
		Ang4 = glm::degrees(acos((Pos4_z - S_end_z) / dis4)) - t;
		if (S_end_y < Pos4_y) { Ang4 = -glm::degrees(acos((Pos4_z - S_end_z) / dis4)) - t; }
		Pos5_y = Pos4_y + Length * sin(glm::radians(Ang4));
		Pos5_z = Pos4_z - Length * cos(glm::radians(Ang4));
		Pos6_y = Pos5_y + Length * sin(glm::radians(Ang4 + Ang5));
		Pos6_z = Pos5_z - Length * cos(glm::radians(Ang4 + Ang5));
		loop_num++;
	}
}

void computeComplexFK() {
	Pos2_y = Pos1_y + Length * sin(glm::radians(Ang1));
	Pos2_z = Pos1_z - Length * cos(glm::radians(Ang1));

	Pos3_y = Pos2_y + Length * sin(glm::radians(Ang1 + Ang2));
	Pos3_z = Pos2_z - Length * cos(glm::radians(Ang1 + Ang2));

	S_end_y = Pos3_y + Length * sin(glm::radians(Ang1 + Ang2 + Ang3));
	S_end_z = Pos3_z - Length * cos(glm::radians(Ang1 + Ang2 + Ang3));
	computeComplex45();
}

void computeSimpleIK() {
	GLfloat y = S_end_y + 1.0f;
	GLfloat z = 1.8f - S_end_z;

	//unreachable
	if (sqrt(z * z + y * y) > 2*Length) {
		Ang1 = glm::degrees(acos(z / sqrt(z * z + y * y)));
		Ang2 = 0;
		Cant_reach = true;
	}
	else {
		Ang1 = glm::degrees(acos(z / sqrt(z * z + y * y)) - acos(sqrt(z * z + y * y) / (2 * Length)));
		Ang2 = glm::degrees(2 * acos(sqrt(z * z + y * y) / (2 * Length)));
		Cant_reach = false;
	}
	if (y < 0) {
		Ang1 = -Ang1;
		Ang2 = -Ang2;
	}
	Pos2_y = Pos1_y + Length * sin(glm::radians(Ang1));
	Pos2_z = Pos1_z - Length * cos(glm::radians(Ang1));
}

void computeComplexIKCCD() {
	GLuint loop_num = 0;
	while (loop_num < 50) {
		GLfloat dis3 = sqrt((S_end_y - Pos3_y)*(S_end_y - Pos3_y) + (S_end_z - Pos3_z)*(S_end_z - Pos3_z));
		if (dis3<0.1) { break; }
		// move bone 3
		Ang3 = glm::degrees(acos((Pos3_z- S_end_z)/dis3)) - Ang1 - Ang2;
		if (S_end_y < Pos3_y) { Ang3 = -glm::degrees(acos((Pos3_z - S_end_z) / dis3)) - Ang1 - Ang2; }
		dis3 = sqrt((S_end_y - Pos3_y) * (S_end_y - Pos3_y) + (S_end_z - Pos3_z) * (S_end_z - Pos3_z));
		if (dis3 < 0.1) { break; }
		// move bone 2
		GLfloat dis2 = sqrt((S_end_y - Pos2_y) * (S_end_y - Pos2_y) + (S_end_z - Pos2_z) * (S_end_z - Pos2_z));
		Ang2 = glm::degrees(acos((Pos2_z - S_end_z) / dis2)) - Ang1 - 0.5f*Ang3;
		if (S_end_y < Pos2_y) { Ang2 = -glm::degrees(acos((Pos2_z - S_end_z) / dis2)) - Ang1 - 0.5f * Ang3; }
		Pos3_y = Pos2_y + Length * sin(glm::radians(Ang1 + Ang2));
		Pos3_z = Pos2_z - Length * cos(glm::radians(Ang1 + Ang2));
		dis3 = sqrt((S_end_y - Pos3_y) * (S_end_y - Pos3_y) + (S_end_z - Pos3_z) * (S_end_z - Pos3_z));
		if (dis3 < 0.1) { break; }
		// move bone 1
		GLfloat T_end_y = Pos3_y + Length * sin(glm::radians(Ang1 + Ang2 + Ang3));
		GLfloat T_end_z = Pos3_z - Length * cos(glm::radians(Ang1 + Ang2 + Ang3));
		GLfloat dis_t = sqrt((T_end_y - Pos1_y) * (T_end_y - Pos1_y) + (T_end_z - Pos1_z) * (T_end_z - Pos1_z));
		GLfloat t = glm::degrees(acos((Pos1_z - T_end_z) / dis_t)) - Ang1;
		if (T_end_y < Pos1_y) {t = -glm::degrees(acos((Pos1_z - T_end_z) / dis_t)) - Ang1;}
		GLfloat dis1 = sqrt((S_end_y - Pos1_y) * (S_end_y - Pos1_y) + (S_end_z - Pos1_z) * (S_end_z - Pos1_z));
		Ang1 = glm::degrees(acos((Pos1_z - S_end_z) / dis1))-t;
		if (S_end_y < Pos1_y) { Ang1 = -glm::degrees(acos((Pos1_z - S_end_z) / dis1)) - t;}
		Pos2_y = Pos1_y + Length * sin(glm::radians(Ang1));
		Pos2_z = Pos1_z - Length * cos(glm::radians(Ang1));
		Pos3_y = Pos2_y + Length * sin(glm::radians(Ang1 + Ang2));
		Pos3_z = Pos2_z - Length * cos(glm::radians(Ang1 + Ang2));
		loop_num++;
	}
	computeComplex45();
	if (sqrt((S_end_y - Pos1_y) * (S_end_y - Pos1_y) + (S_end_z - Pos1_z) * (S_end_z - Pos1_z)) > 3*Length || sqrt((S_end_y - Pos4_y) * (S_end_y - Pos4_y) + (S_end_z - Pos4_z) * (S_end_z - Pos4_z)) > 3 * Length) {
		Cant_reach = true;
	}
	else { Cant_reach = false; }
}

void display() {
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	char array[10];

	//Declare your uniform variables that will be used in your shader
	glm::mat4 view = glm::lookAt(camera_pos, // Camera is at (x,y,z), in World Space
		camera_pos + camera_dir, // and looks at the origin 
		glm::vec3(0, 1, 0));  // Head is up (set to 0,-1,0 to look upside-down)

    // skybox
	cubemapTexture = loadCubemap(faces);
	glDepthFunc(GL_LEQUAL);
	glUseProgram(SkyBoxID);
	generateSkybox();
	glUniformMatrix4fv(glGetUniformLocation(SkyBoxID, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(SkyBoxID, "proj"), 1, GL_FALSE, &persp_proj[0][0]);
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glUseProgram(PhongID);
	glUniformMatrix4fv(glGetUniformLocation(PhongID, "proj"), 1, GL_FALSE, &persp_proj[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(PhongID, "view"), 1, GL_FALSE, &view[0][0]);
	glUniform3f(glGetUniformLocation(PhongID, "color"), 0.9f, 0.3f, 0.1f);
	glUniform1f(glGetUniformLocation(PhongID, "shininess"), shininess);
	glUniform1f(glGetUniformLocation(PhongID, "ks"), ks);
	glUniform3f(glGetUniformLocation(PhongID, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);
	
	// cone1
	generateObjectBufferMesh(PhongID, mesh_cone);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(Pos1_x, Pos1_y, Pos1_z));
	model = glm::rotate(model, glm::radians(Ang1), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(PhongID, "model"), 1, GL_FALSE, &model[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, mesh_cone.mPointCount);
	//cone2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(Pos2_x, Pos2_y, Pos2_z));
	model = glm::rotate(model, glm::radians(Ang1 + Ang2), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(PhongID, "model"), 1, GL_FALSE, &model[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, mesh_cone.mPointCount);
	
	if (Simple_Mode) {
		// cube
		generateObjectBufferMesh(PhongID, mesh_cube);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 1.8f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(PhongID, "model"), 1, GL_FALSE, &model[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, mesh_cube.mPointCount);
	}
	else {
		//cone3
		model = glm::translate(glm::mat4(1.0f), glm::vec3(Pos3_x, Pos3_y, Pos3_z));
		model = glm::rotate(model, glm::radians(Ang1 + Ang2 + Ang3), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(PhongID, "model"), 1, GL_FALSE, &model[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, mesh_cone.mPointCount);
		//cone4
		model = glm::translate(glm::mat4(1.0f), glm::vec3(Pos4_x, Pos4_y, Pos4_z));
		model = glm::rotate(model, glm::radians(Ang4), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniform3f(glGetUniformLocation(PhongID, "color"), 0.2f, 0.9f, 0.1f);
		glUniformMatrix4fv(glGetUniformLocation(PhongID, "model"), 1, GL_FALSE, &model[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, mesh_cone.mPointCount);
		//cone5
		model = glm::translate(glm::mat4(1.0f), glm::vec3(Pos5_x, Pos5_y, Pos5_z));
		model = glm::rotate(model, glm::radians(Ang4 + Ang5), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(PhongID, "model"), 1, GL_FALSE, &model[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, mesh_cone.mPointCount);
		//cone6
		model = glm::translate(glm::mat4(1.0f), glm::vec3(Pos6_x, Pos6_y, Pos6_z));
		model = glm::rotate(model, glm::radians(Ang4 + Ang5 + Ang6), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(PhongID, "model"), 1, GL_FALSE, &model[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, mesh_cone.mPointCount);
		// cube
		generateObjectBufferMesh(PhongID, mesh_cube);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 1.8f));
		model = glm::scale(model, glm::vec3(0.2f, 0.6f, 0.2f));
		glUniform3f(glGetUniformLocation(PhongID, "color"), 0.9f, 0.9f, 0.1f);
		glUniformMatrix4fv(glGetUniformLocation(PhongID, "model"), 1, GL_FALSE, &model[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, mesh_cube.mPointCount);
	}

	// end sphere
	generateObjectBufferMesh(PhongID, mesh_sphere);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(S_end_x, S_end_y, S_end_z));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	glUniform3f(glGetUniformLocation(PhongID, "color"), 0.2f, 0.3f, 0.9f);
	glUniformMatrix4fv(glGetUniformLocation(PhongID, "model"), 1, GL_FALSE, &model[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, mesh_sphere.mPointCount);

	if (Cant_reach) {
		drawText("Can't reach!!!", 5, glm::vec3(13.0f, 4.0f, 0.0f));
	}

	if (Simple_Mode) {
		drawText("Simple Object", 4, glm::vec3(4.0f, 4.0f, 0.0f));
	}
	else {
		drawText("Complex Object", 4, glm::vec3(4.0f, 4.0f, 0.0f));
	}

	glutPostRedisplay();
	glutSwapBuffers();
}

void init()
{
	mesh_cone = load_mesh(MESH_CONE);
	mesh_cube = load_mesh(MESH_CUBE);
	mesh_sphere = load_mesh(MESH_SPHERE);
	// Set up the shaders
	PhongID = CompileShaders("./shaders/PhongVertexShader.txt", "./shaders/simpleFragmentShader.txt");
	SkyBoxID = CompileShaders("./shaders/skyboxVertexShader.txt", "./shaders/skyboxFragmentShader.txt");
	if (Simple_Mode) {
		computeSimpleFK();
	}
	else {
		computeComplexFK();
	}
}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {
	switch (key)
	{
	    case 'q':
			Simple_Mode = !Simple_Mode;
		    break;
	    case 'a':
			Ang1 += 10.0f;
			if (Ang1 > 360.0f) { Ang1 = Ang1-360.0f; }
			break;
		case 'z':
			Ang1 -= 10.0f;
			if (Ang1 < 0.0f) { Ang1 = Ang1 + 360.0f; }
			break;
		case 's':
			Ang2 += 10.0f;
			if (Ang2 > 360.0f) { Ang2 = Ang2 - 360.0f; }
			break;
		case 'x':
			Ang2 -= 10.0f;
			if (Ang2 < 0.0f) { Ang2 = Ang2 + 360.0f; }
			break;
		case 'd':
			Ang3 += 10.0f;
			if (Ang3 > 360.0f) { Ang3 = Ang3 - 360.0f; }
			break;
		case 'c':
			Ang3 -= 10.0f;
			if (Ang3 < 0.0f) { Ang3 = Ang3 + 360.0f; }
			break;
	    default:
		    break;
	}
	if (Simple_Mode) {
		computeSimpleFK();
	}
	else {
		computeComplexFK();
	}
}

void mousePress(int button, int state, int xpos, int ypos) {
	// Wheel reports as button 3(scroll up) and button 4(scroll down)
	if (button == 3) // It's a wheel event
	{
		// Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		printf("Scroll %s At %d %d\n", (button == 3) ? "Up" : "Down", xpos, ypos);
		camera_pos.x += 1.0f;
	}
	else if (button == 4) // It's a wheel event
	{
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		printf("Scroll %s At %d %d\n", (button == 4) ? "Up" : "Down", xpos, ypos);
		camera_pos.x -= 1.0f;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {  // normal button event
		S_end_z = -0.0075f * xpos + 6.0f;
		S_end_y = -0.0075f * ypos + 4.5f;
		if (Simple_Mode) {
			computeSimpleIK();
		}
		else {
			computeComplexIKCCD();
		}
	}
}

int main(int argc, char** argv) {
	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("lab1");

	//texure
	glEnable(GL_DEPTH_TEST);

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutKeyboardFunc(keypress);
	glutMouseFunc(mousePress);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}
