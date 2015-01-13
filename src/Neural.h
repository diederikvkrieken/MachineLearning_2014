#ifndef NEURAL_H
#define NEURAL_H

#include "functions.h"

class NN
{
  public:
    void init();
    void initializeNN();

    void trainNN();
    float calculateError(vector<float> input);
    void positionToWeights(vector<float> postition);
    float activationFunction(float input ,float bias);

    vector<float> runNN(vector<float> input);

    vector< vector<float> > wHL,
                            wOL,
                            p_x,
                            p_v,
                            p_bx;//particle best position

    vector<int> randomOrder;

    vector<float> nHL,
                  bHL,
                  nOL,
                  bOL,
                  nIL,
                  best_global_position,
                  p_be,
                  result;//particle best error
    int n_input,
        n_hidden,
        n_output,
        n_layers;
        //private:
};

#endif // NEURAL_H
