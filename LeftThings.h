#ifndef LeftThings_h_
#define LeftThings_h_

#include <boost/date_time.hpp>

/**
 * Класс для хранения параметров и характеристик оставленной вещи.
 */
class LeftThings {

public:

	/**
	 * Время которое вещь находится на своём месте неподвижно.
	 */
	boost::posix_time::ptime restTime_;

	/**
	 * Флаг показывает что объект точно является оставленной вещью.
	 */
	bool isLeftThings_;

	/**
	 * Создаёт объект класса с настройками по умолчанию.
	 */
	LeftThings();

	/**
	 * Устанавливает время нахождения вещи на месте.
	 *
	 * @param restTime время нахождения вещи на месте.
	 */
	void setRestTime(const boost::posix_time::ptime &restTime);

};

#endif // LeftThings_h_
