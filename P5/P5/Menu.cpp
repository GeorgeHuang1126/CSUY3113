#include "Menu.h"

#define MENU_WIDTH 11
#define MENU_HEIGHT 9

unsigned int menuData[] = {
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
};

GLuint fontTextureMenuID;

void Menu::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");

    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menuData, mapTextureID, 1.0f, 4, 1);

    fontTextureMenuID = Util::LoadTexture("font1.png");


}

void Menu::Update(float deltaTime) {
    return;
}
void Menu::Render(ShaderProgram* program) {
    state.map->Render(program);

    Util::DrawText(program, fontTextureMenuID, "Watch out! Eevee!", .85, -0.5f, glm::vec3(2.0, -1.5, 0));

    Util::DrawText(program, fontTextureMenuID, "Press Enter to start!", .80, -0.5f, glm::vec3(2.0, -5.0, 0));

}

