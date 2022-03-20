#include "game.h"
#include <iostream>
#include "SDL.h"
#include <thread>
#include <random>
#include <future>
#include <time.h>
#include <chrono>
#include <math.h> 
#include <stdio.h>

using namespace std;

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width)),
      random_h(0, static_cast<int>(grid_height)) {
  PlaceFood();
}
void Game::food_mover()
{
    
    while(snake.alive == true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

      	std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_int_distribution<> distr(8, 10);
        
        int cycle_duration = distr(eng);
        int time_since_last_update = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - last_update).count();

        if(time_since_last_update >cycle_duration)
        {
          last_update = std::chrono::system_clock::now();
          _mutex.lock();
          PlaceFood();
          _mutex.unlock();
        }
      // std::cout<<"in a thread "<<time_since_last_update<<endl;
    }
}


void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;
  last_update = std::chrono::system_clock::now();


  std::thread t = std::thread(&Game::food_mover,this);

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, food, obstacles);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    bool flag = false;
    // Check that the location is not occupied by a snake item before placing
    // food.
    for (auto const &obstacle : obstacles) {
      if(obstacle.x == x && obstacle.y == y)
      {
        flag = true;
      }
    }    
    if (!snake.SnakeCell(x, y) && flag == false) {
      food.x = x;
      food.y = y;
      cout<<"food placed at: "<<food.x<<" "<<food.y<<endl;
      return;
    }
  }
}
void Game::PlaceObstacle(){
  int x, y;
  while (true) {
    bool flag = false;
    x = random_w(engine);
    y = random_h(engine);
    for (auto const &obstacle : obstacles) {
      if(obstacle.x == x && obstacle.y == y)
      {
        flag = true;
      }
    }    
    //Eating food
    if(food.x!=x && food.y!= y && !snake.SnakeCell(x,y) && flag == false){
      SDL_Point temp;
      temp.x = x;
      temp.y = y;
      cout<<"obstacle updated"<<endl;
      obstacles.push_back(temp);
      cout<<"obstacle placed at: "<<temp.x<<" "<<temp.y<<endl;
      return;
    }

  }
}

void Game::Update() {
  //if (!snake.alive) return;
  if (snake.num_lives == MAX_NUM_LIVES)
  {
   cout<<"Game over\n"; 
   return;
  }
  else
  {
   snake.alive = true;
   score = 0;
  }
    

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    last_update = std::chrono::system_clock::now();
    _mutex.lock();
    PlaceObstacle();
    PlaceFood();

    _mutex.unlock();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.food_eaten++;
    snake.speed += 0.1*pow(2,-snake.food_eaten);
    // std::cout<<snake.speed<<std::endl;
  }
  //Obstacle collision
  for (auto const &obstacle : obstacles) {
    if(obstacle.x == new_x && obstacle.y == new_y)
    {
      snake.Restart();
      snake.num_lives++;
      obstacles.clear();
      cout<<"You are DEAD!! you have " << MAX_NUM_LIVES - snake.num_lives << " lives\n";
    }
  }    
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }