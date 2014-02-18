#include "FireDetectOnDynamicAlgorithm.h"
#include <algorithm>
#include <logging/logging.hpp>
#include "AnCommon.h"
#include "System.h"
#include <utils/maputils.hpp>

const std::string FireDetectOnDynamicAlgorithm::FIRE_DETECT_ON_DYNAMIC = "FIRE_DETECT_ON_DYNAMIC";

const double FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithmSettings::MIN_FIRE_DELTA = 0.5;
const int FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithmSettings::FIRED_PIXELS_THRESHOLD_PERCENT = 20;
const double FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithmSettings::SLIDING_AVG_ALPHA = 0.01;
const double FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithmSettings::MIN_FOREGROUND_PIXELS_PERCENT = 0.1;
const int FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithmSettings::FOREGROUND_TO_FIRE_MAX_RATIO = 100;

FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithmSettings::FireDetectOnDynamicAlgorithmSettings() 
	: minFireDelta_(MIN_FIRE_DELTA)
	, firedPixelsThresholdPercent_(FIRED_PIXELS_THRESHOLD_PERCENT)
	, slidingAvgAlpha_(SLIDING_AVG_ALPHA)
	, minForegroundPixelsPercent_(MIN_FOREGROUND_PIXELS_PERCENT)
	, foregroundToFireMaxRatio_(FOREGROUND_TO_FIRE_MAX_RATIO)  
{}
		
FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithmSettings::FireDetectOnDynamicAlgorithmSettings(
																		double minFireDelta,
																		int firedPixelsThresholdPercent,
																		double slidingAvgAlpha,
																		double minForegroundPixelsPercent,
																		int foregroundToFireMaxRatio) 
	: minFireDelta_(minFireDelta)
	, firedPixelsThresholdPercent_(firedPixelsThresholdPercent)
	, slidingAvgAlpha_(slidingAvgAlpha)
	, minForegroundPixelsPercent_(minForegroundPixelsPercent)
	, foregroundToFireMaxRatio_(foregroundToFireMaxRatio)
{}
	
FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithm() 
	: totalBgAvg_(CHANNELS)
	, foregroundPixelCount_(0)
	, firedPixelCount_(0)
{}
	
FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithm(const FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithmSettings& settings)
	: settings_(settings)
	, totalBgAvg_(CHANNELS)
	, foregroundPixelCount_(0)
	, firedPixelCount_(0)
{}

std::string FireDetectOnDynamicAlgorithm::getType() {
	return FIRE_DETECT_ON_DYNAMIC;
}

bool FireDetectOnDynamicAlgorithm::pixelIsForeground(int x, int y) {

	float b = static_cast<float>(currentFrameBGR_.ptr(y)[x * CHANNELS + 0]);
	float g = static_cast<float>(currentFrameBGR_.ptr(y)[x * CHANNELS + 1]);
	float r = static_cast<float>(currentFrameBGR_.ptr(y)[x * CHANNELS + 2]);
	
	bool isFire =
		r > g &&
		g > b &&
		r <= 1.143f * g + 175.f &&
		r > 220.f &&
		(r < 250.f || g < 250.f || b < 250.f);

	if (!isFire) {
		return false;
	}

	int h = static_cast<float>(currentFrameHSV_.ptr(y)[x * CHANNELS + 0]);
	int s = static_cast<float>(currentFrameHSV_.ptr(y)[x * CHANNELS + 1]);
	int v = static_cast<float>(currentFrameHSV_.ptr(y)[x * CHANNELS + 2]);

	isFire = (h <= 40 || (160 <= h && h <= 180)) && s + v >= 255;
	if (!isFire) {
		return false;	
	}

	float Y  = static_cast<float>(currentFrameYCrCb_.ptr(y)[x * CHANNELS + 0]);
	float cr = static_cast<float>(currentFrameYCrCb_.ptr(y)[x * CHANNELS + 1]);
	float cb = static_cast<float>(currentFrameYCrCb_.ptr(y)[x * CHANNELS + 2]);

	isFire =
		cr <= 1.25f * cb + 150 &&
		cr >= 150.f - 7.f / 30.f * cb &&
		cr <= 430.f - 2.1f * cb &&
		cr <= 225.f &&
		Y  <= 0.24 * cb + 225.f &&
		cb <= 140;
		
	return isFire;
}

void FireDetectOnDynamicAlgorithm::fillForegroundMask() {
	foregroundPixelCount_ = 0;
	for (int y = 0; y < currentFrameBGR_.size().height; y++) {
		for (int x = 0; x < currentFrameBGR_.size().width; x++) {
			uchar& maskItem = foregroundMask_.ptr(y)[x];
			maskItem = pixelIsForeground(x, y);
			foregroundPixelCount_ += maskItem;
		}
	}
	LOG_TRACE("Foreground pixels: " << foregroundPixelCount_);
}

void FireDetectOnDynamicAlgorithm::updateSlidingAvg(int x, int y) {
	for (size_t i = 0; i < CHANNELS; i++) {
		float& avg = reinterpret_cast<float*>(slidingAvg_.ptr(y))[x * CHANNELS + i];
		avg = (1.0 - settings_.slidingAvgAlpha_) * avg + settings_.slidingAvgAlpha_ * std::abs(currentFrameBGR_.ptr(y)[x + i] - prevImg_.ptr(y)[x + i]);
	}
}

void FireDetectOnDynamicAlgorithm::updateAverages() {
	for (size_t i = 0; i < CHANNELS; i++) {
		totalBgAvg_[i] = 0.0;
	}
	
	for (int y = 0; y < currentFrameBGR_.size().height; y++) {
		for (int x = 0; x < currentFrameBGR_.size().width; x++) {
			updateSlidingAvg(x, y);
			if (foregroundMask_.ptr(y)[x] == 0) {
				for (size_t i = 0; i < CHANNELS; i++) {
					totalBgAvg_[i] += reinterpret_cast<float*>(slidingAvg_.ptr(y))[x * CHANNELS + i];
				}
			}
		}
	}
	
	int backgroundPixelCount = currentFrameBGR_.size().area() - foregroundPixelCount_;
	for (size_t i = 0; i < CHANNELS; i++) {
		totalBgAvg_[i] /= backgroundPixelCount;
	}
}

void FireDetectOnDynamicAlgorithm::recreateMatrixIfNeeded(cv::Mat& m, int type, cv::Scalar initialValue) {
	if (m.size() != currentFrameBGR_.size()) {
		m = cv::Mat(currentFrameBGR_.size(), type, initialValue); 
	}
}

void FireDetectOnDynamicAlgorithm::fillPerPixelResultMask() {
	firedPixelCount_ = 0;
	float avgDelta = 0.0;
	for (int y = 0; y < currentFrameBGR_.size().height; y++) {
		for (int x = 0; x < currentFrameBGR_.size().width; x++) {
			uchar& result = perPixelResultMask_.ptr(y)[x];
			result = 0;
			if (foregroundMask_.ptr(y)[x] == 1) {
				double delta = 0.0;
				for (size_t i = 0; i < CHANNELS; i++) {
					delta += std::max(reinterpret_cast<float*>(slidingAvg_.ptr(y))[x * CHANNELS + i] - totalBgAvg_[i], 0.0);
				}
				avgDelta += delta / CHANNELS;
				if (delta / CHANNELS > settings_.minFireDelta_) {
					result = 1;
					firedPixelCount_++;
				}
			}
		}
	}
	LOG_TRACE("Average background fluctuations = (" << totalBgAvg_[0] << ", " << totalBgAvg_[1] << ", " << totalBgAvg_[2] << ")");
	LOG_TRACE("Average delta for foreground pixels = " << avgDelta / foregroundPixelCount_);
	LOG_TRACE("Fired pixels: " << firedPixelCount_);
}

cv::Mat FireDetectOnDynamicAlgorithm::detect(const cv::Mat& img) {
	LOG_TRACE("FireDetectOnDynamic::detectFire begins"); 

	if (!img.isContinuous()) {
		errors::throwException(errors::ERR_08_IMAGE_MUST_BE_CONTINUOUS);
	}
	currentFrameBGR_ = img;
	
	LOG_TRACE("if (prevImg_.size() == currentFrameBGR_.size())"); 
	if (prevImg_.size() == currentFrameBGR_.size()) {

		LOG_TRACE("Creating HSV copy");
		cv::cvtColor(currentFrameBGR_, currentFrameHSV_, CV_BGR2HSV);
		LOG_TRACE("Creating YUV copy");
		cv::cvtColor(currentFrameBGR_, currentFrameYCrCb_, CV_BGR2YCrCb);

		LOG_TRACE("Recreating internal matrices");
		recreateMatrixIfNeeded(slidingAvg_, CV_32FC3, CV_RGB(0, 0, 0));
		recreateMatrixIfNeeded(foregroundMask_, CV_8UC1, CV_RGB(0, 0, 0));
		recreateMatrixIfNeeded(perPixelResultMask_, CV_8UC1, CV_RGB(0, 0, 0));

		LOG_TRACE("fillForegroundMask");
		fillForegroundMask();

		LOG_TRACE("Updating averages");
		updateAverages();

		if (foregroundPixelCount_ >= settings_.minForegroundPixelsPercent_) {
			LOG_TRACE("Filling per pixel results");
			fillPerPixelResultMask();

			if (firedPixelCount_ != 0 && foregroundPixelCount_ / firedPixelCount_ <= settings_.foregroundToFireMaxRatio_) {
				LOG_TRACE("Return result");
				return perPixelResultMask_;
			} else {
				LOG_TRACE("Too few fired pixels among foreground pixels");
			}
			
		} else {
			LOG_TRACE("Too few foreground pixels");
		}
		
	} else {
		LOG_TRACE("Frame size has been changed.");
	}
	
	currentFrameBGR_.copyTo(prevImg_);
	
	return perPixelResultMask_;
}

void FireDetectOnDynamicAlgorithm::setSettings(const xml::Request::Params &settings, AnCommon::StrSet &usedSettings) {
	try { 
		
		std::string lexCastError = errors::ERR_05_SETTINGS_CAN_NOT_BE_APPLIED_INCORRECT_PARAMETER;
		std::string error = errors::ERR_06_SETTINGS_CAN_NOT_BE_APPLIED_PARAMETER_NOT_FOUND;
		
		{
			std::string paramName = "foregroundToFireMaxRatio";
			settings_.foregroundToFireMaxRatio_ = System::throwingLexCast<std::string, int>(MapUtils::value(settings, paramName, error + paramName), lexCastError + paramName);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "slidingAvgAlpha";
			settings_.slidingAvgAlpha_ = System::throwingLexCast<std::string, double>(MapUtils::value(settings, paramName, error + paramName), lexCastError + paramName);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "minForegroundPixelsPercent";
			settings_.minForegroundPixelsPercent_ = System::throwingLexCast<std::string, double>(MapUtils::value(settings, paramName, error + paramName), lexCastError + paramName);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "minFireDelta";
			settings_.minFireDelta_ = System::throwingLexCast<std::string, double>(MapUtils::value(settings, paramName, error + paramName), lexCastError + paramName);
			usedSettings.insert(paramName);
		}
	
	} catch (std::string& err) {
		errors::throwException(err);
	} catch (...) {
		errors::throwException(errors::ERR_04_SETTINGS_CAN_NOT_BE_APPLIED);
	}
	
}
	
void FireDetectOnDynamicAlgorithm::getSettings(xml::Request::Params &settings) {
	settings["minFireDelta"] = boost::lexical_cast<std::string>(settings_.minFireDelta_);
	settings["slidingAvgAlpha"] = boost::lexical_cast<std::string>(settings_.slidingAvgAlpha_);
	settings["minForegroundPixelsPercent"] = boost::lexical_cast<std::string>(settings_.minForegroundPixelsPercent_);
	settings["foregroundToFireMaxRatio"] = boost::lexical_cast<std::string>(settings_.foregroundToFireMaxRatio_);
}
	
void FireDetectOnDynamicAlgorithm::setSettings(const FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithmSettings &settings) {
	settings_ = settings;
}
	
FireDetectOnDynamicAlgorithm::FireDetectOnDynamicAlgorithmSettings FireDetectOnDynamicAlgorithm::getSettings() {
	return settings_;
}
	
void FireDetectOnDynamicAlgorithm::clear() {
	currentFrameBGR_.setTo(cv::Scalar(CV_RGB(0,0,0)));
	currentFrameHSV_.setTo(cv::Scalar(CV_RGB(0,0,0)));
	currentFrameYCrCb_.setTo(cv::Scalar(CV_RGB(0,0,0)));
	foregroundMask_.setTo(cv::Scalar(CV_RGB(0,0,0)));
	totalBgAvg_ = std::vector<double>(CHANNELS);
	foregroundPixelCount_ = 0;
	firedPixelCount_ = 0;
	perPixelResultMask_.setTo(cv::Scalar(CV_RGB(0,0,0)));
	prevImg_.setTo(cv::Scalar(CV_RGB(0,0,0)));
}
