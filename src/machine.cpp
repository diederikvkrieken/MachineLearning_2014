#include "machine.h"

#include "master.h"

void Machine::init(Master *master_ptr)
{
  master = master_ptr;

  ui.init(master, this);
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
    int NN = simulation->getNN;
    if (NN == 1)
    {
        network.positionToWeights(current_particle);
    }
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
      // Train the networks after simulation ends, particle optimisation
      /*printf("epoch++\n");
      for(unsigned int i=0; i < time_results.size(); i++)
      {
        printf("time %d: %d\n", i, time_results[i]);
      }*/
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
    printf("epoch:\t%d.\t Particle:\t%d\n", current_epoch, current_particle);
    simulation.setStatus(IDLE);
  }

  simulation.update(frame_time, inputs);

  return true;  // Keep running
}

void Machine::saveState()
{
  ofstream file;
  stringstream output, file_name;

  file_name << "saves/" << "e" << current_epoch << "p" << current_particle << ".save";
  printf("save file name: %s\n", file_name.str().c_str());

  file.open(file_name.str().c_str());
  if(!file.is_open())
  {
    fprintf(stderr, "[Machine.saveState] Error opening file: %s.\n", file_name.str().c_str());
  }

  // Current epoch
  output << current_epoch << "\n";
  file.write(output.str().c_str(), output.str().size());
  output.str("");
  // Current particle
  output << current_particle << "\n\n";
  file.write(output.str().c_str(), output.str().size());
  output.str("");

  vector<float> global_best_weights = network.getBestWeights();
  // Store best positions
  for(unsigned int i=0; i < global_best_weights.size(); i++)
  {
    output << global_best_weights[i];
    if(i < global_best_weights.size() - 1)
    { output << "|" ; }
    file.write(output.str().c_str(), output.str().size());
    output.str("");
  }
  file.write("\n\n", 2);

  // Particle positions
  for(int i=0; i < network.nParticles; i++)
  {
    // Get the values
    vector<float> weights = network.getParticleWeights(i);
    vector<float> speed = network.getParticleSpeed(i);
    vector<float> best_weights = network.getParticleBestWeights(i);
    for(unsigned int j=0; j < weights.size(); j++)
    {
      output << weights[j] << ";" << speed[j] << ";" << best_weights[j];
      if(j < weights.size() -1)
      { output << "|"; }
      file.write(output.str().c_str(), output.str().size());
      output.str("");
    }

    output << "\n----------------------------\n"; // Separation of particles
    file.write(output.str().c_str(), output.str().size());
    output.str("");
  }

  file.close();
}

/** See ./saves/layout for format **/
void Machine::loadState(string path)
{
  ifstream file;
  string line;
  vector<string> words;
  vector<string> values;
  vector<float> weights;

  file.open(path.c_str());
  if(!file.is_open())
  {
    fprintf(stderr, "[Machine.loadState] Error opening file: %s.\n", path.c_str());
  }

  int line_number = 0;
  while(getline(file, line))
  {
    weights.clear();

    if(line == "----------------------------")
    { continue; } // Particle separation
    if(line.size() == 0)
    { continue; } // Empty lines

    if(line_number == 0)
    {
      // Epoch
      current_epoch = atoi(line.c_str());
    }
    else if(line_number == 1)
    {
      // Particle
      current_particle = atoi(line.c_str());
    }
    else if(line_number == 2)
    {
      // Global best
      words = splitString(line, '|');
      for(unsigned int i=0; i < words.size(); i++)
      {
        weights.push_back(atof(words[i].c_str()));
      }
      network.setBestWeights(weights);
    }
    else if(line_number > 2)
    {
      vector<float> speeds;
      vector<float> bests;
      int particle = line_number - 3;
      words = splitString(line, '|');
      for(unsigned int i=0; i < words.size(); i++)
      {
        values = splitString(words[i], ';');
        weights.push_back(atof(values[0].c_str()));
        speeds.push_back(atof(values[1].c_str()));
        bests.push_back(atof(values[2].c_str()));
      }
      network.setParticleWeights(particle, weights);
      network.setParticleSpeed(particle, speeds);
      network.setParticleBestWeights(particle, bests);
    }

    line_number++;
  }

  file.close();
}

human_action Machine::queryNetwork(vector<float> nn_inputs)
{
  human_action action;

  if (NN == 1)
  {
    vector<float> output = network.runNN(nn_inputs);
  }else
  {
    vector<float> output = network.runHandAlgorithm(nn_inputs);
  }


  action.direction.set(output[0], output[1]);
  /*printDim2("direction output: ", action.direction);*/
  action.direction = (normalise(action.direction))/100;
  action.panic = output[2];
  return action;
}








