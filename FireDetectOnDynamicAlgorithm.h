#ifndef FireDetectOnDynamicAlgorithm_h_
#define FireDetectOnDynamicAlgorithm_h_

#include "FireDetectAlgorithm.h"

/**
 * Класс алгоритма детектирования огня по динамике.
 */
class FireDetectOnDynamicAlgorithm 
: public FireDetectAlgorithm
{
public:
	
	/**
	 * Тип алгоритма.
	 */
	static const std::string FIRE_DETECT_ON_DYNAMIC;
	
	/**
	 * Класс настроек алгоритма детектирования огня по динамике.
	 */
	class FireDetectOnDynamicAlgorithmSettings {
	
	public: 
		
		/**
		 * Значения по умолчанию соответствующих полей.
		 * 
		 * @{
		 */	
		static const double MIN_FIRE_DELTA;
		static const int FIRED_PIXELS_THRESHOLD_PERCENT;
		static const double SLIDING_AVG_ALPHA;
		static const double MIN_FOREGROUND_PIXELS_PERCENT;
		static const int FOREGROUND_TO_FIRE_MAX_RATIO;
		/**
		 * @}
		 */
	
		/**
		 * Минимальное колебание интенсивностей "нефоновых" пикселей, позволяющее считать их огнём.
		 */
		double minFireDelta_;
			
		/**
		 * Начиная с какого количества огненных пикселей считается, что в блоке огонь. Измеряется в процентах, от 1 до 100.
		 */
		int firedPixelsThresholdPercent_;
	
		/**
		 * Весовой коэффициент для вычисления скользящих средних.
		 */
		double slidingAvgAlpha_;
		
		/**
		 * Минимальный процент "нефоновых" пикселей в кадре, принимаемый алгоритмом во внимание.
		 */
		double minForegroundPixelsPercent_;
		
		/**
		 * Отношение количества "нефоновых" пикселей к количеству огненных, при превышении которого считается, что огня в кадре нет.
		 */
		int foregroundToFireMaxRatio_;
		
		/**
		 * Создаёт объект класса с значениями по умолчанию.
		 */
		FireDetectOnDynamicAlgorithmSettings();
		
		/**
		 * Создаёт объект класса с заданными значениями.
		 * 
		 * @param описание параметров см. в описании соотвествующих полей класса.
		 */
		FireDetectOnDynamicAlgorithmSettings(
						double minFireDelta,
						int firedPixelsThresholdPercent,
						double slidingAvgAlpha,
						double minForegroundPixelsPercent,
						int foregroundToFireMaxRatio);
		
	};
	
	/**
	 * Создаёт объект с параметрами по умолчанию. 
	 */
	FireDetectOnDynamicAlgorithm();
	
	/**
	 * Создаёт алгоритм с заданными параметрами(настройками).
	 * 
	 * @param settings параметры(настройки) алгоритма.
	 */
	FireDetectOnDynamicAlgorithm(const FireDetectOnDynamicAlgorithmSettings& settings);
	
	/**
	 * Возвращает результат детектирования огня.
	 * 
	 * @param img результат выделения "огненный" по цветовой маске.
	 * @return результат работы детектора.
	 */
	virtual cv::Mat detect(const cv::Mat& img);
	
	/**
	 * Устанавливает настройки(параметры) алгоритма.
	 * 
	 * @param settings параметры(настройки) алгоритма.
	 * @param usedSettings множество используемых настроек.
	 */
	virtual void setSettings(const xml::Request::Params &settings, AnCommon::StrSet &usedSettings);
	
	/**
	 * Возвращает настройки(параметры) алгоритма.
	 * 
	 * @param settings параметры(настройки) алгоритма.
	 */
	virtual void getSettings(xml::Request::Params &settings);
	
	/**
	 * Устанавливает настройки(параметры) алгоритма.
	 * 
	 * @param settings параметры(настройки) алгоритма.
	 */
	virtual void setSettings(const FireDetectOnDynamicAlgorithmSettings &settings);
	
	/**
	 * Возвращает настройки(параметры) алгоритма.
	 * 
	 * @return параметры(настройки) алгоритма.
	 */
	FireDetectOnDynamicAlgorithmSettings getSettings();
	
	/**
	 * Возвращает тип алгоритма.
	 * 
	 * @return тип алгоритма.
	 */
	virtual std::string getType();

	/**
	 * Очищает все поля, связанные с работой алгоритма.
	 */
	virtual void clear();
	
private:
	
	/**
	 * Проверяет находить ли цвет пикселя в диапазоне огненя.
	 * 
	 * @param x абсцисса пикселя.
	 * @param y ордината пикселя.
	 */
	bool pixelIsForeground(int x, int y);
	
	/**
	 * Заполняет маску переднеплановых пикселей(пикселей огненого цвета).
	 */
	void fillForegroundMask();
	
	/**
	 * Настройки алгоритма.
	 */
	FireDetectOnDynamicAlgorithmSettings settings_;
	
	/**
	 * Количество каналов во всех используемых форматах изображений.
	 */
	static const size_t CHANNELS = 3;
	
	/**
	 * Копия заголовка текущего обрабатываемого кадра.
	 */
	cv::Mat currentFrameBGR_;
	
	/**
	 * Текущий кадр в формате HSV.
	 */
	cv::Mat currentFrameHSV_;
	
	/**
	 * Текущий кадр в формате YCrCb.
	 */
	cv::Mat currentFrameYCrCb_;
	
	/**
	 * Маска для различения "фоновых" и "нефоновых" пикселей, устанавливается извне.
	 */
	cv::Mat foregroundMask_;
	
	/**
	 * Скользящие средние изменения интенсивностей для каждого пикселя.
	 */
	cv::Mat slidingAvg_;

	/**
	 * Осреднённые скользящие средние изменения интенсивностей для всех "фоновых" пикселей.
	 */
	std::vector<double> totalBgAvg_;
	
	/**
 	 * Количество "нефоновых" пикселей.
	 */
	int foregroundPixelCount_;
	
	/**
	 * Количество огненных пикселей.
	 */
	int firedPixelCount_;
	
	/**
	 * Попиксельный результат детектирования огня.
	 */
	cv::Mat perPixelResultMask_;

	/**
	 * Предыдущий кадр.
	 */
	cv::Mat prevImg_;
	
	/**
	 * Заполнить @a perPixelResultMask_ по динамике изменений интенсивностей.
	 */
	void fillPerPixelResultMask();
	
	/**
	 * Пересчёт скользящих средних изменения интенсивностей для одного пикселя.
	 * 
	 * @param x абсцисса пикселя. 
	 * @param y ордината пикселя.
	 */
	void updateSlidingAvg(int x, int y);
	
	/**
	 * Пересчёт всех скользящих средних.
	 */
	void updateAverages();
	
	/**
	 * Пересоздать матрицу и инициализировать нулями, если её размер отличается от размера текущего кадра.
	 * 
	 * @param m пересоздаваемая матрица.
	 * @param type тип элемента матрицы.
	 * @param initialValue значение элементов вновь созданной матрицы.
	 */
	void recreateMatrixIfNeeded(cv::Mat& m, int type, cv::Scalar initialValue);

};

#endif // FireDetectOnDynamicAlgorithm_h_