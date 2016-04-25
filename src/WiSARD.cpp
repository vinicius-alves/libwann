/*
 * Discriminator.cpp
 *
 *  Created on: Oct 2, 2015
 *      Author: fabricio
 */

#include "../include/WiSARD.hpp"
#include "../include/Discriminator.hpp"
#include "../include/Util.hpp"

#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>
#include <string>

using namespace wann;
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


void WiSARD::fit(const vector< vector<int> > &X, const vector<string> &y)
{
	// get unique labels
	vector<string> labels(y);
	sort(labels.begin(), labels.end());
	labels.erase(unique(labels.begin(), labels.end()), labels.end() );

	//creating discriminators
	for(int i = 0; i < labels.size(); i++ )
	{
		string label = labels[i];
		Discriminator d = Discriminator(retinaLength,
										numBitsAddr, 
										memoryAddressMapping, 
										isCummulative, 
										ignoreZeroAddr);
		discriminators[label] = &d;
	}	

	//training discriminators
	for(int i=0; i < y.size(); i++)
	{
		string label = y[i];
		discriminators[label]->addTrainning(X[i]);
	}	
}


vector<unordered_map<string, float>> WiSARD::predictProba(const vector< vector<int> > &X)
{
	vector<unordered_map<string, float>> results;
	int numMemories = pow(2, numBitsAddr);

	//for each retina
	for(int i=0; i < X.size(); i++)
	{
		string label;

		unordered_map<string, float> result;
		unordered_map<string, vector<int>> memoryResult;

		Discriminator *auxDisc;
		vector<int> memoryResultAux;

		//getting a retina
		vector<int> retina = X[i];

		// for each discriminator
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

			// to calc probability, what percentage of memories recognize the element;
			result[label] = sumMemoriesValue / numMemories;
			memoryResult[label] = memoryResultAux;
		}

		if(useBleaching)
			WiSARD::applyBleaching(result, memoryResult);
		
		results.push_back(result);
	}	

	return results;
} 


vector<string> WiSARD::predict(const vector< vector<int> > &X)
{
	vector<string> vecRes;
	vector<unordered_map<string, float>>  results = WiSARD::predictProba(X);

	for(int i=0; i< results.size(); i++)
	{
		unordered_map<string, float> result = results[i];
		string resultLabel = util::argMax(result);
		vecRes.push_back(resultLabel);
	}

	return vecRes;
}

void WiSARD::applyBleaching(unordered_map<string, float> &result,  unordered_map<string, vector<int>> &memoryResult)
{
	//apply bleaching
	vector<int> labelResult;

	unordered_map<string, float> resultPrevious;
	int b = defaultBleaching_b;
	int numMemories = pow(2, numBitsAddr);
	string label;

	float confidence = util::calculateConfidence(result);

	while(confidence < confidenceThreshold)
	{

		resultPrevious = result;
		
		for(auto it = result.begin(); it != result.end(); ++it )
		{
			label = it->first;
			int sumMemoriesValue = 0;
		

			labelResult = memoryResult[label];

			for(int i = 0; i < labelResult.size(); i++)
			{
				if(labelResult[i] > b)
					sumMemoriesValue += 1;
				cout << i << endl;
			}

			result[label] = sumMemoriesValue / numMemories;
		}

		// if no memory recognize the pattern, return previous value
		float maxValue = util::maxValue(result);
		if(maxValue <= 0.0000001)  // if is zero
		{	
			result = resultPrevious;
			break;
		}

		b ++;
		confidence = util::calculateConfidence(result);
	}
}
