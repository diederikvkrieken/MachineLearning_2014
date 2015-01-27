#ifndef NEURAL_H
#define NEURAL_H

#include <fstream>

#include "functions.h"

class NN
{
  public:
    void initializeNN();
    void trainNN(vector <int> result);
    void positionToWeights(int particle);
//    void wegschrijvenParticles(string text_name);
    float activationFunction(float input ,float bias);
    vector<float> runNN(vector<float> input);

    vector<float> getParticleWeights(int particle);
    vector<float> getParticleSpeed(int particle);
    vector<float> getParticleBestWeights(int particle);
    vector<float> getBestWeights();

    void setParticleWeights(int particle, vector<float> weights);
    void setParticleSpeed(int particle, vector<float> weights);
    void setParticleBestWeights(int particle, vector<float> weights);
    void setBestWeights(vector<float> weights);

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
    unsigned int nW; // number of weights per particle

    float x_max,
          x_min,
          v_max,
          v_min,
          precision,
          c1,
          c2,
          w;
    float best_global_error;

    string input_text;
};

#endif // NEURAL_H
