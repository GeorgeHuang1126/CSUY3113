#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool startGame = false;
bool endGame = false;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix_player1, modelMatrix_player2, modelMatrix_pong, projectionMatrix, modelMatrix;
glm::vec3 player1_position, player1_movement, player2_position, player2_movement, pong_position, pong_movement;


float player1_x = 0;
float player2_x = 0;
float pong_x = 0;
float player1_y = 0;
float player2_y = 0;
float pong_y = 0;
float x_dir = -1.0;
float y_dir = 1.0;
float player1_xdist, player1_ydist, player2_xdist, player2_ydist, x_1, x_2, y_1, y_2, px, py;

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Triangle!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

	viewMatrix = glm::mat4(1.0f);
	modelMatrix_player1 = glm::mat4(1.0f);
	modelMatrix_player2 = glm::mat4(1.0f);
	modelMatrix_pong = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);


	x_1 = -4.75f;
	x_2 = 4.75f;
	y_2 = 0.0f;
	y_1 = 0.0f;
	px = 0.0f;
	py = 0.0f;

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(1.0f, 1.0f, 0.0f, 1.0f);

	glUseProgram(program.programID);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}



void ProcessInput() {
	player1_movement = glm::vec3(0);
	player2_movement = glm::vec3(0);
	pong_movement = glm::vec3(0);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {

			case SDLK_SPACE:
				startGame = true;
			}
		}
	}
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_UP]) {
		if (endGame == false) {
			player2_movement.y = 1.0f;
		}
	}
	else if (keys[SDL_SCANCODE_DOWN]) {
		if (endGame == false) {
			player2_movement.y = -1.0f;
		}
	}
	if (glm::length(player2_movement) > 1.0f) {
		player2_movement = glm::normalize(player2_movement);
	}
	if (keys[SDL_SCANCODE_W]) {
		if (endGame == false) {
			player1_movement.y = 1.0f;
		}
	}
	else if (keys[SDL_SCANCODE_S]) {
		if (endGame == false) {
			player1_movement.y = -1.0f;
		}
	}
	if (glm::length(player1_movement) > 1.0f) {
		player1_movement = glm::normalize(player1_movement);
	}
	if (glm::length(player2_movement) > 1.0f) {
		player2_movement = glm::normalize(player2_movement);
	}

}

void pongCollision() {
	y_1 = player1_position.y;
	y_2 = player2_position.y;

	player1_xdist = fabs(x_1 - px) - (0.5f);
	player1_ydist = fabs(y_1 - py) - ((2.5f) / 2.0f);
	player2_xdist = fabs(x_2 - px) - (0.5f);
	player2_ydist = fabs(y_2 - py) - ((2.5f) / 2.0f);
	if ((player1_xdist < 0 && player1_ydist < 0) || (player2_xdist < 0 && player2_ydist < 0)) {
		x_dir = x_dir * (-1.0);
		y_dir = y_dir * (-1.0);
		pong_movement.x = +((0.8f) * (x_dir));
		pong_movement.y = +((0.8f) * (y_dir));
	}

	if (pong_position.x < -4.75 || pong_position.x > 4.75) {
		startGame = false;
		endGame = true;
	}

	if (pong_position.y > 3.45 || pong_position.y < -3.45) {
		y_dir = y_dir * (-1.0);
	}

}

float lastTicks = 0.0f;
void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	player1_position += player1_movement * 2.0f * deltaTime;
	player2_position += player2_movement * 2.0f * deltaTime;

	if (player1_position.y<2.75f && player1_position.y>-2.75f) {
		modelMatrix_player1 = glm::mat4(1.0f);
		modelMatrix_player1 = glm::translate(modelMatrix_player1, player1_position);
	}
	else {
		player1_position -= player1_movement * 2.0f * deltaTime;
	}
	if (player2_position.y<2.75f && player2_position.y>-2.75f) {
		modelMatrix_player2 = glm::mat4(1.0f);
		modelMatrix_player2 = glm::translate(modelMatrix_player2, player2_position);
	}
	else {
		player2_position -= player2_movement * 2.0f * deltaTime;
	}

	if (startGame) {
		pongCollision();
		pong_movement.x = +((1.0f) * (x_dir));
		pong_movement.y = +((1.0f) * (y_dir));

		pong_position += pong_movement * 2.0f * deltaTime;
		px = pong_position.x;
		py = pong_position.y;

		if (pong_position.y<4.0f && pong_position.y>-4.0f) {
			modelMatrix_pong = glm::mat4(1.0f);
			modelMatrix_pong = glm::translate(modelMatrix_pong, pong_position);
		}
		else {
			pong_position -= pong_movement * 2.0f * deltaTime;
		}

	}
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	program.SetModelMatrix(modelMatrix_pong);

	float vertices[] = { -0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25 };
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDisableVertexAttribArray(program.positionAttribute);

	program.SetModelMatrix(modelMatrix_player1);

	float verticesp1[] = { -5.0,-1.0,-5.0,1.0,-4.5,-1.0,-5.0,1.0,-4.5,1.0,-4.5,-1.0 };
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesp1);
	glEnableVertexAttribArray(program.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDisableVertexAttribArray(program.positionAttribute);

	program.SetModelMatrix(modelMatrix_player2);

	float verticesp2[] = { 4.5,-1.0, 4.5,1.0, 5.0,-1.0, 4.5,1.0, 5.0,1.0, 5.0,-1.0 };
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesp2);
	glEnableVertexAttribArray(program.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);

	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}

	Shutdown();
	return 0;
}