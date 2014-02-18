#ifndef Object_h_
#define Object_h_

#include <opencv/cv.h>
#include <opencv/cv.hpp>

namespace AnCommon {

/**
 * Класс для хранения данных о объекте.
 */
class Object {
	
private:
	
	/**
	 * Идентификационный номер объекта на кадре.
	 */
	int id;
	
	/**
	 * Обрамляющий прямоугольник.
	 */
	cv::Rect rect;

	/**
	 * Запрещено создавать объект по умолчанию.
	 */
	Object();
	
public:
	
	/**
	 * Создаёт объект по id и обрамляющему прямоугольнику.
	 */ 
	Object(int id_, cv::Rect boundingRect)
		: id(id_)
		, rect(boundingRect) {};
	
	/**
	 * Возвращает id объекта. 
	 * 
	 * @return id объекта.
	 */
	int getId() const {
		return id;	
	}
	
	/**
	 * Возвращает обрамляющий прямоугольник объекта. 
	 *
	 * @return обрамляющий прямоугольник.
	 */
	cv::Rect getRect() const {
		return rect;
	}
	
	/**
	 * Устанавливает id объекта.
	 * 
	 * @param objectId новый id объекта.
	 */
	void setId(int objectId) {
		id = objectId;
	}
	
	/**
	 * Устанавливает новый обрамляющий прямоугольник объекта.
	 * 
	 * @param boundingRect новый обрамляющий прямоугольник объекта.
	 */
	void setRect(cv::Rect boundingRect) {
		rect = boundingRect;	
	}
	
};

}

#endif // Object_h_