#ifndef ImageFilter_h_
#define ImageFilter_h_

#include "opencv/cv.h"
#include <string>
#include <networking/ExchangeTypes.hpp>
#include "AnCommon.h"

/**
 * Абстрактный класс фильтра изображения.
 */
class ImageFilter {

public:

	/**
	 * Выполняет фильтрацию данных.
	 * 
	 * @param img входное изображение.
	 * @return результат работы фильтра. 
	 */
	virtual cv::Mat operator()(cv::Mat& img) = 0;
	
	/**
	 * Устанавливает настройки(параметры) алгоритма.
	 * 
	 * @param settings параметры(настройки) алгоритма.
	 * @param usedSettings множество используемых настроек.
	 */
	virtual void setSettings(const xml::Request::Params &settings, AnCommon::StrSet &usedSettings) = 0;
	
	/**
	 * Возвращает настройки(параметры) алгоритма.
	 * 
	 * @param settings параметры(настройки) алгоритма.
	 */
	virtual void getSettings(xml::Request::Params &settings) = 0;

	/**
	 * Возвращает тип фильтра.
	 * 
	 * @return тип детектора.
	 */
	virtual std::string getType() = 0;

};

#endif // ImageFilter_h_