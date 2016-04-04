/*
 * Discriminator.cpp
 *
 *  Created on: Apr 2, 2016
 *      Author: fabricio
 */

#include "../include/Util.hpp"

using namespace wann;

float util::calculateConfidence(unordered_map<string, int> &result)
{
    int max = 0;
    int secondMax = 0;

    for (auto it = result.begin(); it != result.end(); ++it )
    {
        int value = it->second;

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
    
    float confidence = 1.0 - ( ( (float)secondMax )/max);
    return confidence;
}


float util::maxValue(unordered_map<string, float> &values)
{
    int max = 0;
    for (auto it = values.begin(); it != values.end(); ++it )
    {
        int value = it->second;
        if(max < value)
        {
            max = value;
        }
    }

    return max;
}


float util::argMax(unordered_map<string, float> &values)
{
    int max = 0;
    string maxLabel = "";

    for (auto it = values.begin(); it != values.end(); ++it )
    {
        int value = it->second;
        if(max < value)
        {
            max = value;
            maxLabel = it->first;
        }
    }

    return maxLabel;
}



