#ifndef MACHINE_H
#define MACHINE_H

#include "simulation.h"
#include "Neural.h"
#include "ui.h"

class Master;

class Machine
{
  public:
    void init(Master *master_ptr);
    bool run(int frame_time, input inputs);
    void saveState();
    void loadState(string path);
    human_action queryNetwork(vector<float> nn_inputs);

    UI *getUI() { return &ui; }
    NN *getNetwork() { return &network; }
    Simulation *getSimulation() { return &simulation; }
    SDL_Surface *getScreen() { return simulation.getScreen(); }
  private:
    int current_particle,
        current_epoch;
    vector<int> time_results; // The simulation results (total escape time) for each particle

    Master *master;
    Simulation simulation;
    NN network;
    UI ui;
};

#endif
