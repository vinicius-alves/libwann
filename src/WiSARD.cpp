/*
 * Discriminator.cpp
 *
 *  Created on: Oct 2, 2015
 *      Author: fabricio
 */

#include "../include/WiSARD.hpp"
#include "../include/Discriminator.hpp"

#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>
#include <string>


using namespace std;


WiSARD::WiSARD(int retinaLength, 
			   int numBitsAddr, 
			   bool useBleaching, 
			   float confidenceThreshold, 
			   int defaultBleaching_b, 
			   bool randomizePositions, 
			   bool isCummulative, 
			   bool ignoreZeroAddr)

:retinaLength(retinaLength), 
 numBitsAddr(numBitsAddr),
 useBleaching(useBleaching),
 confidenceThreshold(confidenceThreshold),
 defaultBleaching_b(defaultBleaching_b),
 randomizePositions(randomizePositions),
 isCummulative(isCummulative),
 ignoreZeroAddr(ignoreZeroAddr)
{
	
	for(int i=0; i < retinaLength; i++)
		memoryAddressMapping.push_back(i);

	if(randomizePositions)
	{	
		seed = chrono::system_clock::now().time_since_epoch().count();
		shuffle(begin(memoryAddressMapping), end(memoryAddressMapping), default_random_engine(seed));
	}
}

WiSARD::~WiSARD(void)
{
	for (auto it = discriminators.begin(); it != discriminators.end(); ++it )
	{
		delete it->second;
	}
}
Discriminator * WiSARD::getDiscriminator(string label)
{
	return discriminators[label];
}

void WiSARD::createDiscriminator(string discriminatorLabel)
{
	discriminators[discriminatorLabel] = new Discriminator(retinaLength, numBitsAddr, memoryAddressMapping, isCummulative, ignoreZeroAddr);
}


void WiSARD::fit(const vector< vector<int> > &X, const vector<string> &y)
{
	for(int i=0; i < y.size(); i++)
	{
		string label = y[i];
		discriminators[label]->addTrainning(X[i]);
	}	
}

unordered_map<string, int> WiSARD::predict(const vector<int> &retina)
{
	unordered_map<string, int> result;
	unordered_map<string, vector<int>> memoryResult;
	string label;
	Discriminator * auxDisc;
	vector<int> memoryResultAux;
	
	for (auto it = discriminators.begin(); it != discriminators.end(); ++it )
	{
		label = it->first;
		auxDisc = it->second;
		memoryResultAux = auxDisc->predict(retina);
	
		int sumMemoriesValue = 0;
		for(int i = 0; i < memoryResultAux.size(); i++)
		{
			if(memoryResultAux[i] > 0)
				sumMemoriesValue += 1;
		}
		result[label] = sumMemoriesValue;
		memoryResult[label] = memoryResultAux;
	}

	if( ! useBleaching)
	{
		return result;
	}
	else
	{
		//apply bleaching
		float confidence = calculateConfidence(result);
		int b = defaultBleaching_b;
		int counter = 0;

		// cout << "result before bleaching" << endl;
		// for (auto it = result.begin(); it != result.end(); ++it )
		// {
		// 	label = it->first;
		// 	int s = it->second;
		// 	cout << "class: " << label << " result: " << s << endl;
			
		// }
		while(confidence < confidenceThreshold)
		{
			for (auto it = result.begin(); it != result.end(); ++it )
			{
				label = it->first;
				int sumMemoriesValue = 0;

				const std::vector<int> &labelResult = memoryResult[label];
				for(int i = 0; i < labelResult.size(); i++)
				{

					if(labelResult[i] > b)
					//if(memoryResult[label][i] > b)
						sumMemoriesValue += 1;
					
				}
				

				result[label] = sumMemoriesValue;
			}
			
			counter++;
			b+=defaultBleaching_b;
			confidence = calculateConfidence(result);
		}
		//cout << "entered " << counter << endl;
		// cout << "result after bleaching" << endl;
		// for (auto it = result.begin(); it != result.end(); ++it )
		// {
		// 	label = it->first;
		// 	int s = it->second;
		// 	cout << "class: " << label << " result: " << s << endl;
			
		// }
		// cout << "------" << endl;

		return result;
	}

}

float WiSARD::calculateConfidence(unordered_map<string, int> &result)
{
	int max = 0;
	int secondMax = 0;

	for (auto it = result.begin(); it != result.end(); ++it )
	{
		int value = it->second;

		if (value > max)
		{
			//secondMax = max;
			max = value;
		}	
		if(value <= max && value >= secondMax )
			secondMax = value;
	}
	
	float confidence = 1.0 - ( ( (float)secondMax )/max);
	return confidence;
}


