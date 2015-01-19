#define _CRT_SECURE_NO_DEPRECATE

//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "stb_image.h"

// Assimp includes

#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include <stdlib.h>
#include "assert.h"

// Header includes

#include "BasicParticles.h"
#include "BasicParticleGenerators.h"
#include "BasicParticleUpdaters.h"
#include "particleRenderer.h"
#include "glParticleRenderer.h"

using namespace std;

int width = 800;
int height = 600;

GLuint _tex;
unsigned char* _img;
int _ix;
int _iy;

#define TEXTURE_PARTICLE "Textures/black_texture.png"
#define V_SHADER_PARTICLE "../Shaders/particleVertexShader.txt"
#define F_SHADER_PARTICLE "../Shaders/particleFragmentShader.txt"

GLuint shaderProgramIDparticle;

int num_particles = 12000;

ParticleSystem mySystem(num_particles);
GLParticleRenderer myRenderer;

int oldTime = 0;

glm::vec3 camera_pos(0.0f, 0.5f, 2.0f);
float cam_move_speed = 0.5f;
float cam_rot_speed = 1.0f;






//loads image to texture
bool load_image_to_texture(const char* file_name, unsigned int& tex, bool gen_mips)
{
	int n;
	int force_channels = 4;

	_img = stbi_load(file_name, &_ix, &_iy, &n, force_channels);

	if(!_img)
	{
		fprintf(stderr, "ERROR: could not load image %s. Check file type and path\n", file_name);
		return false;
	}

	// FLIP UP-SIDE DIDDLY-DOWN
	// make upside-down copy for GL
	unsigned char* imagePtr = &_img[0];
	int half_height_in_pixels = _iy/2;
	int height_in_pixels = _iy;

	//assuming RGBA for 4 components per pixel
	int num_colour_components = 4;

	//assuming each colour component is an unsigned char
	int width_in_chars = _ix * num_colour_components;

	unsigned char* top = NULL;
	unsigned char* bottom = NULL;
	unsigned char temp = 0;

	for(int h=0; h<half_height_in_pixels; h++)
	{
		top = imagePtr + h * width_in_chars;
		bottom = imagePtr + (height_in_pixels - h - 1) * width_in_chars;

		for(int w=0; w<width_in_chars; w++)
		{
			//swap the chars around
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			++top;
			++bottom;
		}
	}

	//copy into an OpenGL texture
	

//	if(!sss)
//	{
		glActiveTexture(GL_TEXTURE0);
//		sss = true;
//	}
//	else
//		glActiveTexture(GL_TEXTURE1);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _ix, _iy, 0, GL_RGBA, GL_UNSIGNED_BYTE, _img);



	if(gen_mips)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	}
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


#pragma region SHADER_FUNCTIONS

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile) {   
    FILE* fp = fopen(shaderFile, "rb"); //!->Why does binary flag "RB" work and not "R"... wierd msvc thing?

    if ( fp == NULL ) { return NULL; }

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
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
       // exit(0);
    }
	const char* pShaderSource = readShaderSource( pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
    glCompileShader(ShaderObj);
    GLint success;
	// check for shader related errors using glGetShaderiv
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        //exit(1);
    }
	// Attach the compiled shader object to the program object
    glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders(const char* vertex_shader, const char* fragment_shader)
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    GLuint shaderProgramID = glCreateProgram();
    if (shaderProgramID == 0) {
        fprintf(stderr, "Error creating shader program\n");
      //  exit(1);
    }

	// Create two shader objects, one for the vertex, and one for the fragment shader
    AddShader(shaderProgramID, vertex_shader, GL_VERTEX_SHADER);
    AddShader(shaderProgramID, fragment_shader, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };
	// After compiling all shader objects and attaching them to the program, we can finally link it
    glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        //exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
    glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        //exit(1);
    }
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
    glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion 


void fountain()
{
	
	auto myEmitter = std::make_shared<ParticleEmitter>();
	myEmitter->m_emitRate = (float)num_particles*0.1f;
/*
	auto posGenerator = std::make_shared<RoundPosGen>();
//	posGenerator->m_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
//	posGenerator->m_maxStartPosOffset = glm::vec4(0.1, 0.1, 0.0, 1.0);
    posGenerator->m_centre = glm::vec4(0.0);
    posGenerator->m_radX = 0.01f;
    posGenerator->m_radY = 0.01f;
    myEmitter->addGenerator(posGenerator);
*/
	auto posGenerator = std::make_shared<MySpinningGen>();
    posGenerator->m_centre = glm::vec4(0.0);
    posGenerator->m_rad = 0.5f;
    posGenerator->m_speed = 0.06;
    myEmitter->addGenerator(posGenerator);

    auto colGenerator = std::make_shared<BasicColorGen>();
    colGenerator->m_minStartCol = glm::vec4( 0.8, 0.0, 0.0, 1.0 );
    colGenerator->m_maxStartCol = glm::vec4( 1.0, 0.0, 0.0, 1.0 );
    colGenerator->m_minEndCol = glm::vec4( 0.0, 0.8, 0.0, 1.0 );
    colGenerator->m_maxEndCol = glm::vec4( 0.0, 1.0, 0.0, 1.0 );
    myEmitter->addGenerator(colGenerator);

    auto velGenerator = std::make_shared<BasicVelGen>();
    velGenerator->m_minStartVel = glm::vec4( -0.5f, 0.4, -0.1f, 0.0f );
    velGenerator->m_maxStartVel = glm::vec4( 0.5f, 0.6, 0.1f, 0.0f );
    myEmitter->addGenerator(velGenerator);

    auto timeGenerator = std::make_shared<BasicTimeGen>();
    timeGenerator->m_minTime = 2.0;
    timeGenerator->m_maxTime = 4.0;
    myEmitter->addGenerator(timeGenerator);
	
	mySystem.addEmitter(myEmitter);





	auto timeUpdater = std::make_shared<BasicTimeUpdater>();
	mySystem.addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<BasicColorUpdater>();
	mySystem.addUpdater(colorUpdater);

	auto eulerUpdater = std::make_shared<EulerUpdater>();
	eulerUpdater->m_globalAcceleration = glm::vec4(-0, -15, -0, 0.0);
	mySystem.addUpdater(eulerUpdater);

	auto floorUpdater = std::make_shared<FloorUpdater>();
	floorUpdater->m_floorY = -0.0;
	floorUpdater->m_bounceFactor = 0.5;
	mySystem.addUpdater(floorUpdater);

	auto dragUpdater = std::make_shared<MyDragUpdater>();
	dragUpdater->m_dragCoef = 1.0f;
	mySystem.addUpdater(dragUpdater);

	auto wallUpdater = std::make_shared<MyPlaneUpdater>();
	wallUpdater->m_topLeft = glm::vec4(-0.501f, -1.0f, -1.0f, 1.0f);
	wallUpdater->m_bottomRight = glm::vec4(-0.501f, 1.0f, 1.0f, 1.0f);
	wallUpdater->m_normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	wallUpdater->m_bounceFactor = 1.0;
	mySystem.addUpdater(wallUpdater);
}

void waterfallRK4()

{
	auto myEmitter = std::make_shared<ParticleEmitter>();
	myEmitter->m_emitRate = (float)num_particles*0.05f;

	auto posGenerator = std::make_shared<BoxPosGen>();
	posGenerator->m_pos =  glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	posGenerator->m_maxStartPosOffset = glm::vec4(-0.1f, 0.001f, 0.001f, 1.0f);
    myEmitter->addGenerator(posGenerator);

    auto colGenerator = std::make_shared<BasicColorGen>();
    colGenerator->m_minStartCol = glm::vec4( 0.0, 0.0, 1.0, 1.0 );
    colGenerator->m_maxStartCol = glm::vec4( 0.0, 0.0, 1.0, 1.0 );
    colGenerator->m_minEndCol = glm::vec4( 1.0, 1.0, 1.0, 1.0 );
    colGenerator->m_maxEndCol = glm::vec4( 1.0, 1.0, 1.0, 1.0 );
    myEmitter->addGenerator(colGenerator);

    auto velGenerator = std::make_shared<BasicVelGen>();
    velGenerator->m_minStartVel = glm::vec4( 0.1f, 0.1f, 0.4, 0.0f );
    velGenerator->m_maxStartVel = glm::vec4( -0.1f, -0.1f, 0.8f, 0.0f );
    myEmitter->addGenerator(velGenerator);

    auto timeGenerator = std::make_shared<BasicTimeGen>();
    timeGenerator->m_minTime = 2.0;
    timeGenerator->m_maxTime = 4.0;
    myEmitter->addGenerator(timeGenerator);
	
	mySystem.addEmitter(myEmitter);



	auto timeUpdater = std::make_shared<BasicTimeUpdater>();
	mySystem.addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<MyColourVelUpdater>();
	colorUpdater->m_velLimit = 0.7f;
	mySystem.addUpdater(colorUpdater);

	auto rk4Updater = std::make_shared<MyRK4Updater>();
	rk4Updater->m_constantAcceleration = glm::vec4(-0, -20, -0, 0.0);
	mySystem.addUpdater(rk4Updater);

	auto floorUpdater = std::make_shared<FloorUpdater>();
	floorUpdater->m_floorY = -0.7f;
	floorUpdater->m_bounceFactor = 0.5;
	mySystem.addUpdater(floorUpdater);

	auto dragUpdater = std::make_shared<MyDragUpdater>();
	dragUpdater->m_dragCoef = 1.0f;
	mySystem.addUpdater(dragUpdater);

	auto leftBankUpdater = std::make_shared<MyPlaneUpdater>();
	leftBankUpdater->m_normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	leftBankUpdater->m_topLeft = glm::vec4(-0.1f, 1.0f, -1.0f, 1.0f);
	leftBankUpdater->m_bottomRight = glm::vec4(-0.1f, -1.0f, -0.2f, 1.0f);
	leftBankUpdater->m_bounceFactor = 0.5f;
	mySystem.addUpdater(leftBankUpdater);

	auto rightBankUpdater = std::make_shared<MyPlaneUpdater>();
	rightBankUpdater->m_normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	rightBankUpdater->m_topLeft = glm::vec4(0.1f, 1.0f, -1.0f, 1.0f);
	rightBankUpdater->m_bottomRight = glm::vec4(0.1f, -1.0f, -0.2f, 1.0f);
	rightBankUpdater->m_bounceFactor = 0.5f;
	mySystem.addUpdater(rightBankUpdater);

	

	auto riverBedUpdater = std::make_shared<MyPlaneUpdater>();
	riverBedUpdater->m_normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	riverBedUpdater->m_topLeft = glm::vec4(-1.0f, 0.0f, -1.0f, 1.0f);
	riverBedUpdater->m_bottomRight = glm::vec4(1.0f, 0.0f, -0.5f, 1.0f);
	riverBedUpdater->m_bounceFactor = 0.5f;
	mySystem.addUpdater(riverBedUpdater);

	auto secondRiverBedUpdater = std::make_shared<MyPlaneUpdater>();
	secondRiverBedUpdater->m_normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	secondRiverBedUpdater->m_topLeft = glm::vec4(-1.0f, -0.2f, -1.0f, 1.0f);
	secondRiverBedUpdater->m_bottomRight = glm::vec4(1.0f, -0.2f, 0.0f, 1.0f);
	secondRiverBedUpdater->m_bounceFactor = 0.5f;
	mySystem.addUpdater(secondRiverBedUpdater);

	auto rock1Updater = std::make_shared<MyPlaneUpdater>();
	rock1Updater->m_normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	rock1Updater->m_topLeft = glm::vec4(0.05f, -0.3f, -1.0f, 1.0f);
	rock1Updater->m_bottomRight = glm::vec4(0.0f, -0.3f, 0.15f, 1.0f);
	rock1Updater->m_bounceFactor = 0.5f;
	mySystem.addUpdater(rock1Updater);

	auto rock2Updater = std::make_shared<MyPlaneUpdater>();
	rock2Updater->m_normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	rock2Updater->m_topLeft = glm::vec4(-0.09f, -0.5f, -1.0f, 1.0f);
	rock2Updater->m_bottomRight = glm::vec4(-0.04f, -0.5f, 0.25f, 1.0f);
	rock2Updater->m_bounceFactor = 0.5f;
	mySystem.addUpdater(rock2Updater);

}


void init()
{
	shaderProgramIDparticle = CompileShaders(V_SHADER_PARTICLE, F_SHADER_PARTICLE);

	glGenTextures(1, &_tex);
	load_image_to_texture(TEXTURE_PARTICLE, _tex, true);


	waterfallRK4();

	myRenderer.generate(&mySystem, false); 
}

void display()
{
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _tex);
 
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

//	glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram (shaderProgramIDparticle);
	

	int model_mat_location = glGetUniformLocation (shaderProgramIDparticle, "matModel");
	int view_mat_location = glGetUniformLocation (shaderProgramIDparticle, "matView");
	int proj_mat_location = glGetUniformLocation (shaderProgramIDparticle, "matProjection");

	glm::mat4 persp_proj = glm::perspective(45.0f, (float)width/(float)height, 0.1f, 200.0f);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, glm::value_ptr(persp_proj));

	
	glm::mat4 view_mat = glm::lookAt(camera_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, glm::value_ptr(view_mat));

	glm::mat4 model_mat;
	glUniformMatrix4fv (model_mat_location, 1, GL_FALSE, glm::value_ptr(model_mat));

	
	myRenderer.render();


	glutSwapBuffers();
}

void updateScene()
{
	int time = glutGet(GLUT_ELAPSED_TIME);
	int delta_time = time-oldTime;
	oldTime = time;

	//time since last frame in seconds
	double elapsed_seconds = (double)delta_time/1000;

	mySystem.update(elapsed_seconds);
	mySystem.sortParticles(camera_pos);
	myRenderer.update();

	glutPostRedisplay();
}



void specialpress(int key, int x, int y)
{
	switch(key)
	{
		//left arrow
		case 100 :	
			camera_pos.x -= cam_move_speed;
		break;

		//up arrow
		case 101 :	
			camera_pos.z -= cam_move_speed;
		break;
	
		//right arrow
		case 102 :
			camera_pos.x += cam_move_speed;
		break;

		//down arrow
		case 103:
			camera_pos.z += cam_move_speed;
		break;
	}

	glutPostRedisplay(); 
}



int main(int argc, char** argv)
{

	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Particle System");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);


	glutSpecialFunc(specialpress);



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