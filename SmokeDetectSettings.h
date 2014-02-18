#ifndef SmokeDetectSettings_h_
#define SmokeDetectSettings_h_

/**
 * Класс настроек для алгоритма отделения объектов от фона. FM: неверный комментарий
 */
class SmokeDetectSettings {

private:

	/**
	 * Количество блоков, на которые разбивается изображение по горизонтали и по вертикали.
	 */
	static const int BLOCKS_PER_DIMENSION = 2;

public:
	
	/**
	 * Значения поумолчанию соответствующих полей.
	 * 
	 * @{
	 */
	static const int ALERT_TIME; 
	static const int NUM_WIDTH_BLOCKS; 
	static const int NUM_HEIGHT_BLOCKS; 
	static const double MIN_SMOKE_AREA; 
	 /**
	 * @}
	 */
	
	/**
	 * Интервал времени, с которым сообщается о результате работы детектора.
	 */
	int alertTime_;
	
	/**
	 * Количество блоков по ширине кадра.
	 */ 
	int numWidthBlocks_;
	
	/**
	 * Количество блоков по высоте кадра.
	 */ 
	int numHeightBlocks_;
	
	/**
	 * Минимальная сумарная площадь дыма, процент от площади кадра.
	 */ 
	double minSmokeArea_;

	/**
	 * Конструктор по умолчанию.
	 */
	SmokeDetectSettings();

	/**
	 * Создаёт объект с заданными параметрами.
	 * 
	 * @param alertTime интервал времени, с которым сообщается о результате работы детектора.
	 * @param numWidthBlocks количество блоков по ширине кадра.
	 * @param numHeightBlocks количество блоков по высоте кадра.
	 * @param minSmokeArea минимальная сумарная площадь дыма, процент от площади кадра.
	 * 
	 */
	SmokeDetectSettings(int alertTime, 
						int numWidthBlocks, 
						int numHeightBlocks, 
						double minSmokeArea);
	
	/**
	 * Возвращает количество блоков по ширине.
	 * 
	 * @return количество блоков по ширине.
	 */
	int getBlockSizeX(int sizeX);

	/**
	 * Возвращает количество блоков по высоте.
	 * 
	 * @return количество блоков по высоте.
	 */
	int getBlockSizeY(int sizeY);

};

#endif // SmokeDetectSettings_h_
