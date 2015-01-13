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
    nIL[i] = 0;
  }
  //initialise output nodes values and bias
  for(int i = 0; i<n_output;i++)
  {
    bOL[i] = 0;
    nOL[i] = 0;
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
    int x_max = 10;
    int x_min = -10;
    int v_max = 1;
    int v_min = -1;
    int maxEpoch = 1000;
    int nParticles = 20; //youtube video gives optimal range 20-40
    int trainData = 0; //moet meegegeven worden aan trainNN function
	float precision = 0.001;
	float c1 = 1.4;
	float c2 = 1.4;
	float w = 1.4;
	float min_error = 0.1;
	 //vector<float> best_global_error = 0;

    //exit-error can still be added (see: voorbeeld_code: Iris flowers ML)
    //number of weights+bias
    int nW = (n_input*n_hidden) + (n_output*n_hidden) + n_hidden + n_output;
    /** TO-DO best global postion vector klopt zo?**/
    // best_global_position = 0;
    /** TO-DO maxValue Controleren **/
    float best_global_error = 100000000;

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
      // Error check?????
      /** TO-DO calculateError, data meegeven?, hoe error uitrekenen? **/
      p_be[i] = calculateError(p_x[i]);
      best_global_position = p_x[i];
      if (p_be[i] < best_global_error)
      {
        best_global_error = p_be[i];
        best_global_position = p_x[i];
      }
      p_bx[i] = p_x[i];//first position is best position
      randomOrder[i]=i;
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
      /** ONDERSTAANDE ZOU MOETEN SHUFFELEN.**/
      std::random_shuffle ( randomOrder.begin(), randomOrder.end() );
      for (int z = 0; z<randomOrder.size();z++) //RandomOrder.length == nParticles
      {
        int i = randomOrder[z];
        // particle update
        for (int j = 0; j<nW;j++)
        {
          // 1. new velocity
          // new velocity = (w * current_v)+(c1 * r1 * p_best_pos - current_pos)+(c2 * r2 * global_best_pos - current_pos)
          p_v[i][j] = (w * p_v[i][j])
          + (c1*randomFloat(0, 1, precision)*(p_bx[i][j]-p_x[i][j]));
          //+ (c2*randomFloat(0, 1, precision)*(global_best_pos[j]-p_x[i][j]));
          // 2. compute new position with the new velocity
          // newrandInt(0, 100) / 100.0Position = current_pos + new velocity;
          p_x[i][j] = p_x[i][j] + p_v[i][j];
        }

        // 3. compute new error with the new position
        double new_error = calculateError(p_x[i]);
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

float NN::calculateError(vector<float> weights )
{
  //JUISTE ANTWOORD/OUTPUT GEVEN/BEPALEN
  //data = trainData;
 //  requiredResult = requiredData;
  positionToWeights(weights);
  //Elke keer data set runnen of 1 voor een, of online learning?
  //result = runNN(data);
  //mean squard error or visa versa
  //MSE(data, result);

}

void NN::positionToWeights(vector<float> position)
{
  int nW = (n_input*n_hidden) + (n_output*n_hidden) + n_hidden + n_output;;
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
    if (input.size()!= n_input)
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
