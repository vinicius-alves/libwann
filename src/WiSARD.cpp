/**
 * @file   WiSARD.cpp
 * @Author fabricio
 * @date   Outubro 2, 2015
 * @brief  Arquivo de implementação da classe WiSARD.
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

/**
 * Preenche o membro interno "memoryAddressMapping" com a sequência 0,1,2,3... até retinaLength.
 * Caso randomizePositions seja verdadeiro, cria uma semente aleatória, baseada na hora atual,
 * salva seu conteúdo no membro interno "seed" e a utiliza  para embaralhar
 * o conteúdo do vetor "memoryAddressMapping".
 */
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

/**
 * Deleta dinâmicamente todos os objetos "discriminators"
 * associados a WiSARD criada.
 */
WiSARD::~WiSARD(void)
{
	for (auto it = discriminators.begin(); it != discriminators.end(); ++it )
	{
		delete it->second;
	}
}

/**
 * Inicialmente, utiliza um mapa não ordenado para obter valores únicos das labels
 * representadas pela entrada y. En seguida, cria um objeto Discriminator para cada
 * label e os armazena no membro interno discriminators.
 * Após, treina cada Discriminator com todos as entradas de X associadas à label
 * daquele Discriminator.
 */
void WiSARD::fit(const vector< vector<int> > &X, const vector<string> &y)
{
	vector<string> labels;
	unordered_map <string, int> auxMap ;
	for(int i = 0; i < y.size(); i++)
	{
		auxMap[y[i]] = 0; 
	}
	for(auto it = auxMap.begin(); it != auxMap.end(); ++it)
	{
		labels.push_back(it->first);
	}

	for(int i = 0; i < labels.size(); i++ )
	{
		string label = labels[i];
	
		Discriminator *d = new Discriminator(retinaLength,
											 numBitsAddr, 
											 memoryAddressMapping, 
											 isCummulative, 
											 ignoreZeroAddr);
		discriminators[label] = d;
	}	

	for(int i=0; i < y.size(); i++)
	{
		string label = y[i];
		discriminators[label]->addTrainning(X[i]);
	}	
}

/**
 * Para cada entrada a ser testada, chama o método predict de todos os discriminadores
 * da rede. Este, retorna um vetor com os conteúdos das posições de memória associadas
 * àquela entrada, por aquele discriminador. É considerado que uma memória do discriminador
 * foi ativada caso seu conteúdo endereçado possua valor maior que zero.
 * A métrica utilizada para a avaliação do discriminador é porcentagem de memórias ativadas.
 * Obtendo esta quantidade, é criado um unordered map, result, contendo a porcentagem de memórias
 * ativadas para cada label em relação à retina selecionada.
 * Caso o membro interno "useBleaching" seja verdadeiro, chama o método applyBleaching sobre o result.
 * Em seguida, este result é adicionado ao vetor de resultados a ser retornado.
 */
vector<unordered_map<string, float>> WiSARD::predictProba(const vector< vector<int> > &X)
{
	vector<unordered_map<string, float>> results;
	int numMemories =  (int) ceil( (float)retinaLength/ (float) numBitsAddr );

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
			result[label] = (float)sumMemoriesValue / (float)numMemories;
			memoryResult[label] = memoryResultAux;
		}
		

		if(useBleaching)
			result = WiSARD::applyBleaching(result, memoryResult);

		results.push_back(result);
	}	

	return results;
} 

/**
 * Utiliza o membro predictProba para receber uma unordered_map contendo a porcentagem
 * de memórias ativadas para cada label de uma linha do vetor de entrada.
 * Em seguida, seleciona a label que obteve maior porcentagem de memórias ativadas.
 * Após, adiciona esta label ao vetor a ser retornado, de predições.
 */
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
/**
 * Carrega o valor defaultBleaching_b em uma variável interna b. Em seguida, entra em um loop.
 * Nele, realiza o mesmo cálculo que é feito no membro predictProba, com a diferença de que agora as memórias
 * consideradas como ativadas são aquelas cujo conteúdo é maior que b.
 * O membro ficará em loop enquanto a confiança for menor que o membro interno confidenceThresold.
 * A cada loop, o bleaching, representado por b, é incrementado em uma unidade.
 * Se o maior resultado das porcentagens das memórias for muito próximo de zero,
 * para algum dado bleaching no loop, para o loop e retorna o valor inicial sem bleaching
 * aplicado.
 */
unordered_map<string, float> WiSARD::applyBleaching(unordered_map<string, float> &result,  unordered_map<string, vector<int>> &memoryResult)
{
	unordered_map<string, float> resultFinal = result;

	//apply bleaching
	vector<int> labelResult;
	string label;
	int b = defaultBleaching_b;
	int numMemories = (int) ceil( (float)retinaLength/ (float) numBitsAddr );
	

	float confidence = util::calculateConfidence(resultFinal);

	while(confidence < confidenceThreshold)
	{

		for(auto it = resultFinal.begin(); it != resultFinal.end(); ++it )
		{
			label = it->first;
			int sumMemoriesValue = 0;
		
			labelResult = memoryResult[label];

			for(int i = 0; i < labelResult.size(); i++)
			{
				if(labelResult[i] > b)
					sumMemoriesValue += 1;
			}

			resultFinal[label] = ((float) sumMemoriesValue / (float) numMemories);
		}

		// if no memory recognize the pattern, return previous value
		float maxValue = util::maxValue(resultFinal);
		
		if(maxValue <= 0.000001)  // if is zero
		{	
			resultFinal = result;
			break;
		}

		b ++;
		confidence = util::calculateConfidence(resultFinal);
	}
	
	return resultFinal;
}
