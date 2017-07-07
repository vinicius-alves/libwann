/**
 * @file   WiSARD.hpp
 * @Author fabricio
 * @date   Outubro 2, 2015
 * @brief  Arquivo de declaração da classe WiSARD.
 */

#ifndef WISARD_HPP_
#define WISARD_HPP_

#include "./Discriminator.hpp"

#include <vector>
#include <string>
#include <unordered_map>


namespace wann
{
	/** Classe responsável pela criação e gerenciamento de discriminadores
	 * associados a uma dada entrada e a uma configuração inicial da rede.
	 * Também responsável pela interface externa da biblioteca.
	 */
	class WiSARD
	{
		public:
			/**
			 * @brief Construtor da classe
			 * @param retinaLength Comprimento da retina.
			 * @param numBitsAddr Número de bits a ser utilizado para endereçamento.
			 * @param useBleaching Flag para sinalizar se deve-se ou não utilizar bleaching.
			 * @param confidenceThreshold  Valor limite para a confiança, que é a diferença percentual entre os dois melhores resultados da rede.
			 * @param defaultBleaching_b Valor padrão para o bleaching.
			 * @param randomizePositions Flag para sinalizar se o mapeamento da entrada deve ser randômico.
			 * @param isCummulative Flag para sinalizar se o conteúdo das memórias associadas à rede é cumulativo.
			 * @param ignoreZeroAddr Flag para sinalizar se o primeiro enedereço das memórias deve ser omitido na análise.
			 */
			WiSARD(int retinaLength, 
				   int numBitsAddr, 
				   bool useBleaching=true, 
				   float confidenceThreshold=0.1, 
				   int defaultBleaching_b=1, 
				   bool randomizePositions=true, 
				   bool isCummulative=true, 
				   bool ignoreZeroAddr=false);

			/**
			 * @brief Destrutor da classe
			 */
			~WiSARD(void);

			/**
			 * @brief Método responsável pela criação e treinamento de objetos do tipo
			 * Discriminator, associados as entradas.
			 * @param X Matriz de inteiros, cada linha é uma entrada a ser treinada pela rede.
			 * @param y Vetor de labels, deve existir exatamente uma label para cada entrada.
			 */
			void fit(const std::vector< std::vector<int> > &X, const std::vector<std::string> &y);

			/**
			 * @brief Seleciona uma label para cada entrada, na qual a label selecionado teve maior porcentagem de memórias ativadas.
			 * @param X Matriz de inteiros, cada linha é uma entrada a ser classificada pela rede.
			 * @return Vetor de strings, cada string representa a label na qual ocorreu maior porcentagem de memórias ativadas.
			 */
			std::vector<std::string> predict(const std::vector< std::vector<int> > &X);
			
			/**
			 * @brief Calcula a porcentagem de memórias ativadas para cada discriminador de cada entrada da matriz X.
			 * @param X Matriz de inteiros, cada linha é uma entrada a ser classificada pela rede.
			 * @return Unordered_map contendo a porcentagem de memórias ativadas para cada label de cada entrada da matriz X.
			 */
			std::vector<std::unordered_map<std::string, float>> predictProba(const std::vector<std::vector<int> > &X);

		private:
			/** Comprimento da retina.*/
			int retinaLength;
			/** Número de bits a ser utilizado para endereçamento.*/
			int numBitsAddr;
			/** Flag para sinalizar se deve-se ou não utilizar bleaching.*/
			bool useBleaching;
			/** Valor limite para a confiança, que é a diferença percentual entre os dois melhores resultados da rede.*/
			float confidenceThreshold;
			/** Valor padrão para o bleaching.*/
			int defaultBleaching_b;
			/** Flag para sinalizar se o mapeamento da entrada deve ser randômico.*/
			bool randomizePositions;
			/** Flag para sinalizar se o conteúdo das memórias associadas à rede é cumulativo.*/
			bool isCummulative;
			/** Flag para sinalizar se o primeiro endereço das memórias deve ser omitido na análise.*/
			bool ignoreZeroAddr;
			/** Semente aleatória utilizada para se embaralhar o endereçamento da entrada.*/
			unsigned seed;
			/** Mapa de objetos Discriminator associados ao objeto WiSARD;*/
			std::unordered_map <std::string, Discriminator*> discriminators;
			/** Vetor auxiliar, utilizado para auxiliar o endereçamento da entrada.*/
			std::vector<int> memoryAddressMapping;

			/**
			 * @brief Responsável pela implmentação da técnica de bleaching.
			 * @param result Unordered_map contendo a porcentagem de memórias ativadas para cada label de uma dada entrada.
			 * @param memoryResult Vetor com o conteúdo de cada memória, endereçado por uma porção de bits de uma dada entrada.
			 * @return Unordered_map contendo a porcentagem de memórias ativadas para cada label de uma dada entrada com bleaching aplicado.
			 */
			std::unordered_map <std::string,float> applyBleaching(std::unordered_map<std::string,float> &result,
				std::unordered_map<std::string, std::vector<int>> &memoryResult);


	};
}

#endif /* WISARD_HPP_ */
