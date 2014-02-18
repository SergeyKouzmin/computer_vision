#ifndef AnCommon_h_
#define AnCommon_h_

#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <opencv/cxcore.h>
#include <opencv/cxcore.hpp>

#include <opencv/highgui.h>
#include <opencv/highgui.hpp>

#include <map>
#include <boost/date_time.hpp>
#include <exception>
#include <list>
#include <set>
#include <time.h>
#include <logging/logging.hpp>
#include "Object.h"
#include "Errors.h"

/**
 * Представление белого цвета.
 */
#define CV_CVX_WHITE CV_RGB(0xff,0xff,0xff)

/**
 * Представление чёрного цвета.
 */
#define CV_CVX_BLACK CV_RGB(0x00,0x00,0x00)

/**
 * Пространство имён содержащее структуры данных и функции, которые используются в нескольких детекторах видеоаналитики.
 */
namespace AnCommon {
	
/**
 * Режимы работы детектора.
 */
enum Mode {
		
	/**
	 * Режим бездействия.
	 */
	IDLE = 0,
		
	/**
	 * Режим обучения.
	 */
	LEARNING,
		
	/**
	 * Режим классификации.
	 */
	CLASSIFICATION,
		
	/**
	* Количество режимов.
	*/
	MODE_COUNT
};

/**
 * Уровень аппроксимации кривых в алгоритме Дугласа-Пейкера.
 */
const int STANDARD_APPROX_LEVEL = 3;

/**
* Множество из строк.
*/
typedef std::set<std::string>  StrSet;

/**
 * Типы для описания контуров объектов на плоскости.
 * 
 * @{
 */
typedef std::vector<cv::Point> Contour;

typedef std::vector< std::vector<cv::Point> > Contours;
/**
 * @}
 */

/**
 * Коэффициент линейного уменьшения, которое применяется к каждому пришедшему на обработку кадру.
 */
int& frameMinification();

/**
 * Верхнее ограничение на размер блока, @see getSizeInBlocks().
 * 
 * @{
 */
int getMaxBlockWidth();
int getMaxBlockHeight();
/**
 * @}
 */

/**
 * Возвращает среднее значение в прямоугольнике для выбраного канала.
 * 
 * @param image исходное изображение.
 * @param x начальный столбец.
 * @param y начальная строка.
 * @param sizeX ширина прямоугольника.
 * @param sizeY высота прямоугольника.
 * @param channel число каналов изображения.
 * @return среднее значение в прямоугольнике для выбраного канала.
 */
float getAverageChanelValueInRect(const cv::Mat &image, int x, int y, int sizeX, int sizeY, int channel);

/**
 * Декодировать изображение средствами OpenCV.
 * @throws std::string при неверном формате.
 */
cv::Mat decodeImage(const std::vector<char>& encodedImage);

/**
 * Для двумерной матрицы найти наибольший размер прямоугольного блока, которым можно "замостить" матрицу без остатка.
 * @param src матрица.
 * @returns размер матрицы, выраженный в блоках.
 * @note размер блока ограничен сверху значением getMaxBlockWidth() x getMaxBlockHeight().
 */
cv::Size getSizeInBlocks(const cv::Mat& src);

/**
 * Создаёт список объектов, найденных на маске.
 * 
 * @param mask входная маска, 1 - объект, 0 - не объект.
 * @param approxLevel параметр алгоритма Дугласа-Пейкера.
 * @param minObjectArea минимальная площадь объекта.
 * @param maxObjectArea максимальная площадь объекта.
 * @return список объектов.
 */
std::list<Object> createObjectList(const cv::Mat& mask, double approxLevel, int minObjectArea = std::numeric_limits<int>().min(), int maxObjectArea = std::numeric_limits<int>().max());


/**
 * Возвращает битовую карту для блоков изображения в виде матрицы cv::Mat.
 * 
 * @param mask исходное изображение.
 * @param blockSize размер блока.
 * @param pixelsThresholdPercent процент заполнености блока, для признания его частью объекта.
 */
cv::Mat getBitMap(const cv::Mat& mask, const cv::Size& blockSize, int pixelsThresholdPercent);

/**
 * Преобразует битовую карту в строку формате в xml.
 *
 * @param mask битовая маска.
 * @param blockSize размер блока в пикселях.
 * @return строка в формате xml.
 */
std::string getXMLBitMap(const cv::Mat& mask, cv::Size blockSize);

/**
 * Преобразует список объектов в строку формате в xml. 
 * 
 * @param objects список объектов.
 * @param blockSize размер блока в пикселях.
 * @param minObjectArea минимальная площадь объекта.
 * @param maxObjectArea максимальная площадь объекта.
 * @return строка в формате xml.
 */
std::string getXMLObjectList(std::list<Object> objects, cv::Size blockSize, int minObjectArea = std::numeric_limits<int>().min(), int maxObjectArea = std::numeric_limits<int>().max());

/**
 * Подсчитать количество ненулевых элементов в прямоугольнике одноканальной маски.
 */
int countNonZeroPixelsInMaskRange(const cv::Mat& mask, int x, int y, int width, int height);

/**
 * Находит количество белых(значение которых равно 255) пикселей в прямоугольной области, заданным левой верхней и правой нижней вершинами.
 *
 * @param img одноканальное изображение, глубина -- 8 бит.
 * @param left_top верхняя левая вершина прямоугольной области.
 * @param down_right нижняя правая вершина прямоугольной области.
 * @return количество белых пикселей(значение которых равно 255).
 */
int findNumWhitePixels(cv::Mat& img, CvPoint left_top, CvPoint down_right);

/**
 * Копирует пиксель из одного изображения в другое.
 * 
 * @param src изображение из которого нужно скопировать пиксель.
 * @param srcX абсцисса пикселя в исходном изображении @a src.
 * @param srcY ордината пикселя в исходном изображении @a src.
 * @param dst изображение в которое нужно скопировать пиксель.
 * @param dstX абсцисса пикселя в изображении @a dst.
 * @param dstY ордината пикселя в изображении @a dst.
 */
void copyPixel(const cv::Mat& src, int srcX, int srcY, cv::Mat& dst, int dstX, int dstY, int numChannels = 3); // РК: Channel.

/**
 * Производит поворот изображения @a img в положение определяемое параметром по часовой стрелке.
 * 
 * @param img исходное изображение.
 * @param numberOfDegreeClockwiseRotations параметра задающий порот изображаения, 
 * 0 - поворот на 0 градусов, 1 - поворот на 90 градусов, 2 - поворот на 180 градусов, 3 - поворот на 270 градусов. 
 * @return повёрнутое в заданное положение изображение.
 * @throws
 */
cv::Mat rotateImageClockwise(const cv::Mat& img, int numberOf90DegreeClockwiseRotations); // РК: rotateDegreeClockwise -> rotateImageClockwise, numberOfDegreeClockwiseRotations -> numberOf90DegreeClockwiseRotations.

/**
 * Производит поворот изображения на 90, 180, 270 градусов, соответственно, по часовой стрелке.
 * 
 * @param img изображение которое нужно повернуть.
 * @return повернутое изображение.
 * @{
 */	
cv::Mat rotate90DegreeClockwise(const cv::Mat& img);

cv::Mat rotate180DegreeClockwise(const cv::Mat& img);

cv::Mat rotate270DegreeClockwise(const cv::Mat& img);
/**
 * @}
 */

/**
 * Абстрактный класс для замера времени.
 */
class StatisticsTimer {

public:
	
	/**
	 * Возвращает время(мс) прошедшее с начала создания объекта.
	 */
	virtual int getMillisecondTime() = 0;
	
};

/**
 * Класс для замера астрономического времени.
 */
class StatisticsAstronomicalTimer : public StatisticsTimer {

private:
	
	/**
	 * Время начала работы таймера.
	 */
	boost::posix_time::ptime startTime;
	
public:
	
	/**
	 * Создаёт объект класса.
	 */
	StatisticsAstronomicalTimer() : startTime(boost::posix_time::microsec_clock::local_time()) {};

	/**
	 * Возвращает время(мс) прошедшее с начала создания объекта.
	 */
	virtual int getMillisecondTime() {
		return (boost::posix_time::microsec_clock::local_time() - startTime).total_milliseconds();
	};

};

/**
 * Класс для замера процессорного времени.
 */
class StatisticsProcessorTimer : public StatisticsTimer {

private:
	
	/**
	 * Время начала работы таймера.
	 */
	timespec startProcTime;
	
public:
	
	/**
	 * Создаёт объект класса.
	 */
	StatisticsProcessorTimer() {
		if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &startProcTime) < 0 ) {
			errors::throwException(errors::ERR_01_CLOCK_GETTIME_FAILED);
		}
	};
	
	/**
	 * Возвращает время(мс) прошедшее с начала создания объекта.
	 */
	virtual int getMillisecondTime() {
		timespec endProcTime;
		if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &endProcTime) < 0 ) {
			errors::throwException(errors::ERR_01_CLOCK_GETTIME_FAILED);
		}
		int sec = endProcTime.tv_sec - startProcTime.tv_sec;
		int nsec = endProcTime.tv_nsec - startProcTime.tv_nsec;
		return (sec * 1e9 + nsec) / 1e6;
	};
	
};

} // namespace AnCommon

#endif // AnCommon_h_
