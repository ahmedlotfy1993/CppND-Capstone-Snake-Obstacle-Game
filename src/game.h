#ifndef GAME_H
#define GAME_H

#include <random>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include <chrono>
#include <mutex>

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  void food_mover();
  
 private:
  Snake snake;
  SDL_Point food;
  std::vector<SDL_Point> obstacles;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};
  void PlaceObstacle();
  void PlaceFood();
  void Update();
  
  std::chrono::_V2::system_clock::time_point last_update = std::chrono::system_clock::now();
  std::timed_mutex _mutex;      
  
};

#endif