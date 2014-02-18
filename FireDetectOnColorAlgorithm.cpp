#include "FireDetectOnColorAlgorithm.h"

const std::string FireDetectOnColorAlgorithm::FIRE_DETECT_ON_COLOR_ALGORITHM = "FIRE_DETECT_ON_COLOR_ALGORITHM";

std::string FireDetectOnColorAlgorithm::getType() {
	return FIRE_DETECT_ON_COLOR_ALGORITHM;
}
	
bool FireDetectOnColorAlgorithm::pixelIsForeground(const cv::Mat &img, int x, int y) {

	float b = static_cast<float>(img.ptr(y)[x * CHANNELS + 0]);
	float g = static_cast<float>(img.ptr(y)[x * CHANNELS + 1]);
	float r = static_cast<float>(img.ptr(y)[x * CHANNELS + 2]);
	
	bool isFire =
		r > g &&
		g > b &&
		r <= 1.143f * g + 175.f &&
		r > 220.f &&
		(r < 250.f || g < 250.f || b < 250.f);

	if (!isFire) {
		return false;
	}

	int h = static_cast<float>(img.ptr(y)[x * CHANNELS + 0]);
	int s = static_cast<float>(img.ptr(y)[x * CHANNELS + 1]);
	int v = static_cast<float>(img.ptr(y)[x * CHANNELS + 2]);

	isFire = (h <= 40 || (160 <= h && h <= 180)) && s + v >= 255;
	if (!isFire) {
		return false;	
	}

	float Y  = static_cast<float>(img.ptr(y)[x * CHANNELS + 0]);
	float cr = static_cast<float>(img.ptr(y)[x * CHANNELS + 1]);
	float cb = static_cast<float>(img.ptr(y)[x * CHANNELS + 2]);

	isFire =
		cr <= 1.25f * cb + 150 &&
		cr >= 150.f - 7.f / 30.f * cb &&
		cr <= 430.f - 2.1f * cb &&
		cr <= 225.f &&
		Y  <= 0.24 * cb + 225.f &&
		cb <= 140;
		
	return isFire;
}

cv::Mat FireDetectOnColorAlgorithm::detect(const cv::Mat& img) {
	int foregroundPixelCount = 0;
	cv::Mat result(img);
	result.setTo(cv::Scalar(CV_RGB(0,0,0)));
	for (int y = 0; y < img.size().height; y++) {
		for (int x = 0; x < img.size().width; x++) {
			uchar& maskItem = result.ptr(y)[x];
			maskItem = pixelIsForeground(img, x, y);
			foregroundPixelCount += maskItem;
		}
	}
	LOG_TRACE("Foreground pixels: " << foregroundPixelCount);
	return result;
}