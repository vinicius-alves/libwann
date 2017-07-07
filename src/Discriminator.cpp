/**
 * @file   Discriminator.cpp
 * @Author fabricio
 * @date   Outubro 2, 2015
 * @brief  Arquivo de implementação da classe Discriminator.
 */

#include "../include/Discriminator.hpp"
#include "../include/Memory.hpp"

#include <cmath>
#include <iostream>

using namespace std;
using namespace wann;

/**
 * Calcula o número de objetos do tipo Memory que serão criados, para isto,
 * utiliza a divisão do membro interno retinaLength pelo outro membro numBitsAddr.
 * Se a divisão é inteira, cria o valor da divisão em objetos do tipo Memory,
 * e os adiciona ao membro interno memories.
 * Se há resto na divisão, faz com que a última memória criada seja endereçada
 * pela quantidade de bits representados pelo resto.
 */
Discriminator::Discriminator(int retinaLength, 
                             int numBits,
                             vector<int> memoryAddressMapping, 
                             bool isCummulative, 
                             bool ignoreZeroAddr)
: retinaLength(retinaLength),
  numBitsAddr(numBits),
  memoryAddressMapping(memoryAddressMapping),
  isCummulative(isCummulative),
  ignoreZeroAddr(ignoreZeroAddr)
{
    numMemories = (int) ceil(((float)retinaLength)/(float)numBits);
    bool hasRestMemory = ((retinaLength % numBits) > 0) ? true : false;

    if(!hasRestMemory)
    {
        for(int i=0; i<numMemories; i++)
            memories.push_back(new Memory(numBits, isCummulative, ignoreZeroAddr));
    }

    else
    {
        for(int i=0; i<numMemories-1; i++)
            memories.push_back(new Memory(numBits, isCummulative, ignoreZeroAddr));

        //the rest memory 
        memories.push_back(new Memory((retinaLength % numBits), isCummulative, ignoreZeroAddr));
    }
}

/**
 * Deleta dinamicamente todas as memórias associadas ao Discriminator.
 */
Discriminator::~Discriminator(void)
{
    for(int i = 0; i < memories.size(); i++)
    {
        delete memories[i];
    }
}

/**
 * Segmenta a entrada em porções definidas pelo membro interno numBitsAddr.
 * O acesso a retina é chaveado pelo membro interno memoryAddressMapping.
 * Assim, cada grupo de bits, com comprimento numBitsAddr é relacionado com um objeto Memory.
 * Em seguida, se incrementa em 1, no objeto Memory associado, o endereço chaveado pelo grupo de bits anterior.
 */
void Discriminator::addTrainning(const vector<int> &retina)
{
    int memIndex;
    long long addr;
    int lastMemoryPosition;
    int restOfPositions;
    long long base;
    
    // each group of numBitsAddr is related with a memory
    for(int i=0; i <= retinaLength-numBitsAddr; i+= numBitsAddr)
    {
        addr = 0LL;
        base = 1LL;

        for(int j=0; j < numBitsAddr; j++)
        {
            if(retina[memoryAddressMapping[i+j]] != 0)
                addr += base;

            base *= 2LL;
        }
        memIndex = (int) ( i / numBitsAddr);
        memories[memIndex]->addValue(addr, 1);

    }
    //  the rest of the retina (when the retina length is not a multiple of number of bits of address)
    restOfPositions = retinaLength % numBitsAddr;    
    if(restOfPositions != 0)
    {
        addr = 0LL;
        base = 1LL;
        for(int j=0; j< restOfPositions; j++)
        {
            if(retina[memoryAddressMapping[retinaLength - restOfPositions - 1 +j]] != 0)
                addr += base;

            base *= 2LL;
        }
        lastMemoryPosition = memIndex + 1;
        memories[lastMemoryPosition]->addValue(addr, 1);
    }
}

/**
 * Cria um vetor de inteiros, result, a ser retornado pelo método.
 * Segmenta a entrada em porções definidas pelo membro interno numBitsAddr.
 * O acesso a retina é chaveado pelo membro interno memoryAddressMapping.
 * Assim, cada grupo de bits, com comprimento numBitsAddr é relacionado com um objeto Memory.
 * Em seguida, obtém o conteúdo do objeto Memory associado, endereçado pelo grupo de bits anterior.
 * Adiciona este conteúdo ao vetor results, para cada memória associada.
 */
vector<int> Discriminator::predict(const vector<int> &retina)
{
    long long addr;
    int memIndex;
    int restOfPositions;
    int lastMemoryPosition;
    long long base;
    vector<int> result(numMemories);

    // each group of numBitsAddr is related with a memory
    for(int i=0; i <= retinaLength-numBitsAddr; i+= numBitsAddr)
    {
        addr = 0LL;
        base = 1LL;
        for(int j=0; j<numBitsAddr; j++)
        {
            if(retina[memoryAddressMapping[i+j]] != 0)
                addr += base;

            base *= 2LL;
        }
        memIndex = (int) ( i / numBitsAddr );
        result.push_back(memories[memIndex]->getValue(addr));
    }
    //  the rest of the retina (when the retina length is not a multiple of number of bits of address)   
    restOfPositions = retinaLength % numBitsAddr;
    addr = 0L;
    base = 1L;
    if(restOfPositions != 0)
    {
        for(int j=0; j< restOfPositions; j++)
        {
            if(retina[memoryAddressMapping[retinaLength - restOfPositions - 1 + j]] != 0)
                addr += (long) base;

            base *= 2;
        }
        lastMemoryPosition = memIndex + 1;
        result.push_back(memories[lastMemoryPosition]->getValue(addr));
    }
    return result;
}


