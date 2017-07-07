/**
 * @file   Util.hpp
 * @Author fabricio
 * @date   Abril 2, 2016
 * @brief  Arquivo de declaração das funções do espaço "util".
 */

#ifndef UTIL_CPP_
#define UTIL_CPP_

#include <string>
#include <unordered_map>


namespace wann
{
	namespace util 
	{
		/**
		 * @brief Calcula a confiança do resultado do processamento de uma WiSARD.
		 * @param result Unordered_map contendo a porcentagem de memórias ativadas para cada label de uma dada entrada.
		 * @return Valor percentual de confiança.
		 */
		float calculateConfidence(std::unordered_map<std::string, float>&result);

		/**
		 * @brief Obtém o maior valor de porcentagem de acertos de uma dada entrada.
		 * @param values Unordered_map contendo a porcentagem de memórias ativadas para cada label de uma dada entrada.
		 * @return Máximo valor de porcentagem de memórias ativadas.
		 */
		float maxValue(std::unordered_map<std::string, float>&values);

		/**
		 * @brief Seleciona a label com maior porcentagem de acertos.
		 * @param values Unordered_map contendo a porcentagem de memórias ativadas para cada label de uma dada entrada.
		 * @return Label que obteve mais memórias ativadas.
		 */
		std::string argMax(std::unordered_map<std::string, float>&values);
	}
}

#endif /* UTIL_CPP_ */
