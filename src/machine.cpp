#include "machine.h"

#include "master.h"

void Machine::init(Master *master_ptr)
{
  master = master_ptr;
  
  simulation.init(master);
  ui.init(master);
  network.initialize();
  
  current_particle = 0;
  current_epoch = 0;
}

void Machine::run(int frame_time, input inputs)
{
  if(simulation.getStatus() == IDLE)
  {
    // Simulation is waiting to start
    // Construct the appropriate network
    network.positionToWeights(current_particle);
    // Start simulation
    simulation.setStatus(SPAWNING);
  }
  if(simulation.getStatus() == STOPPED)
  {    
    current_particle++;
    if(current_particle >= network.n_particles)
    {
      /** TODO: train network **/
      current_particle = 0;
      epoch++;
    }
    if(epoch >= network.n_epochs)
    {
      /** TODO: stop **/
    }
    simulation.setStatus(IDLE);
  }
}

human_action Machine::queryNetwork(vector<float> nn_inputs)
{
  human_action action;
  
  vector<float> output = network.runNN(nn_inputs);
  action.direction.set(output[0], output[1]);
  action.direction = normalise(action.direction);
  action.panic = output[2];
  return action;
}








