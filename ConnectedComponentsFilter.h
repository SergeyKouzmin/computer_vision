#ifndef ConnectedComponentsFilter_h_
#define ConnectedComponentsFilter_h_

#include "ImageFilter.h"
#include "AnCommon.h"
#include "MorphologyFilter.h"

/**
 * Фильтр реализующий алгоритм "Связных компонент"(объедиение близко находящихся часетей в объект и аппроксимирует полученный результат). 
 */
class ConnectedComponentsFilter 
	: public ImageFilter 
{
public:
	
	/**
	 * Методы аппроксимации контуров.
	 */
	enum ContourRefinementMethod {
		
		/**
		 * Аппроксимация многоугольником.
		 */
		RM_APPROX,
		
		/**
		 * Выпуклая оболочка.
		 */
		RM_CONVEX_HULL
	};
	
	/**
	 * Класс настроек(параметров фильтра).
	 */
	class ConnectedComponentsFilterSettings {
	
	public:
		
		/**
		 * Значения по умолчанию соответствующих полей.
		 * 
		 * @{
		 */
		static const int COUNTOUR_APPROX_LEVEL;
		static const ContourRefinementMethod METHOD;
		static const float PERIM_SCALE;
		/**
		 * @}
		 */
		
		/**
		 * Уровень аппроксимации, при постобработке результатов алгоритма.
		 * Чем больше значение, тем аппроксимация более грубая.
		 * Этот параметр передаётся функции @a cv::contourApproxLevel, причём может быть вещественным,
		 * в нашем случае достаточно целочисленных значений >= 1.
		 */
		int contourApproxLevel_;
		
		/**
		 * Метод аппроксимации.
		 */
		ContourRefinementMethod method_;
		
		/**
		 * Коэффициент для определения нижнего порога периметра контура.
		 */
		float perimScale_;
		
		/**
		 * Создаёт объект класса с параметрами по умолчанию.
		 */
		ConnectedComponentsFilterSettings();
		
		/**
		 * Создаёт объект класса с заданными параметрами.
		 * 
		 * @param описание параметров смотри у описания соответствующих полей класса.
		 */
		ConnectedComponentsFilterSettings(int contourApproxLevel, ContourRefinementMethod method, float perimScale);
		
	};
	
	/**
	 * Тип фильтра.
	 */
	static const std::string CONNECTED_COMPONENTS_FILTER_TYPE; 

	/**
	 * Созадёт фильтр с настройками(параметрами) по умолчанию.
	 */
	ConnectedComponentsFilter();

	/**
	 * Созадёт фильтр с заданными настройками(параметрами).
	 * 
	 * @param settings настройки фильтра связных компонент.
	 * @param morphSettings настройки фильтра морфологического преобразования.
	 */
	ConnectedComponentsFilter(const ConnectedComponentsFilterSettings& settings, MorphologyFilter::MorphologyFilterSettings morphSettings = MorphologyFilter::MorphologyFilterSettings());
	
	/**
	 * Выполняет выделение "связных компонет" на заданном изображении @a img.
	 * 
	 * @param img входное изображение.
	 * @return результат работы фильтра. 
	 */
	cv::Mat operator()(cv::Mat& img);
	
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
	 * Возвращает тип фильтра.
	 * 
	 * @return тип детектора.
	 */
	virtual std::string getType();
	
private: 
	
	/**
	 * Фильтр производящий морфологическую обработку.
	 */
	MorphologyFilter morphology_;
	
	/**
	 * Настройки(параметры) фильтра.
	 */
	ConnectedComponentsFilterSettings settings_;
	
	/**
	 * Контуры для поиска связных компонент.
	 */
	AnCommon::Contours maskContours_;
};

#endif // ConnectedComponentsFilter_h_