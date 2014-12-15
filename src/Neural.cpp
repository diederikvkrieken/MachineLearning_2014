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
    // Initialise hidden nodes
    for(int j=0; j < n_input; j++)
    {
      wHL[i][j] = randInt(0, 100) / 100.0;
    }
    // Initialise output nodes
    for(int j=0; j < n_output; j++)
    {
      wHL[j][i] = randInt(0, 100) / 100.0;
    }
  }
}
