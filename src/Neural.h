#ifndef NEURAL_H
#define NEURAL_H

#include "functions.h"

class NN
{
  public:
    void initializeNN();
    void trainNN();
    void positionToWeights(vector<float> postition);
    float activationFunction(float input ,float bias);
    vector<float> runNN(vector<float> input);

    vector< vector<float> > wHL,
                            wOL,
                            p_x,
                            p_v,
                            p_bx;//particle best position

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
        n_layers,
        maxEpoch,
        nParticles;
        //private:

    float x_max,
          x_min,
          v_max,
          v_min,
          precision,
          c1,
          c2,
          w;

    string input_text;
};

#endif // NEURAL_H
