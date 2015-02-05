#include "machine.h"

#include "master.h"

void Machine::init(Master *master_ptr)
{
  master = master_ptr;

  ui.init(master, this);
  simulation.init(master, this);
  network.initializeNN();

  current_particle = 0;
  repeat_particle = -1;
  current_epoch = 0;
  quit_next_loop = false;
  train_on_epochs = true;
}

bool Machine::run(int frame_time, input inputs)
{
  int NN = simulation.getNN();
  if(repeat_particle > -1)
  { current_particle = repeat_particle; }

  if(simulation.getStatus() == IDLE)
  {
    // Simulation is waiting to start
    // Construct the appropriate network
    if(NN == 1)
    {
      network.positionToWeights(current_particle);
    }
    // Start simulation
    simulation.setStatus(SPAWNING);
  }
  if(simulation.getStatus() == STOPPED)
  {
    printf("Stopped simulation\n");

    if(quit_next_loop)
    { return false; }

    // Get and store the result
    int result_time = simulation.getResult();
    time_results.push_back(result_time);

    if(repeat_particle == -1)
    { current_particle++; }
    if(current_particle >= network.nParticles)
    {
      /*printf("epoch++\n");
      for(unsigned int i=0; i < time_results.size(); i++)
      {
        printf("time %d: %d\n", i, time_results[i]);
      }*/
      current_particle = 0;
      if(train_on_epochs)
      {
        current_epoch++;
        saveResults(time_results);
        // Train the networks after simulation ends, particle optimisation
        if(NN == 1)
        { network.trainNN(time_results); }
      }
      time_results.clear();
    }
    if(current_epoch >= network.maxEpoch)
    {
      // Save results automatically when at the end of the experiment
      saveState(false);
      train_on_epochs = false;
      current_epoch--;
    }
    printf("\tEpoch:\t\t%d.\n\tParticle:\t%d.\n", current_epoch, current_particle);
    simulation.setStatus(IDLE);
  }

  simulation.update(frame_time, inputs);

  return true;  // Keep running
}

void Machine::saveState(bool quit)
{
  if(quit)
  { quit_next_loop = true; }
  else
  { quit_next_loop = false; }

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
  vector<float> output;

  output = network.runNN(nn_inputs);

  action.direction.set(output[0], output[1]);
  action.direction = action.direction * 0.1f; // To adjust for values [-1,1]
  /*printDim2("direction output", action.direction);*/
  action.panic = output[2];
  action.panic = (action.panic + 1.0f) * 0.5f; // Convert to [0,1]
  return action;
}

void Machine::saveResults(vector <int> result){
// write results
  ofstream results;
  results.open(network.input_text.c_str(), std::ios::app);
  if (results.is_open())
  {
    for(unsigned int i=0; i < result.size(); i++)
    {
      results << result[i] << "; ";
    }
    results << "\n";
    results.close();
  }
}





