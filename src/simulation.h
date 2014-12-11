#ifndef SIMULATION_H
#define SIMULATION_H

#include "functions.h"

class Master;
class UI;

using namespace std;

class Simulation
{
  public:
    void init(Master *master_ptr);
    void fillBuilding();
    void update(int frame_time, input inputs);

    void setStatus(simulation_status s) { status = s; }
    void setPeopleAmount(int n) { n_people = n; }
    void setMinRadius(int r) { min_radius = r; }
    void setMaxRadius(int r) { max_radius = r; }
    void setMinSpeed(float s) { min_speed = s; }
    void setMaxSpeed(float s) { max_speed = s; }
    void setMinVision(float v) { min_vision = v; }
    void setMaxVision(float v) { max_vision = v; }
    void setLoadWalls(bool b) { load_walls = b; }

    simulation_status getStatus() { return status; }
    vector<pixel> getWalls() { return wall_vertices; }
    string getStatusText(simulation_status s) { return status_text[s]; }
    SDL_Surface *getScreen() { return screen; }

    // Debug
    void testSim();
  private:
    void handleInput(int frame_time, input inputs);
    void moveHumans(int frame_time);
    void createWalls(input *inputs);
    void editWallSurface();
    void floodFillInside(rgb target, rgb overwrite, pixel start);
    void setupIcons();
    void setupVisionCone();
    void createCircle(SDL_Surface *circle, rgb colour);
    void setupStatusText();

    void drawHuman(human individual);
    void drawVision();
    void drawWalls();
    void placeHuman(dim2 position, dim2 direction, human_gender gender, int age, int height, int radius, float vision_range, health_status status); /** Unused **/

    human *humanCollision(human *target, float *distance);
    bool hitsWall(human *target);

    SDL_Surface *screen, *walls;
    SDL_Texture *vision_cone;
    vector<circle_surface> icons;

    vector<human> people;
    vector<pixel> wall_vertices;

    map<simulation_status, string> status_text;

    simulation_status status; // Run status of the simulation
    bool load_walls;  // Whether a wall configuration file must be loaded
    int n_people;
    int max_placement_tries;  // How many times a person should be attempted to be spawned not inside somebody else
    int min_radius, max_radius; // The minimum/maximum radius of a person, anything < 5 makes the circle odd shaped
    float min_speed, max_speed; // Speed of people, in pixels/ms
    float min_vision, max_vision,
          default_vision_range;
    float default_fov;  // The default field of view of a person, in degrees
    Uint8 vision_alpha; // The alhpa channel value of the vision cone
    rgb colour_healthy, colour_fallen, colour_dead,
        vision_colour,
        wall_colour, exit_colour, inside_bg_colour;

    SDL_Renderer *renderer;

    Master *master;
    UI *ui;
};

#endif // SIMULATION
