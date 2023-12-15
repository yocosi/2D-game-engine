#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Sytems/MovementSystem.h"
#include "../Sytems/RenderSystem.h"
#include <iostream>

Game::Game()
{
  isRunning = false;
  registry = std::make_unique<Registry>();
  Logger::Log("Game constructor called!");
}

Game::~Game()
{
  Logger::Log("Game destructor called!");
}

void Game::Initialize()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
    Logger::Err("Erro initializing SDL.");
    return;
  }

  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode(0, &displayMode);
  windowWidth = displayMode.w;
  windowHeight = displayMode.h;

  window = SDL_CreateWindow(
      NULL,                   // Title of the window
      SDL_WINDOWPOS_CENTERED, // Positionning X
      SDL_WINDOWPOS_CENTERED, // Positionning Y
      // windowWidth,
      // windowHeight,
      1920, // Width
      1080, // Height
      0     // Flag
  );
  if (window == NULL)
  {
    Logger::Err("Error creating SDL window");
    return;
  }

  renderer = SDL_CreateRenderer(
      window,
      -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL)
  {
    Logger::Err("Error creating SDL renderer");
    return;
  }
  // Change the video mode of my display to become a "real" fullscreen
  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  isRunning = true;
}

void Game::ProcessInput()
{
  SDL_Event sdlEvent;
  while (SDL_PollEvent(&sdlEvent))
  {
    switch (sdlEvent.type)
    {
    case SDL_QUIT:
      isRunning = false;
      break;
    case SDL_KEYDOWN:
      if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
      {
        isRunning = false;
      }
      break;
    }
  }
}

void Game::Setup()
{
  // Add the systems that need to be processed in our game
  registry->AddSystem<MovementSystem>();
  registry->AddSystem<RenderSystem>();

  // Create an entity
  Entity tank = registry->CreateEntity();
  tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
  tank.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 0.0));
  tank.AddComponent<SpriteComponent>(10, 10);

  Entity truck = registry->CreateEntity();
  truck.AddComponent<TransformComponent>(glm::vec2(50.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
  truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 50.0));
  truck.AddComponent<SpriteComponent>(10, 50);
}

void Game::Update()
{
  // If we are too fast, waste some time until we reach the MILLISECS_PER_FRAME
  int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
  if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
  {
    // wait and give the hands to the OS to free some proc ressources
    SDL_Delay(timeToWait);
  }

  // The difference in ticks since the last frame, converted to seconds
  double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

  // Store the current frame time
  millisecsPreviousFrame = SDL_GetTicks();

  // Invoke all the systems that need to update
  registry->GetSystem<MovementSystem>().Update(deltaTime);

  // Update the registry to process the entities that are waiting to be created/deleted
  registry->Update();
}

void Game::Render()
{
  SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
  SDL_RenderClear(renderer);

  // Invode all the systems that need to render
  registry->GetSystem<RenderSystem>().Update(renderer);

  SDL_RenderPresent(renderer);
}

void Game::Run()
{
  Setup();
  while (isRunning)
  {
    ProcessInput();
    Update();
    Render();
  }
}

void Game::Destroy()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}