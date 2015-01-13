#ifndef MASTER_H
#define MASTER_H

#include <cstdlib>
#include <iostream>
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "structs.h"
#include "functions.h"

#include "simulation.h"
#include "ui.h"

class Master
{
  public:
    bool init();
    void run();
    void wait(Uint32 frame_length);
    void outputFPS(Uint32 frame_length);
    void quit();
    void handleInput();
    void resetInputs();
    SDL_Surface *createEmptySurface(int width, int height);
    SDL_Texture *loadImage(string path);

    Simulation *getSimulation() { return &simulation; }
    UI *getUI() { return &ui; }

    SDL_Window *getWindow() { return window; }
    SDL_Renderer *getRenderer() { return renderer; }
    SDL_Texture *getMasterTexture() { return screen; }
    dim2 getResolution() { return resolution; }
  private:
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *screen;

    Uint32 rmask, gmask, bmask, amask;

    dim2 resolution;
    bool running;
    input inputs;           // User input each frame

    bool output_fps;        // If frames per second should be displayed in the console
    int max_fps,
        frame_counter,
        frame_time_counter;
    int fps_output_rate;    // Every how many ms the FPS should be displayed
    Timer frame_timer;
};

#endif // MASTER_H

