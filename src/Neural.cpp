#include "Neural.h"

void NN::initializeNN()
{
  n_layers = 1;
  n_input = 22;
  n_hidden = 50;
  n_output = 3;
  nW = (n_input*n_hidden) + (n_output*n_hidden) + n_hidden + n_output;
  input_text = "result_100pp_cont_25particles_200_epoch_50_n_hidden.csv";

  /** Parameters of the particle swarm **/
  //youtube video gives optimal range 20-40
  maxEpoch = 200; // was 1000
  nParticles = 25; //was 20
  /** x_max x_min v_max v_min **/
  x_max = 1.0f;
  x_min = -1.0f;
  v_max = 0.1f;
  v_min = -0.1f;
  precision = 0.001f; // Precision for randomFloat function
  c1 = 1.4f;
  c2 = 1.4f;
  w = 0.8f;
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
      wHL[i][j] = randInt(-100, 100) / 100.0;
    }
    // Initialise output-hidden weights
    for(int j=0; j < n_output; j++)
    {
      wOL[j][i] = randInt(-100, 100) / 100.0;
    }
  }

  //number of weights+bias
  /** TODO: maxValue Controleren **/
  best_global_error = 100000000;
  best_global_position.assign(nW,0);

  // Allocate particles
  for(int i=0; i < nParticles; i++)
  {
    inside_vector.assign(nW,0);
    p_x.push_back(inside_vector);
    p_v.push_back(inside_vector);
    p_bx.push_back(inside_vector);
  }
  // swarm initialization
  // initialise each Particle in the swarm with random positions and velocities
  for (int i = 0; i<nParticles;i++)
  {
    //Particle initialization
    for (unsigned int j = 0; j<nW;j++)
    {
      // particle position:
      // randomPosition = (hi - lo) * random_double + lo; (waarde tussen max en min)
      p_x[i][j] = (x_max - x_min) * randomFloat(0, 1, precision) + x_min;
      // particle velocity:
      // randomVelocity[j] = (hi - lo) * rnd.NextDouble() + lo;
      p_v[i][j] = (v_max - v_min) * randomFloat(0, 1, precision) + v_min;
      /*printf("p_x %d %d = %.2f\n", i, j, p_x[i][j]);
      printf("p_v %d %d = %.2f\n", i, j, p_v[i][j]);*/
    }
  }
  p_be.assign(nParticles,9999999);
}

void NN::trainNN(vector <int> result)
{
  // write results
  ofstream results;
  results.open(input_text.c_str(), std::ios::app);
  if (results.is_open())
  {
    for(unsigned int i=0; i < result.size(); i++)
    {
      results << result[i] << "; ";
    }
    results << "\n";
    results.close();
  }
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
    for(unsigned int j = 0; j<nW;j++)
    {
      // 1. new velocity
      // new velocity = (w * current_v)+(c1 * r1 * p_best_pos - current_pos)+(c2 * r2 * global_best_pos - current_pos)
      p_v[i][j] = (w * p_v[i][j])
      + (c1*randomFloat(0, 1, precision)*(p_bx[i][j]-p_x[i][j]))
      + (c2*randomFloat(0, 1, precision)*(best_global_position[j]-p_x[i][j]));

      // Ensure smallest/largest
      clamp(&p_v[i][j], v_min, v_max);

      // 2. compute new position with the new velocity
      // Position = current_pos + new velocity;
      p_x[i][j] = p_x[i][j] + p_v[i][j];

      // Ensure smallest/largest
      clamp(&p_x[i][j], x_min, x_max);
    }
  }
}

void NN::positionToWeights(int particle)
{
  vector<float> position = p_x[particle];
  if (position.size() != nW)
  {
      fprintf(stderr, "[NN.positionToWeights] length of position != nW (length(position) = %d , nW = %d). Particle: %d\n", position.size(), nW, particle);
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
	/*printf("input: %.2f\n", input);
	printf("bias: %.2f\n", bias);*/
	/*return 1/(1+expf(-x));*/
	// tanh function
	return tanhf(x);
}

vector<float> NN::runNN(vector<float> input)
{
    if (input.size()!= (unsigned int)n_input)
    {
        fprintf(stderr, "[NN.runNN] length of input != n_input.\n");
    }
    //Calculate Hiddenlayer nodes
    for(int i = 0; i<n_hidden;i++)
    {
        nHL[i]=0;
        for(int j = 0;j<n_input;j++)
        {
            /* printf("wHL[i][j]: %.2f\n", wHL[i][j]);
            printf("input[j]: %.2f\n", input[j]);*/
            nHL[i] += wHL[i][j]*input[j];
        }
        nHL[i] = activationFunction(nHL[i],bHL[i]);
        /*printf("nHL %d = %.2f -- ", i, nHL[i]);*/
    }
    /*printf("\n");*/
    //Calcute output layer nodes
    for(int i = 0; i<n_output;i++)
    {
        nOL[i]=0;
        for(int j = 0;j<n_hidden;j++)
        {
            nOL[i] += wOL[i][j]*nHL[j];
        }
        nOL[i] = activationFunction(nOL[i],bOL[i]);
        /*printf("output %d = %.2f -- ", i, nOL[i]);*/
    }
    /*printf("\n");*/
    return nOL;
}

vector<float> NN::runHandAlgorithm(vector<float> input)
{
  /**
  FOLLOWING INPUT ORDER:
  0. closest_wall_distance
  1. exit_distance
  2. mean_age
  3. mean_direction.x
  4. mean_direction.y
  5. mean_height
  6. mean_panic
  7. mean_radius
  8. n_people
  9. n_walls
  10. var_age
  11. var_direction.x
  12. var_direction.y
  13. var_height
  14. var_panic
  15. var_radius
  16. h->direction.x
  17. h->direction.y
  18. h->age
  19. h->height
  20. h->panic
  21. h->radius


  FOLLOWING OUPUT ORDER:
  dim2 direction; (x,y)
  float panic;

  **/
  float x_dir = input[3];
  float y_dir = input[4];
  float panic = input[6];
  printf("x_dir = %.2f. y_dir = %.2f\n",x_dir, y_dir );


  vector <float> output;
  output.push_back(x_dir);
  output.push_back(y_dir);
  output.push_back(panic);
  return output;
}

vector<float> NN::getParticleWeights(int particle)
{
  return p_x[particle];
}

vector<float> NN::getParticleSpeed(int particle)
{
  return p_v[particle];
}

vector<float> NN::getParticleBestWeights(int particle)
{
  return p_bx[particle];
}

vector<float> NN::getBestWeights()
{
  return best_global_position;
}

void NN::setParticleWeights(int particle, vector<float> weights)
{
  p_x[particle] = weights;
}

void NN::setParticleSpeed(int particle, vector<float> weights)
{
  p_v[particle] = weights;
}

void NN::setParticleBestWeights(int particle, vector<float> weights)
{
  p_bx[particle] = weights;
}

void NN::setBestWeights(vector<float> weights)
{
  best_global_position = weights;
}

//void NN::wegschrijvenParticles(string text_name)
//{
//  // write results
//  ofstream results;
//  results.open(text_name.c_str(), std::ios::app);
//  if (results.is_open())
//  {
//    for(unsigned int i=0; i < p_x.size(); i++)
//    {
//      results << "particle: " << p_x[i] <<"; ";
//      for(int j=0; j < p_x[i].size(); j++)
//      {
//        results << p_x[i][j] << "; ";
//      }
//      results << "\n";
//    }
//    results.close();
//  }
//}
