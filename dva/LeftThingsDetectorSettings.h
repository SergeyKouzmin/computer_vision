#ifndef LeftThingsDetectorSettings_h_
#define LeftThingsDetectorSettings_h_

/**
 * Класс настроек для класса AnCvLeftThingsDetector.
 */
class LeftThingsDetectorSettings {

public:
	
	/**
	 * Значения по умолчанию соответствующих полей.
	 * 
	 * @{
	 */	
	static const int ALERT_TIME;
	
	static const int MAX_SUPERVISION_TIME;
	
	static const int NUM_BLOCK_WIDTH;
	
	static const int NUM_BLOCK_HEIGHT;
	
	static const double MIN_OBJECT_AREA;
	
	static const double MAX_OBJECT_AREA;
	
	static const int STARTING_LEARNING_TIME;
	
	static const int MAX_MERGING_GAP_PERCENT;
	
	static const int STARTING_LEARNING_PERCENT;
	
	static const bool USE_MORPHOLOGY;
	
	static const bool USE_CONNECTED_COMP;
	/**
	 * @}
	 */
	
	/**
	 * Интервал, с которым сообщается УС о оставленных вещах.
	 */
	int alertTime_;

	/**
	 * Время после истечения которого вещь считается оставленной (в сек.).
	 */
	int maxSupervisionTime_;

	/**
	 * Количество блоков по горизонтали.
	 */
	int numBlockWidth_;

	/**
	 * Количество блоков по вертикали.
	 */
	int numBlockHeight_;

	/**
	 * Минимальная площадь оставленной вещи, процент от площади кадра.
	 */
	double minObjectArea_;

	/**
	 * Максимальная площадь оставленной вещи, процент от площади кадра.
	 */
	double maxObjectArea_;

	/**
	 * Время обучения динамического фона при старте детектора (сек).
	 */
	int startingLearningTime_;

	/**
	 * Наибольший зазор между прямоугольниками с ост. вещами, при котором они сливаются в один в ответе, в процентах от ширины изображения.
	 */
	int maxMergingGapPercent_;

	/**
	 * Процент готовности первоначального обучения детектора. 
	 */
	int startingLearningPercent_;
	
	/**
	 * Показвает нужно ли применять морфологический фильтр.
	 */
	bool useMorphology_;
	
	/**
	 * Показывает нужно ли применять выделение связных компонент.
	 */
	bool useConnectedComp_;

	/**
	 * Создаёт объект настроек детектора.
	 *
	 * @param alertTime интервал, с которым сообщается УС о оставленных вещах.
	 * @param maxSupervisionTime максимальное время слежения за объектом
	 *        (после истечения этого времени объект считаеться забытым) (в сек.).
	 * @param numBlockWidth количество блоков по ширине.
	 * @param numBlockHeight количество блоком по высоте.
	 * @param startingLearningTime время обучения динамического фона при старте детектора (сек).
	 * @param maxMergingGapPercent наибольший зазор между прямоугольниками с ост. вещами, при котором они сливаются в один в ответе, в процентах от ширины изображения.
	 * @param startingLearningPercent процент готовности первоначального обучения детектора.
	 * @param useMorphology показвает нужно ли применять морфологический фильтр.
	 * @param useConnectedComp показывает нужно ли применять выделение связных компонент.
	 */
	LeftThingsDetectorSettings(int alertTime
								, int maxSupervisionTime
								, double minObjectArea
								, double maxObjectArea
								, int numBlockWidth
								, int numBlockHeight
								, int startingLearningTime
								, int maxMergingGapPercent
								, int startingLearningPercent
								, bool useMorphology
								, bool useConnectedComp);

	/**
	 * Создаёт объект настроек детектора.
	 */
	LeftThingsDetectorSettings();

	/**
	 * Конструктор копирования, копирует данные из другово объекта.
	 *
	 * @param settings объект настроек детектора.
	 */
	LeftThingsDetectorSettings(const LeftThingsDetectorSettings &settings);
};

#endif // LeftThingsDetectorSettings_h_
