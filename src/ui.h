#ifndef UI_H
#define UI_H

#include "functions.h"

#include <SDL_ttf.h>

class Master;
class Simulation;

using namespace std;

class UI
{
  public:
    void init(Master *master_ptr);
    void update(input *inputs);

    void setStatus(simulation_status s) { status = s; }
  private:
    void readConfigFile(string path);
    void handleTextInput(bool *done, input *inputs);
    void handleFileSelecting(input *inputs);
    void handleWallStoring(input *inputs);
    void drawFileSelector(bool draw_buttons);

    string input_text;
    dim2 input_box_pos,
         text_offset;

    simulation_status status;

    SDL_Texture *box_surface,
                *text_texture;

    TTF_Font *input_font;

    SDL_Renderer *renderer;

    Master *master;
    Simulation *simulation;
};

#endif

