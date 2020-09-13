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

ShaderProgram program;
glm::mat4 viewMatrix, gun1Matrix, gun2Matrix, gun3Matrix, apexMatrix, twenty1Matrix, twenty2Matrix, twenty3Matrix, twenty4Matrix, twenty5Matrix, bullet1Matrix, bullet2Matrix, bullet3Matrix, projectionMatrix;

float bullet1_x = 0;
float bullet2_x = 0;
float gun_rotate = 0;
float apex_x = 0;
float twenty1_x = 0;
float twenty2_x = 0;
float twenty3_x = 0;
float twenty4_x = 0;
float twenty5_x = 0;
float apex_rotate = 0;

GLuint twenty1TextureID;
GLuint twenty2TextureID;
GLuint twenty3TextureID;
GLuint twenty4TextureID;
GLuint twenty5TextureID;
GLuint bullet1TextureID;
GLuint bullet2TextureID;
GLuint bullet3TextureID;
GLuint gun1TextureID;
GLuint gun2TextureID;
GLuint gun3TextureID;
GLuint apexTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    gun1Matrix = glm::mat4(1.0f);
    bullet1Matrix = glm::mat4(1.0f);
    gun2Matrix = glm::mat4(1.0f);
    bullet2Matrix = glm::mat4(1.0f);
    gun3Matrix = glm::mat4(1.0f);
    bullet3Matrix = glm::mat4(1.0f);
    apexMatrix = glm::mat4(1.0f);
    twenty1Matrix = glm::mat4(1.0f);
    twenty2Matrix = glm::mat4(1.0f);
    twenty3Matrix = glm::mat4(1.0f);
    twenty4Matrix = glm::mat4(1.0f);
    twenty5Matrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.4f, 0.4f, 0.4f, 0.0f);


    bullet1TextureID = bullet2TextureID = bullet3TextureID = LoadTexture("bullet.png");
    gun1TextureID = gun2TextureID = gun3TextureID = LoadTexture("gun.png");
    apexTextureID = LoadTexture("apex.png");
    twenty1TextureID = twenty2TextureID = twenty3TextureID = twenty4TextureID = twenty5TextureID = LoadTexture("20.png");

    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    bullet1_x -= 1.3f * deltaTime;
    bullet2_x += 1.3f * deltaTime;
    gun_rotate += 51.5f * deltaTime;
    twenty1_x += 0.9f * deltaTime;
    twenty2_x += 0.5f * deltaTime;
    twenty3_x += 0.7f * deltaTime;
    twenty4_x += 0.6f * deltaTime;
    twenty5_x += 0.8f * deltaTime;
    apex_rotate -= 30.0f * deltaTime;

    apexMatrix = glm::mat4(1.0f);
    apexMatrix = glm::translate(apexMatrix, glm::vec3(-0.0f, 3.2f, 0.0f));
    apexMatrix = glm::scale(apexMatrix, glm::vec3(1.5f, 1.5f, 1.0f));

    twenty1Matrix = glm::mat4(1.0f);
    twenty1Matrix = glm::translate(twenty1Matrix, glm::vec3(0.0f, 2.3f, 0.0f));
    twenty1Matrix = glm::scale(twenty1Matrix, glm::vec3(1.2f, 1.2f, 1.0f));
    twenty1Matrix = glm::translate(twenty1Matrix, glm::vec3(0.0f, -twenty1_x, 0.0f));
    if (twenty1_x > 5.5f) {
        twenty1_x = 0.0f;
    }
    twenty2Matrix = glm::mat4(1.0f);
    twenty2Matrix = glm::translate(twenty2Matrix, glm::vec3(1.1f, 2.4f, 0.0f));
    twenty2Matrix = glm::scale(twenty2Matrix, glm::vec3(1.2f, 1.2f, 1.0f));
    twenty2Matrix = glm::translate(twenty2Matrix, glm::vec3(0.0f, -twenty2_x, 0.0f));
    if (twenty2_x > 5.5f) {
        twenty2_x = 0.0f;
    }
    twenty3Matrix = glm::mat4(1.0f);
    twenty3Matrix = glm::translate(twenty3Matrix, glm::vec3(-1.1f, 2.3f, 0.0f));
    twenty3Matrix = glm::scale(twenty3Matrix, glm::vec3(1.2f, 1.2f, 1.0f));
    twenty3Matrix = glm::translate(twenty3Matrix, glm::vec3(0.0f, -twenty3_x, 0.0f));
    if (twenty3_x > 5.5f) {
        twenty3_x = 0.0f;
    }
    twenty4Matrix = glm::mat4(1.0f);
    twenty4Matrix = glm::translate(twenty4Matrix, glm::vec3(2.2f, 2.3f, 0.0f));
    twenty4Matrix = glm::scale(twenty4Matrix, glm::vec3(1.2f, 1.2f, 1.0f));
    twenty4Matrix = glm::translate(twenty4Matrix, glm::vec3(0.0f, -twenty4_x, 0.0f));
    if (twenty4_x > 5.5f) {
        twenty4_x = 0.0f;
    }
    twenty5Matrix = glm::mat4(1.0f);
    twenty5Matrix = glm::translate(twenty5Matrix, glm::vec3(-2.2f, 2.3f, 0.0f));
    twenty5Matrix = glm::scale(twenty5Matrix, glm::vec3(1.2f, 1.2f, 1.0f));
    twenty5Matrix = glm::translate(twenty5Matrix, glm::vec3(0.0f, -twenty5_x, 0.0f));
    if (twenty5_x > 5.5f) {
        twenty5_x = 0.0f;
    }

    gun1Matrix = glm::mat4(1.0f);
    gun1Matrix = glm::translate(gun1Matrix, glm::vec3(4.0f, -0.6f, 0.0f));
    gun1Matrix = glm::rotate(gun1Matrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    gun1Matrix = glm::rotate(gun1Matrix, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    gun1Matrix = glm::rotate(gun1Matrix, glm::radians(gun_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    gun1Matrix = glm::scale(gun1Matrix, glm::vec3(1.1f, 1.1f, 1.0f));

    bullet1Matrix = glm::mat4(1.0f);
    bullet1Matrix = glm::translate(bullet1Matrix, glm::vec3(3.3f, -0.5f, 0.0f));
    bullet1Matrix = glm::translate(bullet1Matrix, glm::vec3(bullet1_x, 0.0f, 0.0f));
    if (bullet1_x < -8.8f) {
        bullet1_x = 0.2f;
    }
    bullet1Matrix = glm::scale(bullet1Matrix, glm::vec3(3.0f, 3.0f, 1.0f));

    gun2Matrix = glm::mat4(1.0f);
    gun2Matrix = glm::translate(gun2Matrix, glm::vec3(-4.0f, 1.39f, 0.0f));
    gun2Matrix = glm::rotate(gun2Matrix, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    gun2Matrix = glm::rotate(gun2Matrix, glm::radians(gun_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    gun2Matrix = glm::scale(gun2Matrix, glm::vec3(1.1f, 1.1f, 1.0f));

    bullet2Matrix = glm::mat4(1.0f);
    bullet2Matrix = glm::translate(bullet2Matrix, glm::vec3(-3.1f, 1.6f, 0.0f));
    bullet2Matrix = glm::translate(bullet2Matrix, glm::vec3(bullet2_x, 0.0f, 0.0f));
    if (bullet2_x > 8.8f) {
        bullet2_x = -0.2f;
    }
    bullet2Matrix = glm::scale(bullet2Matrix, glm::vec3(3.0f, 3.0f, 1.0f));

    gun3Matrix = glm::mat4(1.0f);
    gun3Matrix = glm::translate(gun3Matrix, glm::vec3(-4.0f, -2.4f, 0.0f));
    gun3Matrix = glm::rotate(gun3Matrix, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    gun3Matrix = glm::rotate(gun3Matrix, glm::radians(gun_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    gun3Matrix = glm::scale(gun3Matrix, glm::vec3(1.1f, 1.1f, 1.0f));

    bullet3Matrix = glm::mat4(1.0f);
    bullet3Matrix = glm::translate(bullet3Matrix, glm::vec3(-3.1f, -2.2f, 0.0f));
    bullet3Matrix = glm::translate(bullet3Matrix, glm::vec3(bullet2_x, 0.0f, 0.0f));
    bullet3Matrix = glm::scale(bullet3Matrix, glm::vec3(3.0f, 3.0f, 1.0f));

}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(twenty1Matrix);
    glBindTexture(GL_TEXTURE_2D, twenty1TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(twenty2Matrix);
    glBindTexture(GL_TEXTURE_2D, twenty2TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(twenty3Matrix);
    glBindTexture(GL_TEXTURE_2D, twenty3TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(twenty4Matrix);
    glBindTexture(GL_TEXTURE_2D, twenty4TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(twenty5Matrix);
    glBindTexture(GL_TEXTURE_2D, twenty5TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(bullet1Matrix);
    glBindTexture(GL_TEXTURE_2D, bullet1TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(bullet2Matrix);
    glBindTexture(GL_TEXTURE_2D, bullet2TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(bullet3Matrix);
    glBindTexture(GL_TEXTURE_2D, bullet3TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(gun1Matrix);
    glBindTexture(GL_TEXTURE_2D, gun1TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(gun2Matrix);
    glBindTexture(GL_TEXTURE_2D, gun2TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(gun3Matrix);
    glBindTexture(GL_TEXTURE_2D, gun3TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(apexMatrix);
    glBindTexture(GL_TEXTURE_2D, apexTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
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