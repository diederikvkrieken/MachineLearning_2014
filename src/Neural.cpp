#include "Neural.h"

void NN::initializeNN()
{
  n_layers = 1;
  n_input = 26;
  n_hidden = 33;
  n_output = 3;
  input_text = "walls1.csv";

  /** Parameters of the particle swarm **/
  //youtube video gives optimal range 20-40
  maxEpoch = 1000;
  nParticles = 20;
  /** x_max x_min v_max v_min **/
  x_max = 10;
  x_min = -10;
  v_max = 1;
  v_min = -1;
  precision = 0.001; // Precision for randomFloat function
  c1 = 1.4;
  c2 = 1.4;
  w = 1.4;
  /*******************************************************/
  // Allocate neural network
  nHL.assign(n_hidden,0);
  bHL.assign(n_hidden,0);
  nOL.assign(n_output,0);
  bOL.assign(n_output,0);
  nIL.assign(n_input,0);
  // Allocate size
  vector<float> inside_vector;
  // From hidden to input, i->j
  for(int i=0; i < n_hidden; i++)
  {
    inside_vector.assign(n_input,0);
    wHL.push_back(inside_vector);
  }
  // From output to hidden, i->j
  for(int i=0; i < n_output; i++)
  {
    inside_vector.assign(n_hidden,0);
    wOL.push_back(inside_vector);
  }
  // Fill with random.
  for(int i=0; i < n_hidden; i++)
  {
    for(int j=0; j < n_input; j++)
    {
      wHL[i][j] = randInt(0, 100) / 10.0;
    }
    // Initialise output-hidden weights
    for(int j=0; j < n_output; j++)
    {
      wOL[j][i] = randInt(0, 100) / 10.0;
    }
  }

  //number of weights+bias
  int nW = (n_input*n_hidden) + (n_output*n_hidden) + n_hidden + n_output;
  /** TO-DO maxValue Controleren **/
  float best_global_error = 100000000;

  // Allocate particles
  vector<float> inside_vector;
  for(int i=0; i < nParticles; i++)
  {
    inside_vector.assign(nW,0);
    p_x.push_back(inside_vector);
    p_v.push_back(inside_vector);
  }
  // swarm initialization
  // initialise each Particle in the swarm with random positions and velocities
  for (int i = 0; i<nParticles;i++)
  {
    //Particle initialization
    for (int j = 0; j<nW;j++)
    {
      // particle position:
      // randomPosition = (hi - lo) * random_double + lo; (waarde tussen max en min)
      p_x[i][j] = (x_max - x_min) * randomFloat(0, 1, precision) + x_min;
      // particle velocity:
      // randomVelocity[j] = (hi - lo) * rnd.NextDouble() + lo;
      p_v[i][j] = (v_max - v_min) * randomFloat(0, 1, precision) + v_min;
    }
  }
}

void NN::trainNN(vector <int> result)
{
  // write results
  ofstream results;
  myfile.open(input_text, std::ios::app);
  if (myfile.is_open())
  {
    for(int i=0; i < result.size(); i++)
    {
      myfile << result[i] << "; ";
    }
    myfile << "\n";
    myfile.close();
  }
  else cout << "Unable to open file";
  //Check for best error result
  for(int i=0; i < nParticles; i++)
  {
    if (result[i] < p_be[i])
    {
      p_be[i] = result[i];
      p_bx[i] = p_x[i];
    }
    if (result[i] < best_global_error)
    {
      best_global_error = result[i];
      best_global_position = p_x[i];
    }
  }

  // Update particles
  for(int i=0; i < nParticles; i++)
  {
    for (int j = 0; j<nW;j++)
    {
      // 1. new velocity
      // new velocity = (w * current_v)+(c1 * r1 * p_best_pos - current_pos)+(c2 * r2 * global_best_pos - current_pos)
      p_v[i][j] = (w * p_v[i][j])
      + (c1*randomFloat(0, 1, precision)*(p_bx[i][j]-p_x[i][j]))
      + (c2*randomFloat(0, 1, precision)*(best_global_position[j]-p_x[i][j]));

      // Ensure smallest/largest
      clamp(*p_v[i][j], v_min, v_max);

      // 2. compute new position with the new velocity
      // Position = current_pos + new velocity;
      p_x[i][j] = p_x[i][j] + p_v[i][j];

      // Ensure smallest/largest
      clamp(*p_x[i][j], x_min, x_max)
    }
  }
}

void NN::positionToWeights(int i)
{
  vector<float> position = p_x[i];
  unsigned int nW = (n_input*n_hidden) + (n_output*n_hidden) + n_hidden + n_output;
  if (position.size() != nW)
  {
    /** TO-DO error **/
    //throw exception??
    //error!!
  }
  int i=0;
  for(int j=0; j < n_hidden; j++)
  {
    // hidden to input nodes weights
    for(int k=0; k < n_input; k++)
    {
      wHL[j][k] = position[i];
      i++;
    }
    // output to hidden nodes weights
    for(int k=0; k < n_output; k++)
    {
      wOL[k][j] = position[i];
      i++;
    }
  }
  // hidden node bias
  for(int j=0; j < n_hidden; j++)
  {
    bHL[j] = position[i];
    i++;
  }
  // output node bias
  for(int j=0; j < n_output; j++)
  {
    bOL[j] = position[i];
    i++;
  }
}

float NN::activationFunction(float input ,float bias)
{
	// sigmoid function
	float x = input + bias;
	return 1/(1+expf(-x));
	// tanh function
	// return tanh(x);
}

vector<float> NN::runNN(vector<float> input)
{
    if (input.size()!= (unsigned int)n_input)
    {
        /** TO-DO length check doen **/
        //length werkt nog niet!
        //throw exception;
    }
    //Calculate Hiddenlayer nodes
    for(int i = 0; i<n_hidden;i++)
    {
        nHL[i]=0;
        for(int j = 0;j<n_input;j++)
        {
            nHL[i] = nHL[i] + wHL[i][j]*input[j];
        }
        nHL[i] = activationFunction(nHL[i],bHL[i]);
    }
    //Calcute output layer nodes
    for(int i = 0; i<n_output;i++)
    {
        nOL[i]=0;
        for(int j = 0;j<n_hidden;j++)
        {
            nOL[i] = nOL[i] + wOL[i][j]*nHL[j];
        }
        nOL[i] = activationFunction(nOL[i],bOL[i]);
    }
    return nOL;
}
