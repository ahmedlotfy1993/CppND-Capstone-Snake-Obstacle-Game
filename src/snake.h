#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include "SDL.h"
#define MAX_NUM_LIVES 4
class Snake {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height)
      : grid_width(grid_width),
        grid_height(grid_height),
        head_x(grid_width / 2),
        head_y(grid_height / 2) {}

  void Update();

  void GrowBody();
  bool SnakeCell(int x, int y);
  void Restart(void);

  Direction direction = Direction::kUp;

  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;
  int food_eaten = 0;
  int num_lives =0;
 private:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);
  
  bool growing{false};
  int grid_width;
  int grid_height;
};

#endif