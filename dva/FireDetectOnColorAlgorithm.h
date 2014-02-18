#ifndef FireDetectOnColorAlgorithm_h_
#define FireDetectOnColorAlgorithm_h_

#include "FireDetectAlgorithm.h"
#include <string>

/**
 * Абстрактный класс алгоритма детектирования огня.
 */
class FireDetectOnColorAlgorithm 
	: public FireDetectAlgorithm 
{
public:
	
	/**
	 * Тип алгоритма.
	 */
	static const std::string FIRE_DETECT_ON_COLOR_ALGORITHM;
	
	/**
	 * Производит обучение алгоритма.
	 *
	 * @param img входные параметры детектора(текущий кадр).
	 */
	virtual void learn(const cv::Mat& img) {};
	
	/**
	 * Возвращает результат детектирования огня.
	 * 
	 * @param img входные параметры детектора(текущий кадр).
	 * @return результат работы детектора.
	 */
	virtual cv::Mat detect(const cv::Mat& img);
	
	/**
	 * Устанавливает настройки(параметры) алгоритма.
	 * 
	 * @param settings параметры(настройки) алгоритма.
	 * @param usedSettings множество используемых настроек.
	 */
	virtual void setSettings(const xml::Request::Params &settings, AnCommon::StrSet &usedSettings) {};
	
	/**
	 * Возвращает настройки(параметры) алгоритма.
	 * 
	 * @param settings параметры(настройки) алгоритма.
	 */
	virtual void getSettings(xml::Request::Params &settings) {};
	
	/**
	 * Возвращает тип алгоритма.
	 * 
	 * @return тип алгоритм.
	 */
	virtual std::string getType();
	
	/**
	 * Очищает все поля, связанные с работой алгоритма.
	 */
	virtual void clear() {};

private:
	
	/**
	 * Количество каналов во всех используемых форматах изображений.
	 */
	static const size_t CHANNELS = 3;
	
	/**
	 * Определяет имеет ли пиксель огненный цвет.
	 *
	 * @param x абсцисса пикселя.  
	 * @param y ордината пикселя.
	 * @param img текущий кадр.
	 * @return true - пиксель огненного цвета, false - пиксель не огненного цвета.
	 */
	bool pixelIsForeground(const cv::Mat &img, int x, int y); 
	
};

#endif // FireDetectOnColorAlgorithm_h_