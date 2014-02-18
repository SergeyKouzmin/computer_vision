#include "AnCommon.h"
#include <logging/logging.hpp>

namespace AnCommon {

int& frameMinification() {
	static int value = 1;
	return value;
}

int getMaxBlockWidth() {
	return 20 / frameMinification();
}

int getMaxBlockHeight() {
	return 20 / frameMinification();
}
	
float getAverageChanelValueInRect(const cv::Mat& image, int x, int y, int sizeX, int sizeY, int channel) {
	float c = 0;
	for (int j = y; j < y + sizeY; j++) {
		for (int i = x; i < x + sizeX; i++) {
			c += image.ptr(j)[i * image.channels() + channel];
		}
	}
	c /= sizeX * sizeY;
	return c;
}

cv::Mat decodeImage(const std::vector<char>& encodedImage) {
	
	cv::Mat encoded(1, encodedImage.size(), CV_8UC3, const_cast<void*>(reinterpret_cast<const void*>(&encodedImage[0])));
	
	cv::Mat result = cv::imdecode(encoded, 1 /* 3-channel color image */);
	
	if (result.empty()) {
		errors::throwException(errors::ERR_00_UNABLE_TO_DECODE_IMAGE);
	}
	
	return result;
}

cv::Size getSizeInBlocks(const cv::Mat& src) {
	
	int blockW = getMaxBlockWidth();
	int blockH = getMaxBlockHeight();

	while (src.size().width % blockW != 0) {
		blockW--;
	}

	while (src.size().height % blockH != 0) {
		blockH--;
	}

	return cv::Size(src.size().width / blockW, src.size().height / blockH);
}

int countNonZeroPixelsInMaskRange(const cv::Mat& mask, int x, int y, int width, int height) {
	int result = 0;
	int left = std::max(x, 0);
	int top = std::max(y, 0);
	int right = std::min(x + width, mask.size().width - 1);
	int bottom = std::min(y + height, mask.size().height - 1);
	for (int row = top; row <= bottom; row++) {
		for (int col = left; col <= right; col++) {
			result += mask.ptr(row)[col] != 0;
		}
	}
	return result;
}

int findNumWhitePixels(
	cv::Mat& img,
	CvPoint left_top,
	CvPoint down_right
) {
	long long counter = 0;
	if (!img.empty()) {
		for (int y = left_top.y; y < down_right.y; y++) {
			uchar* ptr = img.row(y).ptr();
			for (int x = left_top.x; x < down_right.x; x++) {
				if (ptr[x] != 0) {
					++counter;
				}
			}
		}
	}
	return counter;
}

void copyPixel(const cv::Mat& src, int srcX, int srcY, cv::Mat& dst, int dstX, int dstY, int numChannels) { // РК: Channel.
	for (int i = 0; i < numChannels; i++) {
		dst.ptr(dstY)[dstX * numChannels + i] = src.ptr(srcY)[srcX * numChannels + i];
	}
}

cv::Mat rotate90DegreeClockwise(const cv::Mat& img) {
	cv::Mat result(img.cols, img.rows, img.type());
	for (int y = 0; y < img.size().height; y++) {
		for (int x = 0; x < img.size().width; x++) {
			int newX = result.size().width - y - 1;
			copyPixel(img, x, y, result, newX, x, img.channels());
		}
	}
	return result;
}

cv::Mat rotate180DegreeClockwise(const cv::Mat& img) {
	cv::Mat result(img.rows, img.cols, img.type());
	for (int y = 0; y < img.size().height; y++) {
		for (int x = 0; x < img.size().width; x++) {
			int newX = result.size().width - x - 1;
			int newY = result.size().height - y - 1;
			copyPixel(img, x, y, result, newX, newY, img.channels());
		}
	}
	return result;
}

cv::Mat rotate270DegreeClockwise(const cv::Mat& img) {
	cv::Mat result(img.cols, img.rows, img.type());
	for (int y = 0; y < img.size().height; y++) {
		for (int x = 0; x < img.size().width; x++) {
			int newY = result.size().height - x - 1;
			copyPixel(img, x, y, result, y, newY, img.channels());
		}
	}
	return result;
}

cv::Mat rotateImageClockwise(const cv::Mat& img, int numberOf90DegreeClockwiseRotations) {
	if (!(0 <= numberOf90DegreeClockwiseRotations && numberOf90DegreeClockwiseRotations <= 3)) {
		errors::throwException(errors::ERR_11_INCORRECT_PARAMETER + ": numberOfDegreeClockwiseRotations");
	}
	
	if (numberOf90DegreeClockwiseRotations == 1) {
		return rotate90DegreeClockwise(img);
	} else if (numberOf90DegreeClockwiseRotations == 2) {
		return rotate180DegreeClockwise(img);
	} else if (numberOf90DegreeClockwiseRotations == 3) {
		return rotate270DegreeClockwise(img);
	}

	return img;
}

std::list<Object> createObjectList(const cv::Mat& mask, double approxLevel, int minObjectArea, int maxObjectArea) {
	u_int8_t zero = 0;
	cv::Mat buf(mask.size().height + 2, mask.size().width + 2, CV_8U, zero);
	LOG_TRACE("buf.size = " << buf.size().width << "x" << buf.size().height);
	LOG_TRACE("mask.size = " << mask.size().width << "x" << mask.size().height);
	for(int y = 1; y < mask.size().height + 1; y++) {
		for(int x = 1; x < mask.size().width + 1; x++) {
			buf.ptr(y)[x] = mask.ptr(y-1)[x-1];
		}
	}
	
	Contours maskContours;
	cv::findContours(const_cast<cv::Mat&>(buf), maskContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	std::list<Object> result;
	int idNum = 1;
	for (size_t i = 0; i < maskContours.size(); i++) {
		AnCommon::Contour newContour;
		cv::approxPolyDP(cv::Mat(maskContours[i], false), newContour, approxLevel, true);
		cv::Rect rect = cv::boundingRect(cv::Mat(newContour, false));
		if (minObjectArea <= rect.area() && rect.area() <= maxObjectArea) {
			// контуры искали в buf, а там координаты отличаются от mask.
			rect.x--;
			rect.y--;
			
			result.push_back(Object(idNum, rect));
			idNum++;
		}
	}

	return result;
}

cv::Mat getBitMap(const cv::Mat& mask, const cv::Size& blockSize, int pixelsThresholdPercent) {
	LOG_DEBUG("getBitMap begin");
	u_int8_t zero = 0;
	cv::Mat result = cv::Mat(cv::Size(mask.size().width / blockSize.width, mask.size().height / blockSize.height), CV_8U, zero);
	cv::Size truncatedSize(result.size().width * blockSize.width, result.size().height * blockSize.height);
	
	for (int y = 0, blockY = 0; y < truncatedSize.height; y += blockSize.height, blockY++) {
		for (int x = 0, blockX = 0; x < truncatedSize.width; x += blockSize.width, blockX++) {
			result.ptr(blockY)[blockX] = AnCommon::countNonZeroPixelsInMaskRange(mask, x, y, blockSize.width, blockSize.height) > pixelsThresholdPercent * blockSize.area() / 100;
		}
	}
	LOG_DEBUG("getBitMap end");
	
	return result;
}

std::string getXMLBitMap(const cv::Mat& mask, cv::Size blockSize) {
	std::stringstream result;
	result << "<blockSize>";
	{
		result << "<w>" << blockSize.width * AnCommon::frameMinification() << "</w>";
		result << "<h>" << blockSize.height * AnCommon::frameMinification() << "</h>";
	}
	result << "</blockSize>";

	result << "<data>";
	result << "\n";

	for (int y = 0; y < mask.size().height; y++) {
		result << "<line>";
		for (int x = 0; x < mask.size().width; x++) {
			if (mask.ptr(y)[x]) {
				result << "1";
			} else {
				result << "0";
			}
		}
		result << "</line>";
	}
	result << "</data>";
	
	return result.str();
}

std::string getXMLObjectList(std::list<Object> objects, cv::Size blockSize, int minObjectArea, int maxObjectArea) {
	std::stringstream result;
	result << "<objects>";
	for (std::list<Object>::iterator i = objects.begin(); i != objects.end(); i++) {
		cv::Rect rect = i->getRect();
		int area = blockSize.area() * rect.area() * frameMinification();
		LOG4CXX_TRACE(logger(), "Object's area = " << area);
		if (minObjectArea <= area && area <= maxObjectArea) {
			LOG4CXX_TRACE(logger(), "Area is in valid range, object #" << i->getId());
			result << "<object>";
			result << "<id>";
			result << i->getId();
			result << "</id>";
			result << "<points>";
			result << rect.x * blockSize.width * frameMinification();
			result << ",";
			result << rect.y * blockSize.height * frameMinification();
			result << ",";
			result << (rect.x + rect.width ) * blockSize.width * frameMinification();
			result << ",";
			result << (rect.y + rect.height) * blockSize.height * frameMinification();
			result << "</points>";
			result << "</object>";	
		}
	}
	result << "</objects>";
	
	return result.str();
}

} // namespace AnCommon 
