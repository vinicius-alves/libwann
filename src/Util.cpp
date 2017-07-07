/**
 * @file   Util.cpp
 * @Author fabricio
 * @date   Abril 2, 2016
 * @brief  Arquivo de declaração das funções do espaço "util".
 */

#include "../include/Util.hpp"

using namespace std;
using namespace wann;

/**
 * Calcula a confiança, que é dada pela diferença entre a maior e a
 * segunda maior porcentagem de acertos de uma dada entrada na rede.
 */
float util::calculateConfidence(unordered_map<string, float> &result)
{
    float max = 0.0;
    float secondMax = 0.0;

    for (auto it = result.begin(); it != result.end(); ++it )
    {
        float value = it->second;

        if(max < value)
        {
            secondMax = max;
            max = value;
        }
        else if(secondMax < value)
        {
            secondMax = value;
        }
    }
    
    float confidence = 1.0 - (secondMax /max);
    return confidence;
}

/**
 * Obtém o maior valor de porcentagem de acertos de uma dada entrada.
 */
float util::maxValue(unordered_map<string, float> &values)
{
    float max = 0.0;
    for (auto it = values.begin(); it != values.end(); ++it )
    {
        float value = it->second;
        if( (value - max) > 0.0001 ) 
        {
            max = value;
        }
    }

    return max;
}

/**
 * Seleciona a label com maior porcentagem de acertos.
 */
string util::argMax(unordered_map<string, float> &values)
{
    float max = 0.0;
    string maxLabel = "";

    for (auto it = values.begin(); it != values.end(); ++it )
    {

        float value = it->second;

        if(max <= value)
        {
            max = value;
            maxLabel = it->first;
        }
    }

    return maxLabel;
}



