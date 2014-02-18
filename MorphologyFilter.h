#ifndef MorphologyFilter_h_
#define MorphologyFilter_h_

#include "ImageFilter.h"

/**
 * Фильтр морфологического преобразования с заданным уровнем. 
 */
class MorphologyFilter 
	: public ImageFilter 
{
public:
	
	/**
	 * Класс настроек(параметров фильтра).
	 */
	class MorphologyFilterSettings {
	
	public:

		/**
		 * Значения по умолчанию соответствующих полей.
		 * 
		 * @{
		 */
		static const int CLOSE_ITR;
		/**
		 * @}
		 */

		/**
		 * Уровень морфологической обработки при постобработке результатов алгоритма.
		 * Чем больше значение, тем грубее обработка (больше эрозия), значения: 1 - 10.
		 */
		int closeItr_;

		/**
		 * Создаёт объект класса с параметрами по умолчанию.
		 */
		MorphologyFilterSettings();
		
		/**
		 * Создаёт объект класса с заданными параметрами.
		 * 
		 * @param описание параметров смотри у описания соответствующих полей класса.
		 */
		MorphologyFilterSettings(int closeItr);
		
	};
	
	/**
	 * Тип фильтра.
	 */
	static const std::string MORPHOLOGY_FILTER_TYPE; 

	/**
	 * Создаёт объект класса с настройками(параметрами) по умолчанию.
	 */
	MorphologyFilter();
	
	/**
	 * Создаёт объект класса с заданными настройками(параметрами)
	 * 
	 * @param settings настройки(параметры) фильтра.
	 */
	MorphologyFilter(const MorphologyFilterSettings &settings);
	
	/**
	 * Выполняет морфологическое преобразование на заданном изображении @a img.
	 * 
	 * @param img входное изображение.
	 * @return результат работы фильтра. 
	 */
	virtual cv::Mat operator()(cv::Mat& img);
	
	/**
	 * Возвращает тип фильтра.
	 * 
	 * @return тип детектора.
	 */
	virtual std::string getType();
	
	/**
	 * Устанавливает настройки(параметры фильтра).
	 * 
	 * @param settings настройки(параметры) фильтра.
	 */
	void setSettings(const MorphologyFilterSettings &settings);
	
	/**
	 * Возвращает настройки(параметры) фильтра.
	 */
	MorphologyFilterSettings getSettings();
	
	/**
	 * Устанавливает настройки(параметры) алгоритма.
	 * 
	 * @param settings параметры(настройки) алгоритма.
	 * @param usedSettings множество используемых настроек.
	 */
	void setSettings(const xml::Request::Params &settings, AnCommon::StrSet &usedSettings);
	
	/**
	 * Возвращает настройки(параметры) алгоритма.
	 * 
	 * @param settings параметры(настройки) алгоритма.
	 */
	void getSettings(xml::Request::Params &settings);
	
private:
	
	/**
	 * Настройки(параметры) фильтра.
	 */
	MorphologyFilterSettings settings_;
};

#endif // MorphologyFilter_h_