#ifndef SmokeDetectOnContrastAlgorithm_h_
#define SmokeDetectOnContrastAlgorithm_h_

#include "SmokeDetectAlgorithm.h"

/**
 * Класс алгоритма детектирования огня по динамике.
 */
class SmokeDetectOnContrastAlgorithm 
	: public SmokeDetectAlgorithm
{
public:
	
	/**
	 * Тип алгоритма.
	 */
	static const std::string SMOKE_DETECT_ON_CONTRAST;
	
	/**
	 * Класс настроек алгоритма детектирования огня по динамике.
	 */
	class SmokeDetectOnContrastAlgorithmSettings {
	
	public:
		
		/**
		 * Значения по умолчанию соответствующих полей.
		 *  
		 * @{
		 */
		static const float THRESHOLD; 
		static const float EMA_ALPHA;
		static const int EMA_DELAY; 
		 /**
		  * @}
		  */

		/**
		 * Порог изменения контрастности, в процентах от взвешенного среднего, возможные значения от 0.2 до 0.95
		 */
		float threshold_;

		/**
		 * Значения альфа для вычисления экспоненциального скользящего среднего.
		 */
		float emaAlpha_;

		/**
		 * За сколько кадров назад берется значение скользящей средней для сравнения с порогом.
		 */
		int emaDelay_;
	
		
		/**
		 * Создаёт объект класса с значениями по умолчанию.
		 */
		SmokeDetectOnContrastAlgorithmSettings();
		
		/**
		 * Создаёт объект класса с заданными значениями.
		 * 
		 * @param описание параметров см. в описании соотвествующих полей класса.
		 */
		SmokeDetectOnContrastAlgorithmSettings(float threshold,
											   float emaAlpha,
											   int emaDelay);
		
	};
	
	/**
	 * История для одного блока.
	 */
	struct SmokeDetectContrastData {

		/**
		 * Список средних контрастностей для последних кадров.
		 */
		std::list <float> emaBuf;

		/**
		 * Флаг, показывающий был ли блок раньше дымом.
		 */
		bool isSmoke;

		/**
		 * Создаёт пустую историю блока.
		 */
		SmokeDetectContrastData()
			: isSmoke(false)
		{}
	};

	/**
	 * Создаёт объект с параметрами по умолчанию. 
	 */
	SmokeDetectOnContrastAlgorithm() {};
	
	/**
	 * Создаёт алгоритм с заданными параметрами(настройками).
	 * 
	 * @param settings параметры(настройки) алгоритма.
	 */
	SmokeDetectOnContrastAlgorithm(const SmokeDetectOnContrastAlgorithmSettings &settings);
	
	/**
	 * Возвращает результат детектирования дыма.
	 * 
	 * @param img результат выделения дыма по контрасности.
	 * @return результат работы детектора.
	 */
	virtual cv::Mat detect(const cv::Mat& img);
	
	/**
	 * Возвращает результат детектирования дыма.
	 * 
	 * @param img входные параметры детектора(довольно часто это может быть текущий кадр).
	 * @param blockSize размер блоков, на которые делиться изображение для анализа изображения.
	 * @return результат работы детектора.
	 */
	virtual cv::Mat detect(const cv::Mat& img, const cv::Size &blockSize);
	
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
	void setSettings(const SmokeDetectOnContrastAlgorithmSettings &settings);
	
	/**
	 * Возвращает настройки(параметры) алгоритма.
	 * 
	 * @return параметры(настройки) алгоритма.
	 */
	SmokeDetectOnContrastAlgorithmSettings getSettings();
	
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
	 * Настройки алгоритма.
	 */
	SmokeDetectOnContrastAlgorithmSettings settings_;

	/**
	 * Статистика контрастности по каждому блоку.
	 */
	std::vector <SmokeDetectContrastData> vecSmokeContrastData_;

	/**
	 *  Представление текущего кадра в формате HSV.
	 */
	cv::Mat tempHSVImage;

	/**
	 * Яркость текущего кадра.
	 */
	cv::Mat tempVImage;

	/**
	 * Результат морфологического преобразования.
	 */
	cv::Mat tempMorphologyResult;
	
};

#endif // SmokeDetectOnContrastAlgorithm_h_