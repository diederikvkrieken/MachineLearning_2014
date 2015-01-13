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
    void run(int frame_time, input inputs);
    human_action queryNetwork(vector<float> nn_inputs);
  private:
    int current_particle;
    
    Master *master;
    Simulation simulation;
    NN network;
    UI ui;
};

#endif
