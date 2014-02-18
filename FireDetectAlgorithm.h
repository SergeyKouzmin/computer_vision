#ifndef FireDetectAlgorithm_h_
#define FireDetectAlgorithm_h_

#include "Algorithm.h"

/**
 * Абстрактный класс алгоритма детектирования огня.
 */
class FireDetectAlgorithm 
	: public Algorithm 
{
public:
	
	/**
	 * Определение типа "умного" указателя на алгоритм детектирования огня.
	 */
	typedef boost::shared_ptr<FireDetectAlgorithm> SharedPtr;
	
	/**
	 * Возвращает результат детектирования огня.
	 * 
	 * @param img входные параметры детектора(довольно часто это может быть текущий кадр).
	 * @return результат работы детектора.
	 */
	virtual cv::Mat detect(const cv::Mat& img) = 0;
	
};

#endif // FireDetectAlgorithm_h_