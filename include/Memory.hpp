/**
 * @file   Memory.hpp
 * @Author fabricio
 * @date   Outubro 2, 2015
 * @brief  Arquivo de declaração da classe Memory.
 */

#ifndef MEMORY_CPP_
#define MEMORY_CPP_

#include <unordered_map>


namespace wann
{
	/**
	 * Classe responsável por simular e gerenciar acesso às memórias associdas a objetos do tipo Discriminator.
	 * Cada memória é mapeada em uma estrutura do tipo unordered_map.
	 */
	class Memory
	{
		public:
			/**
			 * @brief Construtor da Classe
			 * @param numBits Número de bits a ser usado para se endereçar a memória.
			 * @param isCummulative Flag para sinalizar se o conteúdo da memória deve ser cumulativo.
			 * @param ignoreZeroAddr Flag para sinalizar se a primeira posição da memória deve ser ignorada.
			 */
			Memory(int numBits, bool isCummulative, bool ignoreZeroAddr);

			/**
			 * Destrutor da Classe
			 */
			~Memory(void);
			
			/**
			 * @brief A partir de um endereço, incrementa o conteúdo associado.
			 * @param addr Endereço
			 * @param value Valor
			 */
			void addValue(const long long addr, int value);
			/**
			 * @brief A partir de um endereço, retorna o conteúdo associado.
			 * @param addr Endereço
			 */
			int getValue(const long long addr);

		private:
			/** Estrutura de dados utilizada para simular uma memória.*/
			std::unordered_map<long long, int> data;
			/** Quantidade de endereços de memória utilizados*/
			long long numAddrs;
			/** Número de bits a ser usado para se endereçar a memória.*/
			int numBits;
			/** Flag para sinalizar se o conteúdo da memória deve ser cumulativo.*/
			bool isCummulative;
			/** Flag para sinalizar se a primeira posição da memória deve ser ignorada.*/
			bool ignoreZeroAddr;
	};

}

#endif /* MEMORY_CPP_ */
