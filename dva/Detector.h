#ifndef Detector_h_
#define Detector_h_

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/date_time.hpp>
#include <opencv/cxcore.h>
#include <opencv/cxcore.hpp>
#include <networking/ExchangeTypes.hpp>

/**
 * Базовый класс для детекторов видеоаналитики.
 */
class Detector
{
protected:

	/**
	 * Состояние детектора, true - детектор включён, false - детектор выключен. 
	 */
	bool on_;
	
public:
	/**
	 * Определение типа "умного" указателя на детектор.
	 */
	typedef boost::shared_ptr<Detector> SharedPtr;

	/**
	 * Инициализирует детектор по умолчанию.
	 */
	Detector();

	/**
	 * Освобождает ресурсы детектора.
	 */
	virtual ~Detector() {}

	/**
	 * Основная функция детектора, анализирует текущий кадр.
	 * 
	 * @param image текущий кадр.
	 * @param imageTime время отправления кадра.
	 * @param resultingXml результат работы детектора.
	 * @throw std::string описание ошибки.
	 */
	virtual void execute(const cv::Mat& image, const boost::posix_time::ptime& imageTime, std::string& resultingXml) = 0;

	/**
	 * Возвращает тип детектора.
	 * 
	 * @return возвращает константу перечислимого типа соотвествующую данному детектору.
	 */
	virtual std::string getType() = 0;

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
	 * Возвращает состояние детектора.
	 * 
	 * @return true - детектор включён, false - детектор выключен.
	 */
	bool state();

	/**
	 * Устанавливает настройки детектора.
	 *
	 * @param params параметры команды(запроса). 
	 * @param error описание произошедшей ошибки.
	 */
	virtual void setSettings(xml::Request::Params params) = 0;

	/**
	 * Возвращает настройки детектора.
	 * 
	 * @return настройки детектора в виде структуры @a xml::Request::Params. 
	 */
	virtual xml::Request::Params getSettings();
};

#endif // Detector_h_
