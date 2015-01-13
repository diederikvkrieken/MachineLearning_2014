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

bool Machine::run(int frame_time, input inputs)
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
    // Get and store the result
    int result_time = simulation->getResult();
    time_results.push_back(result_time);

    current_particle++;
    if(current_particle >= network.n_particles)
    {
      // Train the networks after simulation ends
      network.trainNN(time_results);
      time_results.clear();
      current_particle = 0;
      epoch++;
    }
    if(epoch >= network.n_epochs)
    {
      return false;
    }
    simulation.setStatus(IDLE);
  }

  simulation.update(frame_time, inputs);

  return true;  // Keep running
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








