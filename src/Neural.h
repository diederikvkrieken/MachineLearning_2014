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
    vector<float> runNN(vector<float> input);

    vector< vector<float> > wHL,
                            wOL,
                            p_x,
                            p_v;
    vector<float> nHL,
                  bHL,
                  nOL,
                  bOL,
                  nIL;
    int n_input,
        n_hidden,
        n_output,
        n_layers;
};

#endif // NEURAL_H
