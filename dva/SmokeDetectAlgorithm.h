#ifndef SmokeDetectAlgorithm_h_
#define SmokeDetectAlgorithm_h_

#include "Algorithm.h"

/**
 * Абстрактный класс алгоритма детектирования дыма.
 */
class SmokeDetectAlgorithm 
	: public Algorithm 
{
protected:
	
	/**
	 * Размер анализируемых блоков изображения.
	 */
	cv::Size blockSize_;
	
public:
	
	/**
	 * Определение типа "умного" указателя на алгоритм детектирования дыма.
	 */
	typedef boost::shared_ptr<SmokeDetectAlgorithm> SharedPtr;
	
	/**
	 * Возвращает результат детектирования огня.
	 * 
	 * @param img входные параметры детектора(довольно часто это может быть текущий кадр).
	 * @param blockSize размер блоков, на которые делиться изображение для анализа изображения.
	 * @return результат работы детектора.
	 */
	using Algorithm::detect;
	virtual cv::Mat detect(const cv::Mat& img, const cv::Size &blockSize) = 0;
	
	/**
	 * Устанавливает размер блоков, на которые делиться изображение для анализа изображения.
	 * 
	 * @param blockSize размер блоков, на которые делиться изображение для анализа изображения.
	 */
	virtual void setBlockSize(const cv::Size &blockSize) {
		blockSize_ = blockSize;
	}
	
};

#endif // FireDetectAlgorithm_h_