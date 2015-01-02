#ifndef NEURAL_H
#define NEURAL_H

#include "functions.h"

class NN
{
  public:
    void init();
  private:
    void initializeNN();
    void trainNN();

    vector< vector<float> > wHL,
                            wOL;
    vector<float> bHL,
                  bOL,
                  nHL,
                  nOL,
                  nIL;
    int n_input,
        n_hidden,
        n_output,
        n_layers;
};

#endif // NEURAL_H
