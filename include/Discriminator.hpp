/**
 * @file   Discriminator.hpp
 * @Author fabricio
 * @date   Outubro 2, 2015
 * @brief  Arquivo de declaração da classe Discriminator.
 */

#ifndef DISCRIMINATOR_HPP_
#define DISCRIMINATOR_HPP_

#include "./Memory.hpp"
#include <vector>
 

namespace wann
{
	/**
	 * Classe responsável pelo treinamento e predição de uma dada entrada.
	 * Para isto, a entrada é mapeada em um conjunto de objetos Memory.
	 * A quantidade destes objetos depende do parâmetro numBits.
	 * A escolha do mapeamento da entrada nas memórias é feita através
	 * do vetor memoryAddressMapping.
	 */
	class Discriminator
	{
		public:

		   /**
		    * @brief Construtor da classe.
		    * @param retinaLength Comprimento da retina.
		    * @param numBits Número de bits a ser utilizado para endereçamento.
		    * @param memoryAddressMapping Vetor auxiliar, utilizado para auxiliar o endereçamento das retinas.
		    * @param isCummulative Flag para sinalizar se o conteúdo das memórias associdas ao discriminador é cumulativo.
		    * @param ignoreZeroAddr Flag para sinalizar se o primeiro enedereço das memórias deve ser omitido na análise.
		    */
			Discriminator(int retinaLength, 
						  int numBits, 
						  std::vector<int> memoryAddressMapping, 
						  bool isCummulative = true, 
						  bool ignoreZeroAddr = false);

			/**
			 * @brief Destrutor da classe.
			 */
			~Discriminator(void);

			/**
			 * @brief Treina o discriminador com uma entrada retina.
			 * @param retina Vetor de bits associado a mesma label do discriminador.
			 */
			void addTrainning(const std::vector<int> &retina);

			/**
			 * @brief Recebe uma retina e a partir dela, retorna um vetor com os conteúdos das memórias associadas.
			 * @param retina Vetor de bits a ser utilizado para endereçamento pelo discriminador.
			 * @return Vetor com o conteúdo de cada memória, endereçado por uma porção de bits da retina.
			 */
			std::vector<int> predict(const std::vector<int> &retina);

		private:
			/** Comprimento da retina.*/
			int retinaLength;
			/** Número de bits a ser utilizado para endereçamento.*/
			int numBitsAddr;
			/** Número de memorias utilizadas pelo discriminador.*/
			int numMemories;
			/** Flag para sinalizar se o conteúdo das memórias associdas ao discriminador é cumulativo.*/
			bool isCummulative;
			/** Flag para sinalizar se o primeiro enedereço das memórias deve ser omitido na análise.*/
			bool ignoreZeroAddr;
			/** Vetor de objetos Memory associados ao objeto Discriminator*/
			std::vector<Memory *> memories;
			/** Vetor auxiliar, utilizado para auxiliar o endereçamento das retinas.*/
			std::vector<int> memoryAddressMapping;
			//Memory * getMemory(int addr);
	};

}

#endif /* DISCRIMINATOR_HPP_ */
