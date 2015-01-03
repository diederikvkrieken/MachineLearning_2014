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
  for(int i=0; i < n_hidden; i++)
  {
    // Initialise hidden nodes values
    nHL[i] = 0;

    // Initialise hidden nodes bias
    bHL[i] = 0;

    // Initialise hidden nodes weights
    for(int j=0; j < n_input; j++)
    {
      wHL[i][j] = randInt(0, 100) / 100.0;

      // Initialise input nodes values
      // Done every n_hidden loop, optimlize to only once with if or new for.
      nIL[j] = 0;
    }


    // Initialise output nodes
    for(int j=0; j < n_output; j++)
    {
      wOL[j][i] = randInt(0, 100) / 100.0;
      // Initialise output nodes bias
      bOL[j] = 0;
      // Initialise output nodes value
      nOL[j] = 0;
    }
  }
}

void NN::trainNN()
{
    int maxEpoch = 1000;
    int nParticles = 20; //youtube video gives optimal range 20-40
    int trainData = 0; //moet meegegeven worden aan trainNN function
    //exit-error can still be added (see: voorbeeld_code: Iris flowers ML
    int nW = (n_input*n_hidden) + (n_output*n_hidden);


    // swarm initialization
    // initialise each Particle in the swarm with random positions and velocities

    for (int i = 0; i<nParticles;i++)
    {
        //Particle initialization
        for (int j = 0; j<nW;j++)
        {
            // particle position:
            // randomPosition = (hi - lo) * random_double + lo; (waarde tussen max en min)
            p_x[i][j] = ;
            // particle velocity:
            p_v[i][j] = ;
        }

    }

    //Error check?????

    // particle update
    // 1. new velocity
    // 2. compute new position with the new velocity
    // 3. compute new error with the new position

    //Return optimal

}


vector<float> NN::runNN(vector<float> input)
{
    if (input.size()!= n_input)
    {
        //length werkt nog niet!
        //error_code;
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
