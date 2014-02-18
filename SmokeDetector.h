#ifndef SmokeDetector_h_
#define SmokeDetector_h_

#include "Detector.h"
#include "SmokeDetectOnContrastAlgorithm.h"
#include "SmokeDetectSettings.h"

/**
 * Класс детектор огня.
 */
class SmokeDetector 
	: public Detector 
{
public:
	
	/**
	 * Тип детектора.
	 */
	static const std::string SMOKE_DETECTOR;

	/**
	 * Включает детектор.
	 * 
	 * @throw std::string описание ошибки.
	 */
	virtual void on();

	/**
	 * Выключает детектор.
	 */
	virtual void off();
	
	/**
	 * Основная функция детектора, анализирует текущий кадр.
	 * 
	 * @param image текущий кадр.
	 * @param imageTime время отправления кадра.
	 * @param resultingXml результат работы детектора.
	 * @throw std::string описание ошибки.
	 */
	virtual void execute(const cv::Mat& image, const boost::posix_time::ptime& imageTime, std::string& resultingXml);
	
	/**
	 * Возвращает тип детектора.
	 * 
	 * @return тип детектора.
	 */
	virtual std::string getType();    
	
	/**
	 * Устанавливает настройки детектора.
	 *
	 * @param params параметры команды(запроса). 
	 * @param error описание произошедшей ошибки.
	 * @throw std::string описание ошибки.
	 */
	virtual void setSettings(xml::Request::Params params);
	
	/**
	 * Возвращает настройки детектора.
	 * 
	 * @return настройки детектора ввиде структуры @a xml::Request::Params. 
	 */
	virtual xml::Request::Params getSettings();
	
	/**
	 * Устанавливает значения по умолчанию у всех переменных экземпляра класса.
	 */
	void clear();
	
private:
	
	/**
	 * Настройки(параметры) детектора.
	 */
	SmokeDetectSettings settings_;
	
	/**
	 * Алгоритм детектирования дыма по контрастности.
	 */
	SmokeDetectOnContrastAlgorithm smokeDetectOnContrastAlg_;

	/**
	 * Производит все необходимые действия для детектирования дыма.
	 * 
	 * @return результат детектирования в определённом формате(формат описан в соответствующей статье wiki).
	 */
	std::string detectSmoke(const cv::Mat& image);
	
};

#endif // SmokeDetector_h_
