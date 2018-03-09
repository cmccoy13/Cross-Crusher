/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <iostream>
#include <glad/glad.h>
#include <cstdlib>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include <math.h>
#include "vector"

#include "WindowManager.h"
#include "Shape.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

shared_ptr<Shape> shape;
static float xVel1 = 0.001;
static float yVel1 = 0.001;
static float xBall1 = 0.0;
static float yBall1 = 0.34;


static float xVel2 = -0.001;
static float yVel2 = 0.001;
static float xBall2 = -0.34;
static float yBall2 = 0.0;


static float xVel3 = -0.001;
static float yVel3 = -0.001;
static float xBall3 = 0.0;
static float yBall3 = -0.34;


static float xVel4 = 0.001;
static float yVel4 = -0.001;
static float xBall4 = 0.34;
static float yBall4 = 0.0;


vec4 ball1 = vec4(xVel1, yVel1, xBall1, yBall1);
vec4 ball2 = vec4(xVel2, yVel2, xBall2, yBall2);
vec4 ball3 = vec4(xVel3, yVel3, xBall3, yBall3);
vec4 ball4 = vec4(xVel4, yVel4, xBall4, yBall4);

std::vector<vec4> balls;


class Controls
{
public:
	int w, a, s, d, up, down, left, right;
	Controls()
	{
		w = a = s = d = up = down = left = right = 0;
	}
};

Controls controls;

class Matrix
{
public:
	float data[4][4];
	void createIdentityMatrix();
	void createTranslateMat(float x, float y, float z);
	void createScaleMat(float x, float y, float z);
	void createRotationMatX(float rot);
	void createRotationMatY(float rot);
	void createRotationMatZ(float rot);
	void multMat(Matrix A, Matrix B);
};

void Matrix::createIdentityMatrix()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			data[i][j] = 0.0;
		}		
	}

	data[0][0] = 1.0;
	data[1][1] = 1.0;
	data[2][2] = 1.0;
	data[3][3] = 1.0;
}

void Matrix::createTranslateMat(float x, float y, float z)
{
	createIdentityMatrix();

	data[3][0] = x;
	data[3][1] = y;
	data[3][2] = z;
}

void Matrix::createScaleMat(float x, float y, float z)
{
	createIdentityMatrix();

	data[0][0] = x;
	data[1][1] = y;
	data[2][2] = z;
}

void Matrix::createRotationMatX(float rot)
{
	createIdentityMatrix();

	data[1][1] = cos(rot);
	data[1][2] = -sin(rot);
	data[2][1] = sin(rot);
	data[2][2] = cos(rot);
}

void Matrix::createRotationMatY(float rot)
{
	createIdentityMatrix();

	data[0][0] = cos(rot);
	data[0][2] = sin(rot);
	data[2][0] = -sin(rot);
	data[2][2] = cos(rot);
}

void Matrix::createRotationMatZ(float rot)
{
	createIdentityMatrix();

	data[0][0] = cos(rot);
	data[0][1] = -sin(rot);
	data[1][0] = sin(rot);
	data[1][1] = cos(rot);
}

void Matrix::multMat(Matrix A, Matrix B)
{
	createIdentityMatrix();

	for (int k = 0; k < 4; k++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int i = 0; i < 4; i++)
			{
				data[j][k] += (A.data[i][k]) * (B.data[j][i]);
			}
		}
	}
}

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> courtProg;
	std::shared_ptr<Program> brickProg;
	std::shared_ptr<Program> paddleProg;
	std::shared_ptr<Program> floorProg;
	std::shared_ptr<Program> ballProg;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexColorIDBox, IndexBufferIDBox;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			controls.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			controls.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			controls.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			controls.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			controls.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			controls.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			controls.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			controls.d = 0;
		}
		if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		{
			controls.up = 1;
			controls.down = 0;
			controls.left = 0;
			controls.right = 0;
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		{
			controls.down = 1;
			controls.up = 0;
			controls.left = 0;
			controls.right = 0;
		}
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		{
			controls.left = 1;
			controls.up = 0;
			controls.down = 0;
			controls.right = 0;
		}
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		{
			controls.right = 1;
			controls.up = 0;
			controls.down = 0;
			controls.left = 0;
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{

		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		//SPHERE
		string resourceDirectory = "../resources";
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->resize();
		shape->init();
		//END SPHERE

		balls.push_back(ball1);
		balls.push_back(ball2);
		balls.push_back(ball3);
		balls.push_back(ball4);


		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		GLfloat cube_vertices[] = {
			// front
			-1.0, -1.0,  1.0,
			1.0, -1.0,  1.0,
			1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			// back
			-1.0, -1.0, -1.0,
			1.0, -1.0, -1.0,
			1.0,  1.0, -1.0,
			-1.0,  1.0, -1.0,
		};
		//make it a bit smaller
		for (int i = 0; i < 24; i++)
			cube_vertices[i] *= 0.5;
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

		//color
		GLfloat cube_colors[] = {
			// front colors
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 1.0,
			1.0, 1.0, 1.0,
			// back colors
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 1.0,
			1.0, 1.0, 1.0,
		};
		glGenBuffers(1, &VertexColorIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexColorIDBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		GLushort cube_elements[] = {
			// front
			0, 1, 2,
			2, 3, 0,
			// top
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// bottom
			4, 0, 3,
			3, 7, 4,
			// left
			4, 5, 1,
			1, 0, 4,
			// right
			3, 2, 6,
			6, 7, 3,
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		courtProg = std::make_shared<Program>();
		courtProg->setVerbose(true);
		courtProg->setShaderNames(resourceDirectory + "/shader_vertex_court.glsl", resourceDirectory + "/shader_fragment_court.glsl");
		courtProg->init();
		courtProg->addUniform("P");
		courtProg->addUniform("V");
		courtProg->addUniform("M");
		courtProg->addAttribute("vertPos");
		courtProg->addAttribute("vertColor");

		brickProg = std::make_shared<Program>();
		brickProg->setVerbose(true);
		brickProg->setShaderNames(resourceDirectory + "/shader_vertex_brick.glsl", resourceDirectory + "/shader_fragment_brick.glsl");
		brickProg->init();
		brickProg->addUniform("P");
		brickProg->addUniform("V");
		brickProg->addUniform("M");
		brickProg->addAttribute("vertPos");
		brickProg->addAttribute("vertColor");

		paddleProg = std::make_shared<Program>();
		paddleProg->setVerbose(true);
		paddleProg->setShaderNames(resourceDirectory + "/shader_vertex_paddle.glsl", resourceDirectory + "/shader_fragment_paddle.glsl");
		paddleProg->init();
		paddleProg->addUniform("P");
		paddleProg->addUniform("V");
		paddleProg->addUniform("M");
		paddleProg->addAttribute("vertPos");
		paddleProg->addAttribute("vertColor");

		floorProg = std::make_shared<Program>();
		floorProg->setVerbose(true);
		floorProg->setShaderNames(resourceDirectory + "/shader_vertex_floor.glsl", resourceDirectory + "/shader_fragment_floor.glsl");
		floorProg->init();
		floorProg->addUniform("P");
		floorProg->addUniform("V");
		floorProg->addUniform("M");
		floorProg->addAttribute("vertPos");
		floorProg->addAttribute("vertColor");

		ballProg = std::make_shared<Program>();
		ballProg->setVerbose(true);
		ballProg->setShaderNames(resourceDirectory + "/shader_vertex_ball.glsl", resourceDirectory + "/shader_fragment_ball.glsl");
		ballProg->init();
		ballProg->addUniform("P");
		ballProg->addUniform("V");
		ballProg->addUniform("M");
		ballProg->addAttribute("vertPos");
		ballProg->addAttribute("vertNor");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void drawCourt(glm::mat4 P, glm::mat4 V, glm::mat4 M)
	{
		float w = 3.141592769 / 2;

		courtProg->bind();

		glm::mat4 Rotate;
		glm::mat4 Scale;
		glm::mat4 Trans;
		glm::mat4 TransView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3));

		glUniformMatrix4fv(courtProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);

		glBindVertexArray(VertexArrayID);

		//TOP COURT
		//Top
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.1f, 0.2f));
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.1f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//L
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.35f, 0.75f, 0.0f));
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//R
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.35f, 0.75f, 0.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);



		//BOTTOM COURT
		//Bot
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.1f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//L
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.35f, -0.75f, 0.0f));
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//R
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.35f, -0.75f, 0.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);



		//LEFT Court
		//L
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-1.1f, 0.0f, 0.0f));
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//Top
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.7f, 0.35f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//Bot
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.7f, -0.35f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);



		//RIGHT COURT
		//R
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(1.1f, 0.0f, 0.0f));
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//Top
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, 0.35f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//Bot
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, -0.35f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		courtProg->unbind();



		floorProg->bind();

		glUniformMatrix4fv(courtProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);

		glBindVertexArray(VertexArrayID);

		//TOP FLOOR
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.90f, 0.01f));
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.764f, -0.2f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//BOTTOM FLOOR
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.764f, -0.2f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//LEFT FLOOR
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.764f, 0.0f, -0.2f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//RIGHT FLOOR
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.764f, 0.0f, -0.2f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		floorProg->unbind();
	}

	void processCollisions(float upad, float dpad, float lpad, float rpad)
	{
		for (int i = 0; i < 4; i++)
		{
			balls[i][2] += balls[i][0];
			balls[i][3] += balls[i][1];

			//TOP SECTION
			if (balls[i][3] > 0.31)
			{
				if (balls[i][2] > 0.28)
				{
					balls[i][0] *= -1;
				}
				else if (balls[i][2] < -0.28)
				{
					balls[i][0] *= -1;
				}
				if (balls[i][3] > 1.03)
				{
					balls[i][1] *= -1;
				}

				//PADDLE
				if (balls[i][3] < 0.33)
				{
					if (balls[i][2] < upad + .1  && balls[i][2] > upad - .1)
					{
						balls[i][1] *= -1;
					}
				}
			}

			//BOT SECTION
			else if (balls[i][3] < -0.31)
			{
				if (balls[i][2] > 0.28)
				{
					balls[i][0] *= -1;
				}
				else if (balls[i][2] < -0.28)
				{
					balls[i][0] *= -1;
				}
				if (balls[i][3] < -1.03)
				{
					balls[i][1] *= -1;
				}

				//PADDLE
				if (balls[i][3] > -0.33)
				{
					if (balls[i][2] < dpad + .1  && balls[i][2] > dpad - .1)
					{
						balls[i][1] *= -1;
					}
				}
			}

			//LEFT SECTION
			else if (balls[i][2] < -0.31)
			{
				if (balls[i][3] > 0.28)
				{
					balls[i][1] *= -1;
				}
				else if (balls[i][3] < -0.28)
				{
					balls[i][1] *= -1;
				}
				if (balls[i][2] < -1.03)
				{
					balls[i][0] *= -1;
				}

				//PADDLE
				if (balls[i][2] > -0.33)
				{
					if (balls[i][3] < lpad + .1  && balls[i][3] > lpad - .1)
					{
						balls[i][0] *= -1;
					}
				}
			}

			//RIGHT SECTION
			else if (balls[i][2] > 0.31)
			{
				if (balls[i][3] > 0.28)
				{
					balls[i][1] *= -1;
				}
				else if (balls[i][3] < -0.28)
				{
					balls[i][1] *= -1;
				}
				if (balls[i][2] > 1.03)
				{
					balls[i][0] *= -1;
				}

				if (balls[i][2] < 0.33)
				{
					if (balls[i][3] < rpad + .1  && balls[i][3] > rpad - .1)
					{
						balls[i][0] *= -1;
					}
				}
			}
		}
	}

	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = glm::mat4(1);
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);		
		if (width < height)
			{
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect,  1.0f / aspect, -2.0f, 100.0f);
			}
		// ...but we overwrite it (optional) with a perspective projection.
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		//animation with the model matrix:
		static float w = 0.0;
		static float lpad = 0.0;
		static float rpad = 0.0;
		static float upad = 0.0;
		static float dpad = 0.0;
		float paddleSpeed = 0.004;
		float paddleDist = 0.2;

		if (controls.up == 1)
		{
			if (controls.a == 1 && upad > -paddleDist)
			{
				upad -= paddleSpeed;
			}
			if (controls.d == 1 && upad < paddleDist)
			{
				upad += paddleSpeed;
			}
		}
		if (controls.down == 1)
		{
			if (controls.a == 1 && dpad > -paddleDist)
			{
				dpad -= paddleSpeed;
			}
			if (controls.d == 1 && dpad < paddleDist)
			{
				dpad += paddleSpeed;
			}
		}
		if (controls.left == 1)
		{
			if (controls.w == 1 && lpad < paddleDist)
			{
				lpad += paddleSpeed;
			}
			if (controls.s == 1 && lpad > -paddleDist)
			{
				lpad -= paddleSpeed;
			}
		}
		if (controls.right == 1)
		{
			if (controls.w == 1 && rpad < paddleDist)
			{
				rpad += paddleSpeed;
			}
			if (controls.s == 1 && rpad > -paddleDist)
			{
				rpad -= paddleSpeed;
			}
		}

		processCollisions(upad, dpad, lpad, rpad);
		

		w = 3.141592769 / 2;

		glm::mat4 Rotate;
		glm::mat4 Scale;
		glm::mat4 Trans;
		glm::mat4 TransView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3));

		V = TransView*V;


		drawCourt(P, V, M);	



		paddleProg->bind();

		glUniformMatrix4fv(courtProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);

		//PADDLES
		//Top
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.17f, 0.03f, 0.1f));
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(upad, 0.3f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		
		//Bot
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(dpad, -0.3f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//L
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f, lpad, 0.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//R
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, rpad, 0.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		paddleProg->unbind();



		brickProg->bind();

		glUniformMatrix4fv(courtProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);

		//BRICKS
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.10f, 0.04f, 0.05f));

		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				//TOP
				Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.25f + (.1*i), 0.75f + (.045*j), 0.0f));
				M = Trans*Scale;
				glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

				//BOTTOM
				Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.25f + (.1*i), -0.75f - (.045*j), 0.0f));
				M = Trans*Scale;
				glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

				//LEFT
				Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
				Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-.75f - (0.045*j), -.25f + (.1*i), 0.0f));				
				M = Trans*Rotate*Scale;
				glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

				//RIGHT
				Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
				Trans = glm::translate(glm::mat4(1.0f), glm::vec3(.75f + (0.045*j), .25f - (.1*i), 0.0f));
				M = Trans*Rotate*Scale;
				glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
			}
		}

		glBindVertexArray(0);

		brickProg->unbind();



		ballProg->bind();

		glUniformMatrix4fv(courtProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);

		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(balls[0][2], balls[0][3], 0.0f));
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
		M = Trans * Scale;
		glUniformMatrix4fv(ballProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape->draw(ballProg);

		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(balls[1][2], balls[1][3], 0.0f));
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
		M = Trans * Scale;
		glUniformMatrix4fv(ballProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape->draw(ballProg);

		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(balls[2][2], balls[2][3], 0.0f));
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
		M = Trans * Scale;
		glUniformMatrix4fv(ballProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape->draw(ballProg);

		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(balls[3][2], balls[3][3], 0.0f));
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
		M = Trans * Scale;
		glUniformMatrix4fv(ballProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape->draw(ballProg);

		ballProg->unbind();

	}
};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1500, 1000);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
