#ifndef UI_H
#define UI_H

#include <SDL_ttf.h>

#include <iostream>
#include <fstream>

#include "functions.h"

class Master;
class Machine;
class Simulation;

using namespace std;

class UI
{
  public:
    void init(Master *master_ptr, Machine *machine_ptr);
    void update(input *inputs);

    void setStatus(simulation_status s) { status = s; }
    simulation_status status;
  private:
    void readConfigFile(string path);
    void readWalls(string path);
    void exportWalls(string path);
    void parseParameter(vector<string> words);
    void handleTextInput(bool *done, input *inputs);
    void handleFileSelecting(input *inputs);
    void handleWallStoring(input *inputs);
    void drawFileSelector(bool draw_buttons);

    bool clickedOnButton(SDL_Rect button, input *inputs);

    string input_text;
    dim2 input_box_pos,
         text_offset;
    SDL_Rect save_button, ignore_button;

    string walls_file; // Which walls configuration file should be read

    SDL_Texture *box_surface,
                *text_texture;

    TTF_Font *input_font;

    SDL_Renderer *renderer;

    Master *master;
    Machine *machine;
    Simulation *simulation;
};

#endif

