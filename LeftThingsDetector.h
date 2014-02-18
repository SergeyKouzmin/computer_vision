#ifndef LeftThingsDetector_h_
#define LeftThingsDetector_h_

#include "Detector.h"
#include "LeftThingsDetectorSettings.h"
#include "Block.h"
#include "LeftThings.h"
#include "MorphologyFilter.h"
#include "ConnectedComponentsFilter.h"
#include "BackgroundSeparationAlgorithm.h"

/**
 * Класс реализующий детектор оставленных вещей.
 */
class LeftThingsDetector 
	: public Detector 
{
public:
	/**
	 * Тип детектора.
	 */
	static const std::string LEFT_THINGS_DETECTOR;
	
	/**
	 * Создаёт объект класса, инициализирует значениями по умолчанию.
	 */
	LeftThingsDetector();

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
	 * Основная функция детектора, анализарует текущий кадр.
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
	 * Текущее состояние детектора.
	 */
	AnCommon::Mode mode_;
	
	/**
	 * Предыдущее состояние детектора.
	 */
	AnCommon::Mode prevMode_;
	
	/**
	 * Алгоритм отделения объектов то фона.
	 */
	BackgroundSeparationAlgorithm::SharedPtr backgroundSeparationAlgorithm_;
	
	/**
	 * Размер блоков, на которые делится изображение (одинаковый для всех блоков).
	 */
	CvSize blockSize_;

	/**
	 * Хранит информацию о блоках, на которые делится изображение.
	 */
	std::vector<Block> objectsBlocks_;

	/**
	 * Время включения детектора.
	 */
	boost::posix_time::ptime activationTime_;

	/**
	 * Детектирует блоки с объектами.
	 *
	 * @param img изобржение, которое нужно разделить наблоки.
	 * @param objects_blocks структура, для хранения иформации о блоках.
	 */
	void detectBlocksWithObject(cv::Mat& img, std::vector<Block>& objects_blocks);

	/**
	 * Определяет находится ли в блоке объект.
	 *
	 * @param img изображние на котором находится блок.
	 * @param x абсцисса левого верхнего угла блока.
	 * @param y ордината левого верхнего угла блока.
	 * @param block_width ширина блока.
	 * @param block_height высота блока.
	 * @return true - в блоке есть объект, false - в блоке объекта нет.
	 */
	bool isObjectBlock(cv::Mat& img, int x, int y, int blockWidth, int blockHeight);

	/**
	 * Детектирует те блоки, в которых объект присутствует больше
	 * заданного времени.
	 *
	 * @param objects_blocks структура, для хранения иформации о блоках.
	 * @param date_time время текущего кадра.
	 */
	void detectStandingBlocks(std::vector<Block>& objectsBlocks,
							  const boost::posix_time::ptime& date_time);

	/**
	 * Генерирует матрицу для поиска контуров объектов.
	 * 
	 * @param objects_blocks структура, хранящая иформацию о блоках.
	 * @return матрицу принадлежности блоков объектам.
	 */ 
	cv::Mat generateResultMatrix(const std::vector<Block>& objectsBlocks);

	/**
	 * Детектирует неподжвижные объекты.
	 *
	 * @param objects контейнер с объектами.
	 * @param date_time время текущего кадра.
	 */
	void detectStandingObjects(std::vector<LeftThings> &objects
							  , const boost::posix_time::ptime& dateTime);

	/**
	 * Детектирует объекты, которых ещё нет в основном контейнере.
	 *
	 * @param objects контейнер с объектами.
	 * @param new_objects контейнеор объектов текущего кадра.
	 */
	void detectNewObjects(std::vector<LeftThings> &objects
						, const std::vector<LeftThings> &newObjects);

	/**
	 * Производит генерацию результата.
	 *
	 * @param mask матрица принадлежности блоков объектам.
	 * @return строка с результатом.
	 */
	std::string generateResult(const cv::Mat &mask);
	
	/**
	 * Находит стоящие неподвижно втечении некоторого времени предметы (время задаёться в настройках).
	 *
	 * @param image текущий кадр.
	 * @param frame_update_time время отправления кадра с камеры.
	 */
	std::string findStandingObjects(const cv::Mat& image, const boost::posix_time::ptime& frameUpdateTime);

	/**
	 * Находит стоящие неподвижно втечении некоторого времени предметы (время задаёться в настройках).
	 *
	 * @param frame_update_time время отправления кадра с камеры.
	 * @return результат работы детектора.
	 */
	std::string findBlockWithStandingObject(const boost::posix_time::ptime& frameUpdateTime);

	/**
	 * Настройки детектора забытых вещей.
	 */
	LeftThingsDetectorSettings settings_;

	/**
	 * Текущий кадр.
	 */
	cv::Mat curFrame_;

	/**
	 * Разность фона и кадра.
	 */
	cv::Mat subFoneFrame_;
	
	/**
	 * Фильтр морфологического преобразования.
	 */
	MorphologyFilter morphologyFilter_;
	
	/**
	 * Фильтр выделения связных компонент.
	 */
	ConnectedComponentsFilter connectedComponentsFilter_;
	
	/**
	 * Устанавливает время старта детектора равным текущему системному времени.
	 */
	void setActivationTime();
};

#endif // LeftThingsDetector_h_
