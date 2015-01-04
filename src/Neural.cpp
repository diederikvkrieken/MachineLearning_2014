#include "Neural.h"

void NN::init()
{
  n_layers = 1;
  n_input = 26;
  n_hidden = 33;
  n_output = 3;
}

void NN::initializeNN()
{
  // Initialise input nodes values
  for(int i=0; i < n_input; i++)
  {
    nIL[j] = 0;
  }
  //initialise output nodes values and bias
  for(int i = 0; i<n_output;i++)
  {
    bOL[j] = 0;
    nOL[j] = 0;
  }
  for(int i=0; i < n_hidden; i++)
  {
    // Initialise hidden nodes values
    nHL[i] = 0;
    // Initialise hidden nodes bias
    bHL[i] = 0;
    // Initialise hidden - input weights
    for(int j=0; j < n_input; j++)
    {
      wHL[i][j] = randInt(0, 100) / 100.0;
    }
    // Initialise output-hidden weights
    for(int j=0; j < n_output; j++)
    {
      wOL[j][i] = randInt(0, 100) / 100.0;
    }
  }
}

void NN::trainNN()
{
    /** x_max x_min v_max v_min **/
    int maxEpoch = 1000;
    int nParticles = 20; //youtube video gives optimal range 20-40
    int trainData = 0; //moet meegegeven worden aan trainNN function
    //exit-error can still be added (see: voorbeeld_code: Iris flowers ML)
    //number of weights+bias
    int nW = (n_input*n_hidden) + (n_output*n_hidden) + n_hidden + n_output;
    /** TO-DO RANDOM FUNCTION Controleren **/
    rnd = rand();
    /** TO-DO best global postion vector klopt zo?**/
    best_global_position = 0;
    /** TO-DO maxValue Controleren **/
    double best_global_error = double.maxValue;

    // swarm initialization
    // initialise each Particle in the swarm with random positions and velocities

    for (int i = 0; i<nParticles;i++)
    {
      //Particle initialization
      for (int j = 0; j<nW;j++)
      {
        // particle position:
        // randomPosition = (hi - lo) * random_double + lo; (waarde tussen max en min)
        p_x[i][j] = (x_max - x_min) * rnd.NextDouble() + x_min;
        // particle velocity:
        // randomVelocity[j] = (hi - lo) * rnd.NextDouble() + lo;
        p_v[i][j] = (v_max - v_min) * rnd.NextDouble() + v_min;
      }
      // Error check?????
      /** TO-DO calculateError, data meegeven?, hoe error uitrekenen? **/
      p_be[i] = calculateError(p_x[i], data);
      if (p_be[i] < best_global_error)
      {
        best_global_error = p_be[i];
        best_global_position = p_x[i];
      }
      p_bx[i] = p_x[i];//first position is best position
    }

    /** TO-DO particle volgorde randomizeren **/
    // max epochs + check/exit bij kleine error
    for (int epoch = 0; epoch<maxEpoch;epoch++)
    {
      if (best_global_error<min_error)
      {
        /** TO-DO Break correct implementeren**/
        break;
      }
      //update each particle
      for (int i = 0; i<nParticles;i++)
      {
        // particle update
        for (int j = 0; j<nW;j++)
        {
          // 1. new velocity
          // new velocity = (w * current_v)+(c1 * r1 * p_best_pos - current_pos)+(c2 * r2 * global_best_pos - current_pos)
          p_v[i][j] = (w * p_v[i][j])
          + (c1*rnd.NextDouble()*(p_bx[i][j]-p_x[i][j]))
          + (c2*rnd.NextDouble()*(global_best_pos[j]-p_x[i][j]));
          // 2. compute new position with the new velocity
          // newPosition = current_pos + new velocity;
          p_x[i][j] = p_x[i][j] + p_v[i][j];
        }

        // 3. compute new error with the new position
        double new_error = calculateError(p_x[i], data);
        if (new_error < best_global_error)
        {
          best_global_error = p_be[i];
          best_global_position = p_x[i];
        }
        if (new_error < p_be[i])
        {
          p_be[i] = new_error;
          p_bx[i] = p_x[i];//current position is better position
        }

      }
    }
    /** TO-DO return best_global_position of gewoon wegschrijven?**/
    //return best_global_postion;
    //Return optimal

}

double NN::calculateError(vector<float> weights, data)
{
  //JUISTE ANTWOORD/OUTPUT GEVEN/BEPALEN
  data = trainData;
  requiredResult = requiredData;
  positionToWeights(weights);
  //Elke keer data set runnen of 1 voor een, of online learning?
  result = runNN(data);
  //mean squard error or visa versa
  MSE(data, result);

}

void positionToWeights(vector<float> postition)
{
  int nW = (n_input*n_hidden) + (n_output*n_hidden) + n_hidden + n_output;;
  if (postition.Length != nW)
  {
    /** TO-DO error **/
    //throw exception??
    //error!!
  }
  i=0;
  for(int j=0; j < n_hidden; j++)
  {
    // hidden to input nodes weights
    for(int k=0; k < n_input; k++)
    {
      wHL[j][k] = postition[i];
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
  for(int j=0; j < n_output; j++)
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

vector<float> NN::runNN(vector<float> input)
{
    if (input.size()!= n_input)
    {
        /** TO-DO length check doen **/
        //length werkt nog niet!
        //trow exception;
    }
    //Calculate Hiddenlayer nodes
    for(int i = 0; i<n_hidden;i++)
    {
        nHL[i]=0;
        for(int j = 0;j<n_input;j++)
        {
            nHL[i] = nHL[i] + wHL[i][j]*input[j];
        }
        nHL[i] = nHL[i] + bHL[i];
    }
    /** TO-DO tanh of sigmoid function gebruiken **/
    //FUNCTION IMPLIMENTATION TANH OR SIGMOID

    //Calcute output layer nodes
    for(int i = 0; i<n_output;i++)
    {
        nOL[i]=0;
        for(int j = 0;j<n_hidden;j++)
        {
            nOL[i] = nOL[i] + wOL[i][j]*nHL[j];
        }
        nOL[i] = nOL[i] + bOL[i];
    }
    return nOL;
}
