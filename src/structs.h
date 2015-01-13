#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_opengl.h>

#include "timer.h"

using namespace std;

const float PI = 3.14159265359;
const float EPSILON = 0.00001;

const int LEFT =        0;
const int RIGHT =       1;
const int UP =          5;
const int DOWN =        6;
const int FORWARD =     7;
const int BACKWARD =    8;
const int MIDDLE =      2;
const int WHEEL_UP =    3;
const int WHEEL_DOWN =  4;

const int BLUE =    0;
const int RED =     1;
const int GREEN =   2;
const int YELLOW =  3;
const int WHITE =   4;
const int BLACK =   5;

const int SPHERE =    0;
const int TRIANGLE =  1;
const int BOTTOM =    3;
const int TOP =       4;

enum compass_direction
{
  NORTH,
  SOUTH,
  WEST,
  EAST,
  // Below: AB indicates direction from A to B.
  NW,
  WN,
  NE,
  EN,
  SW,
  WS,
  SE,
  ES
};

// Angles are given in degrees in the interval [0,360] which corresponds to the counter-clockwise circumference of the unity circle

struct rgb
{
  void set(float s_r, float s_g, float s_b)
  {
    r = s_r;
    g = s_g;
    b = s_b;
  }
  void set(float s_col)
  {
    r = s_col;
    g = s_col;
    b = s_col;
  }
  float r;
  float g;
  float b;
};

struct dim2
{
  void set(float s_x, float s_y)
  {
    x = s_x;
    y = s_y;
  }
  float x;
  float y;
};

struct pixel
{
  void set(int s_x, int s_y)
  {
    x = s_x;
    y = s_y;
  }
  int x;
  int y;
};

struct dim3
{
  void set(float s_x, float s_y, float s_z)
  {
    x = s_x;
    y = s_y;
    z = s_z;
  }
  float x;
  float y;
  float z;
};

struct coord3
{
  void set(int s_x, int s_y, int s_z)
  {
    x = s_x;
    y = s_y;
    z = s_z;
  }
  int x;
  int y;
  int z;
};

struct quaternion
{
  float x;
  float y;
  float z;
  float w;
};

struct texture
{
  string name;
  int w;
  int h;
  rgb **image;
};

struct input
{
  dim2 mouse_pos;
  dim2 mouse_relative;
  int mouse_down;
  int mouse_up;
  int key;  // Which key was pressed
  const Uint8 *key_state;
};

/** -------------------- Application specific ------------------- **/
enum human_gender
{
  MALE,
  FEMALE
};

enum health_status
{
  HEALTHY,
  FALLEN,
  DEAD
};

enum simulation_status
{
  IDLE,
  MAIN_SCREEN,
  SELECTING_FILE,
  READING_CONFIG,
  DRAWING_WALLS,
  STORING_WALLS,
  SPAWNING,
  RUNNING,
  PAUSED,
  STOPPED
};

struct human
{
  int ID;
  dim2 position;
  dim2 previous_position; // Useful for collision detection
  dim2 direction; // Also contains speed
  human_gender gender;
  int age;
  int height; // Height in cm
  int radius; // Size of person, in pixels
  float vision_range; // How far someone can see ahead
  float fov;  // Field of view
  float panic; // Panic number between 0 (low panic) and 1 (high panic)
  bool escaped;
  Uint32 escape_time; // Time it took to escape in ms. Max frames value in case of no escape
  health_status status;
  float trample_status; // Value in [0,1] that indicates how close somebody is to dying from being trampled
  Timer lying;  // Time in ms indicating how long someone has been lying on the ground
  Timer push_time_out;  // How long somebody hasn't pushed someone else
};

struct visible_information
{
  int n_people;
  /*float male_ratio;*/
  float mean_height;
  float mean_radius;
  float mean_age;
  float mean_panic;
  dim2 mean_direction;
  float var_height;
  float var_radius;
  float var_age;
  float var_panic;
  dim2 var_direction;
  int n_walls;
  float closest_wall_distance;
  float exit_distance;
};

struct human_action
{
  dim2 direction;
  float panic;
};

struct circle_surface
{
  int radius;
  SDL_Surface *healthy;
  SDL_Surface *fallen;
  SDL_Surface *dead;
};

#endif












