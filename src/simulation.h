#ifndef SIMULATION_H
#define SIMULATION_H

#include "functions.h"
#include "Neural.h"

class Master;
class Machine;
class UI;

using namespace std;

class Simulation
{
  public:
    void init(Master *master_ptr, Machine *machine_ptr);
    void fillBuilding();
    void update(int frame_time, input inputs);

    void setStatus(simulation_status s) { status = s; }
    void setWalls(vector<pixel> vertices) { wall_vertices = vertices; }
    void setLoadWalls(bool b) { load_walls = b; }
    void setConeDrawing(bool setting) { single_cone = setting; }
    void setPeopleAmount(int n) { n_people = n; }
    void setMinRadius(int r) { min_radius = r; }
    void setMaxRadius(int r) { max_radius = r; }
    void setMinSpeed(float s) { min_speed = s; }
    void setMaxSpeed(float s) { max_speed = s; }
    void setMinVision(float v) { min_vision = v; }
    void setMaxVision(float v) { max_vision = v; }
    void setMaxHeight(int h) { max_height = h; }
    void setMinHeight(int h) { min_height = h; }
    void setMinAge(int a) { min_age = a; }
    void setMaxAge(int a) { max_age = a; }
    void setMaxFrames(int a) { max_frames = a; }
    void setNN(int a) { NeuralNetwork = a; }
    int getNN();


    visible_information applyPerception(human *h);
    simulation_status getStatus() { return status; }
    int getResult();
    vector<pixel> getWalls() { return wall_vertices; }
    string getStatusText(simulation_status s) { return status_text[s]; }
    SDL_Surface *getScreen() { return screen; }

  private:
    void calculateTotalTime();
    void handleInput(int frame_time, input inputs);
    void updateActions();
    void moveHumans(int frame_time);
    void updateFallen();
    void push(human *a, human *b);
    void trample(int frame_time, human *fallen, human *treading, float overlap);
    void createWalls(input *inputs);
    void updateWallSurface();
    void fillWallBackground();
    void floodFillInside(rgb target, rgb overwrite, pixel start);
    void setupIcons();
    void setupVisionCone();
    void createCircle(SDL_Surface *circle, rgb colour);
    void setupStatusText();

    void drawHuman(human individual);
    void drawVision();
    void drawWalls();
    void placeHuman(dim2 position, dim2 direction, human_gender gender, int age, int height, int radius, float vision_range, health_status status); /** Unused **/

    vector<human *> humanCollision(human *target, float *distance);
    bool collisionChecked(vector< vector<human *> > checked_collisions, human *a, human *b);
    bool hitsWall(human *target, bool include_exit);
    bool isFrontHuman(human *h, vector<human *> collisions);
    bool humanInBuilding(human *h);
    vector<human *> visibleHumans(human *h);
    float getPushChance(human *h);
    void getAgeMeanVariance(vector<human *> humans, float *mean, float *variance);
    void getRadiusMeanVariance(vector<human *> humans, float *mean, float *variance);
    void getHeightMeanVariance(vector<human *> humans, float *mean, float *variance);
    void getPanicMeanVariance(vector<human *> humans, float *mean, float *variance);
    void getDirectionMeanVariance(vector<human *> humans, dim2 *mean, dim2 *variance);
    dim2 determineExit();
    vector<float> createNNInputs(human *h, visible_information info);
    visible_information normaliseVision(visible_information info);

    SDL_Surface *screen, *walls;
    SDL_Texture *vision_cone;
    vector<circle_surface> icons;

    human *focus_human;
    vector<human> people;
    vector<pixel> wall_vertices;
    dim2 exit_location;

    map<simulation_status, string> status_text;

    simulation_status status; // Run status of the simulation
    Uint32 total_escape_time,
           start_time;
    int max_frames, // Simulation is allowed to run for this many frames before quitting
        frame_counter,
        frames_since_action,  // The amount of frames since the network was last consulted
        action_rate;  // Every how many frames should the network be consulted
    bool load_walls;  // Whether a wall configuration file must be loaded
    bool single_cone; // Whether just the vision cone of the focused human should be drawn
    bool drawing;
    int n_people;
    int max_placement_tries;  // How many times a person should be attempted to be spawned not inside somebody else
    int min_radius, max_radius; // The minimum/maximum radius of a person, anything < 5 makes the circle odd shaped
    int min_age, max_age;
    int min_height, max_height;
    float min_speed, max_speed; // Speed of people, in pixels/ms
    float min_vision, max_vision,
          default_vision_range;
    float default_fov;  // The default field of view of a person, in degrees
    float default_panic;
    Uint8 vision_alpha; // The alpha channel value of the vision cone
    float chance_collision_fall;  // The chance that somebody will fall if they lose a push impact
    float trample_constant; // The amount of trample status added times radius (per ms)
    Uint32 push_rate; // How many ms must be between each push from one person
    Uint32 standup_time;  // Time in ms after which a fallen person will stand up again
    rgb colour_healthy, colour_fallen, colour_dead,
        vision_colour,
        wall_colour, exit_colour, inside_bg_colour;

    SDL_Renderer *renderer;

    Master *master;
    Machine *machine;
    UI *ui;
    NN *network;
    int NeuralNetwork;

};

#endif // SIMULATION
