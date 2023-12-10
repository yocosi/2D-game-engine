#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

const int FPS = 144;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game
{
private:
  bool isRunning;
  int millisecsPreviousFrame = 0;
  SDL_Window *window;     // Game window
  SDL_Renderer *renderer; // Renderer who go inside the window

public:
  Game();
  ~Game();
  void Initialize();
  void Run();
  void Setup();
  void ProcessInput();
  void Update();
  void Render();
  void Destroy();

  int windowWidth;
  int windowHeight;
};