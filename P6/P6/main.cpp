#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <vector>

#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Level1.h"

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix, terrainMatrix, leftMatrix;

SDL_Window* displayWindow;
bool gameIsRunning = true;

Scene* currentScene;
Scene* sceneList[4];

Mix_Music* music;
Mix_Chunk* attacksound;

GLuint font;

bool inGameScreen = true;
int lives = 1;

void SwitchToScene(Scene* scene)
{
    currentScene = scene;
    currentScene->Initialize();
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);

    if (inGameScreen)
    {
        Util::DrawText(&program, font, "Ghost Hunter", 0.5f, -0.25f, glm::vec3(-1.5f, 3.0f, 0.0f));
        Util::DrawText(&program, font, "By George Huang", 0.5f, -0.25f, glm::vec3(-2.0f, 2.0f, 0.0f));
        Util::DrawText(&program, font, "Enter: Start the Hunt", 0.5f, -0.25f, glm::vec3(-3.0f, 1.0f, 0.0f));
        Util::DrawText(&program, font, "WASD: Move  J: Melee  R: Restart", 0.5f, -0.25f, glm::vec3(-4.0f, 0.0f, 0.0f));
        Util::DrawText(&program, font, "1 life", 0.5f, -0.25f, glm::vec3(-1.0f, -1.0f, 0.0f));
        Util::DrawText(&program, font, "7 Ghosts Around....", 0.5f, -0.25f, glm::vec3(-2.0f, -2.0f, 0.0f));
    }
    else
    {
        currentScene->Render(&program);

        if (lives == 0) //Draw Game Over
        {
            currentScene->state.player->textureID = Util::LoadTexture("0_Warrior_Died_008.png");

            Util::DrawText(&program, font, "You Die! Press R to play again!", 0.5f, -0.25f, glm::vec3(1.0, -4.0f, 0.0f));
        }
        else if (currentScene->state.player->enemiesKilled == 7) //Draw You Win
        {
            Util::DrawText(&program, font, "You Win! Ghost Hunter!", 0.5f, -0.25f, glm::vec3(2.0f, -4.0f, 0.0f));
        }
        else //If still in game draw instructions
        {
            Util::DrawText(&program, font, "Lives: " + std::to_string(lives), .75, -0.5f, glm::vec3(1, -0.25, 0));
        }

    }

    SDL_GL_SwapWindow(displayWindow);
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update()
{
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        accumulator = deltaTime;
        return;
    }

    if (!inGameScreen)
    {
        if (currentScene->state.player->killed && lives > 0)
        {
            currentScene->Reset();
            lives--;
        }

        while (deltaTime >= FIXED_TIMESTEP)

        {
            currentScene->Update(FIXED_TIMESTEP);

            deltaTime -= FIXED_TIMESTEP;
        }

        accumulator = deltaTime;

        viewMatrix = glm::mat4(1.0f);

        if (lives == 0 || currentScene->state.player->enemiesKilled == 7)
        {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        }
        else if (currentScene->state.player->position.x > 5 && currentScene->state.player->position.y < -3.75)
        {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, -currentScene->state.player->position.y, 0));
        }
        else if (currentScene->state.player->position.x > 5)
        {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
        }
        else if (currentScene->state.player->position.y < -3.75)
        {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, -currentScene->state.player->position.y, 0));
        }
        else
        {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        }
    }
}

void ProcessInput(){

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:

                break;

            case SDLK_RIGHT:

                break;

            case SDLK_SPACE:

                break;
            case SDLK_RETURN:

                break;
            }
            break;
        }
    }

    // Keyboard Controller
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (inGameScreen && keys[SDL_SCANCODE_RETURN]) {
        SwitchToScene(sceneList[0]);
        inGameScreen = false;
    }
    if (!inGameScreen && keys[SDL_SCANCODE_R])
    {
        currentScene->Reset();
        lives = 1;
    }

    if (!inGameScreen && lives > 0 && !currentScene->state.player->isWin)
    {
        currentScene->state.player->velocity = glm::vec3(0);

        if (keys[SDL_SCANCODE_A] && !currentScene->state.player->collidedLeft)
        {
            currentScene->state.player->attack = false;

            currentScene->state.player->textureID = Util::LoadTexture("0_Warrior_Run_002.png");

            currentScene->state.player->direction = "Left";

            currentScene->state.player->velocity.x = -1.0f;
        }
        else if (keys[SDL_SCANCODE_D] && !currentScene->state.player->collidedRight)
        {
            currentScene->state.player->attack = false;

            currentScene->state.player->textureID = Util::LoadTexture("0_Warrior_Run_009.png");

            currentScene->state.player->direction = "Right";

            currentScene->state.player->velocity.x = 1.0f;
        }
        if (keys[SDL_SCANCODE_W] && !currentScene->state.player->collidedTop)
        {
            currentScene->state.player->attack = false;

            currentScene->state.player->textureID = Util::LoadTexture("0_Warrior_Run_010.png");

            currentScene->state.player->direction = "Up";

            currentScene->state.player->velocity.y = 1.0f;
        }
        else if (keys[SDL_SCANCODE_S] && !currentScene->state.player->collidedBottom)
        {
            currentScene->state.player->attack = false;

            currentScene->state.player->textureID = Util::LoadTexture("0_Warrior_Run_013.png");

            currentScene->state.player->direction = "Down";

            currentScene->state.player->velocity.y = -1.0f;
        }
        if (glm::length(currentScene->state.player->velocity) > 1.0f)
        {
            currentScene->state.player->velocity = glm::normalize(currentScene->state.player->velocity);
        }
        if (keys[SDL_SCANCODE_J])
        {
            Mix_PlayChannel(-1, attacksound, 0);
            currentScene->state.player->attack = true;

            if (currentScene->state.player->direction == "Right")
            {
                currentScene->state.player->textureID = Util::LoadTexture("0_Warrior_Attack_2_007.png");
            }
            else if (currentScene->state.player->direction == "Left")
            {
                currentScene->state.player->textureID = Util::LoadTexture("0_Warrior_Attack_2_008.png");
            }
            else if (currentScene->state.player->direction == "Up")
            {
                currentScene->state.player->textureID = Util::LoadTexture("0_Warrior_Attack_2_010.png");
            }
            else if (currentScene->state.player->direction == "Down")
            {
                currentScene->state.player->textureID = Util::LoadTexture("0_Warrior_Attack_2_009.png");
            }
        }
    }
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Final Project: Ghost Hunter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    terrainMatrix = glm::mat4(1.0f);
    leftMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    font = Util::LoadTexture("font.png");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("spc-x2x-unseen-presence-by-kevin-macleod-from-filmmusic-io.mp3");
    Mix_PlayMusic(music, -1);

    attacksound = Mix_LoadWAV("362348__beerbelly38__knife-slash-3.wav");

    sceneList[0] = new Level1();
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                gameIsRunning = false;
            }
        }

        ProcessInput();
        Update();

        if (!inGameScreen)
        {
            if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);
        }

        Render();
    }

    Shutdown();
    return 0;
}
