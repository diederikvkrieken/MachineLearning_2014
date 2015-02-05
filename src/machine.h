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
    void saveState(bool quit);
    void loadState(string path);
    human_action queryNetwork(vector<float> nn_inputs);

    void setRepeatParticle(int p) { repeat_particle = p; }

    UI *getUI() { return &ui; }
    NN *getNetwork() { return &network; }
    Simulation *getSimulation() { return &simulation; }
    SDL_Surface *getScreen() { return simulation.getScreen(); }
    void saveResults(vector<int> result);
  private:
    int current_particle,
        repeat_particle,  // Which particle should be repeated for visualization, -1 if not applicable
        current_epoch;
    bool quit_next_loop,
         train_on_epochs;
    vector<int> time_results; // The simulation results (total escape frames) for each particle

    Master *master;
    Simulation simulation;
    NN network;
    UI ui;
};

#endif
