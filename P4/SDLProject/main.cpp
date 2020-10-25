#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

SDL_Window* displayWindow;
ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;


bool gameIsRunning = true;
bool gameWon = false;
bool gameLost = false;


#define PLATFORM_COUNT 17


#define ENEMY_COUNT 3


bool start = true;
int liveCount = 0;


struct GameState {

    Entity player;

    Entity players[1];

    Entity platforms[PLATFORM_COUNT];

    Entity enemies[ENEMY_COUNT];

    Entity banners[2];
};


GameState state;


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
    displayWindow = SDL_CreateWindow("AI!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    state.player.entityType = PLAYER;
    state.player.isStatic = false;
    state.player.width = 1.0f;
    state.player.position = glm::vec3(-4, 3, 0);
    state.player.sensorLeft = glm::vec3(state.player.position.x + 0.6f, state.player.position.y - 0.6f, 0);
    state.player.sensorRight = glm::vec3(state.player.position.x - 0.6f, state.player.position.y - 0.6f, 0);
    state.player.acceleration = glm::vec3(0, -9.81f, 0);
    state.player.textures[0] = LoadTexture("playerleft.png");
    state.player.textures[1] = LoadTexture("playerright.png");
    state.player.textureID = state.player.textures[1];
    float player_vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float player_texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    std::memcpy(state.player.vertices, player_vertices, sizeof(state.player.vertices));
    std::memcpy(state.player.texCoords, player_texCoords, sizeof(state.player.texCoords));

    state.players[0] = state.player;
    
    
    GLuint enemyLeft = LoadTexture("enemyleft.png");
    GLuint enemyRight = LoadTexture("enemyright.png");
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].isStatic = false;
        state.enemies[i].width = 1.0f;
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
        state.enemies[i].textures[0] = enemyLeft;
        state.enemies[i].textures[1] = enemyRight;
        float enemy_vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float enemy_texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
        std::memcpy(state.enemies[i].vertices, enemy_vertices, sizeof(state.enemies[i].vertices));
        std::memcpy(state.enemies[i].texCoords, enemy_texCoords, sizeof(state.enemies[i].texCoords));
    }

    state.enemies[0].position = glm::vec3(4, 4, 0);
    state.enemies[1].position = glm::vec3(1, 1, 0);
    state.enemies[2].position = glm::vec3(0, 1, 0);

    state.enemies[0].entityState = STILL;
    state.enemies[1].entityState = WALKING;
    state.enemies[2].entityState = WALKING;

    state.enemies[0].entityDir = LEFT;
    state.enemies[1].entityDir = LEFT;
    state.enemies[2].entityDir = RIGHT;

    state.enemies[0].textureID = state.enemies[0].textures[0];
    state.enemies[1].textureID = state.enemies[1].textures[0];
    state.enemies[2].textureID = state.enemies[2].textures[1];
    

    GLuint groundTextureID = LoadTexture("ground.png");
    GLuint airTextureID = LoadTexture("ground.png");
    

    float platform_vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float platform_texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    for (int i = 0; i < 10; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = groundTextureID;
        state.platforms[i].position = glm::vec3(i - 4.5f, -3.7f, 0);
        std::memcpy(state.platforms[i].vertices, platform_vertices, sizeof(state.platforms[i].vertices));
        std::memcpy(state.platforms[i].texCoords, platform_texCoords, sizeof(state.platforms[i].texCoords));
    }
    

    state.platforms[10].entityType = PLATFORM;
    state.platforms[10].textureID = airTextureID;
    state.platforms[10].position = glm::vec3(-4.5f, 1.0f, 0);
    std::memcpy(state.platforms[10].vertices, platform_vertices, sizeof(state.platforms[0].vertices));
    std::memcpy(state.platforms[10].texCoords, platform_texCoords, sizeof(state.platforms[0].texCoords));
    
    state.platforms[11].entityType = PLATFORM;
    state.platforms[11].textureID = airTextureID;
    state.platforms[11].position = glm::vec3(-3.5f, 1.0f, 0);
    std::memcpy(state.platforms[11].vertices, platform_vertices, sizeof(state.platforms[0].vertices));
    std::memcpy(state.platforms[11].texCoords, platform_texCoords, sizeof(state.platforms[0].texCoords));
    
    state.platforms[12].entityType = PLATFORM;
    state.platforms[12].textureID = airTextureID;
    state.platforms[12].position = glm::vec3(-2.5f, 1.0f, 0);
    std::memcpy(state.platforms[12].vertices, platform_vertices, sizeof(state.platforms[0].vertices));
    std::memcpy(state.platforms[12].texCoords, platform_texCoords, sizeof(state.platforms[0].texCoords));

    state.platforms[13].entityType = PLATFORM;
    state.platforms[13].textureID = airTextureID;
    state.platforms[13].position = glm::vec3(-1.5f, 1.0f, 0);
    std::memcpy(state.platforms[13].vertices, platform_vertices, sizeof(state.platforms[0].vertices));
    std::memcpy(state.platforms[13].texCoords, platform_texCoords, sizeof(state.platforms[0].texCoords));
    
    state.platforms[14].entityType = PLATFORM;
    state.platforms[14].textureID = airTextureID;
    state.platforms[14].position = glm::vec3(2.5f, -0.50f, 0);
    std::memcpy(state.platforms[14].vertices, platform_vertices, sizeof(state.platforms[0].vertices));
    std::memcpy(state.platforms[14].texCoords, platform_texCoords, sizeof(state.platforms[0].texCoords));
    
    state.platforms[15].entityType = PLATFORM;
    state.platforms[15].textureID = airTextureID;
    state.platforms[15].position = glm::vec3(3.5f, -0.50f, 0);
    std::memcpy(state.platforms[15].vertices, platform_vertices, sizeof(state.platforms[0].vertices));
    std::memcpy(state.platforms[15].texCoords, platform_texCoords, sizeof(state.platforms[0].texCoords));
    
    state.platforms[16].entityType = PLATFORM;
    state.platforms[16].textureID = airTextureID;
    state.platforms[16].position = glm::vec3(4.5f, -0.50f, 0);
    std::memcpy(state.platforms[16].vertices, platform_vertices, sizeof(state.platforms[0].vertices));
    std::memcpy(state.platforms[16].texCoords, platform_texCoords, sizeof(state.platforms[0].texCoords));
    

    state.banners[0].isStatic = true;

    state.banners[0].textureID = LoadTexture("win.png");
    float banner1_vertices[]  = { -2.5, -0.25, 2.5, -0.25, 2.5, 0.25, -2.5, -0.25, 2.5, 0.25, -2.5, 0.25 };
    float banner1_texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    std::memcpy(state.banners[0].vertices, banner1_vertices, sizeof(state.banners[0].vertices));
    std::memcpy(state.banners[0].texCoords, banner1_texCoords, sizeof(state.banners[0].texCoords));
    
    

    state.banners[1].isStatic = true;

    state.banners[1].textureID = LoadTexture("lost.png");
    float banner2_vertices[]  = { -2.5, -0.25, 2.5, -0.25, 2.5, 0.25, -2.5, -0.25, 2.5, 0.25, -2.5, 0.25 };
    float banner2_texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    std::memcpy(state.banners[1].vertices, banner2_vertices, sizeof(state.banners[1].vertices));
    std::memcpy(state.banners[1].texCoords, banner2_texCoords, sizeof(state.banners[1].texCoords));


    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    

    glUseProgram(program.programID);
    

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    glClearColor(0.0f, 0.8f, 1.0f, 1.0f);
}


void ProcessInput() {
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
                        state.player.Jump(5.5f);
                        break;
                        
                }
                break;
        }
    }
    
    state.player.velocity.x = 0;
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_A]) {
        state.player.velocity.x = -3.0f;
        state.player.entityDir = LEFT;
    }
    else if (keys[SDL_SCANCODE_D]) {
        state.player.velocity.x = 3.0f;
        state.player.entityDir = RIGHT;
    }
}


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        
        if (state.player.entityState == DEAD) {
            gameLost = true;
        } else if (!start and liveCount == 0) {
            gameWon = true;
        }
        
        state.player.Update(FIXED_TIMESTEP, state.enemies, ENEMY_COUNT);
        
        state.player.Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        
        for (int i = 0; i < ENEMY_COUNT; i++) {

            if (state.enemies[i].entityState != DEAD) {
                state.enemies[i].Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
            }
            
            state.enemies[i].startWalk();
            state.enemies[i].startJump();

            if (state.enemies[i].sensorLeftCol and !state.enemies[i].sensorRightCol) {
                state.enemies[i].entityDir = LEFT;
                state.enemies[i].textureID = state.enemies[i].textures[0];
            } else if (!state.enemies[i].sensorLeftCol and state.enemies[i].sensorRightCol) {
                state.enemies[i].entityDir = RIGHT;
                state.enemies[i].textureID = state.enemies[i].textures[1];
            }
        }
        
        if (state.player.entityDir == LEFT) {
            state.player.textureID = state.player.textures[0];
        } else if (state.player.entityDir == RIGHT) {
            state.player.textureID = state.player.textures[1];
        }
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (gameWon){

        state.banners[0].Render(&program);
    } else if (gameLost) {

        state.banners[1].Render(&program);
    } else {

        state.player.Render(&program);

        liveCount = 0;
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (state.enemies[i].entityState != DEAD) {
                start = false;
                liveCount++;
                state.enemies[i].Render(&program);
            }
        }
        
        for (int i = 0; i < PLATFORM_COUNT; i++) {
            state.platforms[i].Render(&program);
        }
    }

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
