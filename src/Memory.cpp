/**
 * @file   Memory.cpp
 * @Author fabricio
 * @date   Outubro 2, 2015
 * @brief  Arquivo de implementação da classe Memory.
 */

#include "../include/Memory.hpp"
#include <math.h>
#include <iostream>
#include <unordered_map>

using namespace std;
using namespace wann;

/**
 * Gera um warning caso o número de bits para endereçamento da memória seja superior a 62. (Possibilidade de overflow).
 * Seta o atributo interno numAddrs com o valor de 2 elevado a entrada numBits.
 */
Memory::Memory(int numBits, 
			   bool isCummulative=true, 
			   bool ignoreZeroAddr=false)
:numBits(numBits),isCummulative(isCummulative),ignoreZeroAddr(ignoreZeroAddr)
{
	if(numBits > 62)
		cout << "WARNING: Representation overflow due to number of bits" << endl;

	numAddrs = (long long)pow((long long)2, (long long)numBits);
	
}
/**
 * Deleta o contéudo armazenado pelo membro interno data.
 */
Memory::~Memory(void)
{
	data.clear();
}

/**
 * Se o endereço addr for inferior a zero ou maior que o
 * membro interno numAddrs, gera um warning e encerra a execução.
 * Caso o membro interno isCummulative seja falso, apenas seta o conteúdo
 * associado àquele endereço com 1.
 * Caso não seja, verifica se já há algum conteúdo associado ao endereço.
 * Se não houver, o cria com o valor do parâmetro value. Se houver, incrementa
 * o conteúdo com value.
 */
void Memory::addValue(const long long addr, int value = 1)
{	
	if(addr < 0L || addr >= numAddrs)
	{
		cout << "WARNING: invalid address to add value" << endl;
		cout << "WARNING: number of address: " << numAddrs << endl;
		exit(-1);
	}
	if(!isCummulative)
	{
		data[addr] = 1;
	}
	else
	{
		if(!data.count(addr))
		{
			data[addr] = value;
		}
		else
		{
			data[addr] += value;
		}
	}	
}

/**
 * Se o endereço addr for inferior a zero ou maior que o
 * membro interno numAddrs, gera um warning e encerra a execução.
 * Caso o membro interno ignoreZeroAddr seja verdadeiro e o endereço
 * addr seja zero, retorna 0.
 * Verifica se o conteúdo associado ao endereço já foi inicializado,
 * caso sim, retorna o conteúdo, caso não, retorna zero.
 */
int Memory::getValue(const long long addr)
{
	if(addr < 0 || addr >= numAddrs)
	{
		cout << "WARNING: invalid address to add value" << endl;
		cout << "WARNING: number of address: " << numAddrs << endl;
		exit(-1);
	}
	if(ignoreZeroAddr && addr == 0)
		return 0;
	if(!data.count(addr))
		return 0;

	return data[addr];
}
