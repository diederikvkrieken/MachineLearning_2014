#include "machine.h"

#include "master.h"

void Machine::init(Master *master_ptr)
{
  master = master_ptr;

  ui.init(master);
  simulation.init(master, this);
  network.initializeNN();

  current_particle = 0;
  current_epoch = 0;
}

bool Machine::run(int frame_time, input inputs)
{
  if(simulation.getStatus() == IDLE)
  {
    printf("idling\n");
    // Simulation is waiting to start
    // Construct the appropriate network
    network.positionToWeights(current_particle);
    // Start simulation
    simulation.setStatus(SPAWNING);
  }
  if(simulation.getStatus() == STOPPED)
  {
    printf("stopped\n");
    // Get and store the result
    int result_time = simulation.getResult();
    time_results.push_back(result_time);

    current_particle++;
    if(current_particle >= network.nParticles)
    {
      // Train the networks after simulation ends
      printf("epoch++\n");
      for(unsigned int i=0; i < time_results.size(); i++)
      {
        printf("time %d: %d\n", i, time_results[i]);
      }
      network.trainNN(time_results);
      time_results.clear();
      current_particle = 0;
      current_epoch++;
    }
    if(current_epoch >= network.maxEpoch)
    {
      printf("quitting\n");
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
  /*printDim2("direction output: ", action.direction);*/
  action.direction = (normalise(action.direction))/100;
  action.panic = output[2];
  return action;
}








