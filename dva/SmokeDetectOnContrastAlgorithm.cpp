#include "SmokeDetectOnContrastAlgorithm.h"
#include <algorithm>
#include <logging/logging.hpp>
#include "AnCommon.h"
#include "System.h"
#include <utils/maputils.hpp>

/**
 * Радиус фильтра. 
 */
static const int MORPHOLOGY_KERNEL_RADIUS = 1;

const std::string SmokeDetectOnContrastAlgorithm::SMOKE_DETECT_ON_CONTRAST = "SMOKE_DETECT_ON_CONTRAST";

const float SmokeDetectOnContrastAlgorithm::SmokeDetectOnContrastAlgorithmSettings::THRESHOLD = 0.7f; 
const float SmokeDetectOnContrastAlgorithm::SmokeDetectOnContrastAlgorithmSettings::EMA_ALPHA = 0.01f;
const int SmokeDetectOnContrastAlgorithm::SmokeDetectOnContrastAlgorithmSettings::EMA_DELAY = 150; 

SmokeDetectOnContrastAlgorithm::SmokeDetectOnContrastAlgorithmSettings::SmokeDetectOnContrastAlgorithmSettings()
	: threshold_(THRESHOLD)
	, emaAlpha_(EMA_ALPHA)
	, emaDelay_(EMA_DELAY)
{}
	
SmokeDetectOnContrastAlgorithm::SmokeDetectOnContrastAlgorithmSettings::SmokeDetectOnContrastAlgorithmSettings(
						float threshold,
						float emaAlpha,
						int emaDelay)
	: threshold_(threshold)
	, emaAlpha_(emaAlpha)
	, emaDelay_(emaDelay)
{}

SmokeDetectOnContrastAlgorithm::SmokeDetectOnContrastAlgorithm(const SmokeDetectOnContrastAlgorithm::SmokeDetectOnContrastAlgorithmSettings &settings)
	: settings_(settings)
{}

void SmokeDetectOnContrastAlgorithm::setSettings(const xml::Request::Params &settings, AnCommon::StrSet &usedSettings) {
	try { 
		
		std::string lexCastError = errors::ERR_05_SETTINGS_CAN_NOT_BE_APPLIED_INCORRECT_PARAMETER;
		std::string error = errors::ERR_06_SETTINGS_CAN_NOT_BE_APPLIED_PARAMETER_NOT_FOUND;
		
		{
			std::string paramName = "threshold";
			settings_.threshold_ = System::throwingLexCast<std::string, float>(MapUtils::value(settings, paramName, error + paramName), lexCastError + paramName);
			usedSettings.insert(paramName);
		}
		
		{
			std::string paramName = "emaAlpha";
			settings_.emaAlpha_ = System::throwingLexCast<std::string, float>(MapUtils::value(settings, paramName, error + paramName), lexCastError + paramName);
			usedSettings.insert(paramName);
		}
		
		{
			std::string paramName = "emaDelay";
			settings_.emaDelay_ = System::throwingLexCast<std::string, int>(MapUtils::value(settings, paramName, error + paramName), lexCastError + paramName);
			usedSettings.insert(paramName);
		}
		
	} catch (std::string& err) {
		errors::throwException(err);
	} catch (...) {
		errors::throwException(errors::ERR_04_SETTINGS_CAN_NOT_BE_APPLIED);
	}
}
	
void SmokeDetectOnContrastAlgorithm::getSettings(xml::Request::Params &settings) {
	settings["threshold"] = boost::lexical_cast<std::string>(settings_.threshold_);
	settings["emaAlpha"] = boost::lexical_cast<std::string>(settings_.emaAlpha_);
	settings["emaDelay"] = boost::lexical_cast<std::string>(settings_.emaDelay_);
}
	
void SmokeDetectOnContrastAlgorithm::setSettings(const SmokeDetectOnContrastAlgorithm::SmokeDetectOnContrastAlgorithmSettings &settings) {
	settings_ = settings;
}
	
SmokeDetectOnContrastAlgorithm::SmokeDetectOnContrastAlgorithmSettings SmokeDetectOnContrastAlgorithm::getSettings() {
	return settings_;
}

std::string SmokeDetectOnContrastAlgorithm::getType() {
	return SMOKE_DETECT_ON_CONTRAST;
}
	
void SmokeDetectOnContrastAlgorithm::clear() {
	vecSmokeContrastData_.clear();
	tempHSVImage.setTo(cv::Scalar(CV_RGB(0,0,0)));
	tempVImage.setTo(cv::Scalar(CV_RGB(0,0,0)));
	tempMorphologyResult.setTo(cv::Scalar(CV_RGB(0,0,0)));
}

cv::Mat SmokeDetectOnContrastAlgorithm::detect(const cv::Mat& img, const cv::Size &blockSize) {
	setBlockSize(blockSize);
	return detect(img);
}
	
cv::Mat SmokeDetectOnContrastAlgorithm::detect(const cv::Mat& img) {
	LOG_TRACE("SmokeDetectOnContrast::detect begins"); 
	
	
	int imgSizeX = img.size().width;
	int imgSizeY = img.size().height;

	int blockSizeX = blockSize_.width;
	int blockSizeY = blockSize_.height;
	
	LOG_TRACE("blockSizeX = " << blockSize_.width << "blockSizeY = " << blockSize_.height); 

	int blocksPerX = (imgSizeX - 1) / blockSizeX + 1;
	int blocksPerY = (imgSizeY - 1) / blockSizeY + 1;

	LOG_TRACE("cv::Mat result"); 
	cv::Mat result(cv::Size(blocksPerX, blocksPerY), CV_8U, CV_RGB(0,0,0)); 

	if (tempHSVImage.size() != img.size()) {
		tempHSVImage = cv::Mat(img.size(), IPL_DEPTH_8U, 3);
	}

	if (tempVImage.size() != img.size()) {
		tempVImage = cv::Mat(img.size(), IPL_DEPTH_8U, 3);
	}

	if (tempMorphologyResult.size() != img.size()) {
		tempMorphologyResult = cv::Mat(img.size(), IPL_DEPTH_8U, 1);
	}

	if (vecSmokeContrastData_.size() != static_cast<size_t>(blocksPerX * blocksPerY)) {
		vecSmokeContrastData_.resize(blocksPerX * blocksPerY);
	}
	
	bool modeAND = false;
	
	cv::cvtColor(img, tempHSVImage, CV_BGR2HSV);
	int indices[] = {2, 0};
	std::vector<cv::Mat> v(1, tempHSVImage);
	std::vector<cv::Mat> w(1, tempVImage);
	cv::mixChannels(&tempHSVImage, 1, &tempVImage, 1, indices, 1);
	cv::Mat kern = cv::getStructuringElement(CV_SHAPE_ELLIPSE, cv::Size(MORPHOLOGY_KERNEL_RADIUS * 2 + 1, MORPHOLOGY_KERNEL_RADIUS * 2 + 1), cv::Point(MORPHOLOGY_KERNEL_RADIUS, MORPHOLOGY_KERNEL_RADIUS));
	cv::morphologyEx(tempVImage, tempMorphologyResult, CV_MOP_GRADIENT, kern);
	
	for (int y = 0, i = 0; y < imgSizeY; y += blockSizeY) {
		for (int x = 0; x < imgSizeX; x += blockSizeX, i++) {
	
			float average = AnCommon::getAverageChanelValueInRect(tempMorphologyResult, x, y, std::min(blockSizeX, imgSizeX - x), std::min(blockSizeY, imgSizeY - y), 0); //текущая контрастность в блоке
			std::list<float> &emaBuf = vecSmokeContrastData_[i].emaBuf;
	
			uchar& maskItem = result.ptr(y / blockSizeY)[x / blockSizeX];
	
			if (vecSmokeContrastData_[i].isSmoke) { // если данный блок уже идентифицрован как дым.
				// FM: гарантируется ли наличие front()?
				if (average >= emaBuf.front() * (0.5f + settings_.threshold_ / 2.f)) { // если блок можно перестать считать задымленным.
					vecSmokeContrastData_[i].isSmoke = false;
				}
				if (!modeAND || maskItem) { // если текущее значение маски зависит от того, является ли блок задымленным, устанавливаем значение маски.
					maskItem = vecSmokeContrastData_[i].isSmoke; // возможные значения -- 0 или 1, по п. 4.7.4 стандарта
				}

			} else { // если до данного момента блок не считался задымленным
				if (static_cast<int>(emaBuf.size()) >= settings_.emaDelay_) { // если уже накоплено достаточно кадров.
					if (!modeAND || maskItem) { // если текущее значение маски зависит от того, является ли блок задымленным, устанавливаем значение маски.
						maskItem = average < emaBuf.front() * settings_.threshold_;  // возможные значения -- 0 или 1, по п. 4.7.4 стандарта
					}
					if (average < emaBuf.front() * settings_.threshold_) { // если блок стал считаться задымленным, отмечаем его.
						vecSmokeContrastData_[i].isSmoke = true;
					}
				} else if (!modeAND) { // если кадров не накоплено, то не считаем за дым.
					maskItem = 0;
				}
			}

			if (!vecSmokeContrastData_[i].isSmoke) { // если кадр не дымный, то обновляем историю значений.
				if (emaBuf.size()) {
					emaBuf.push_back(emaBuf.back() * (1.f - settings_.emaAlpha_) + average * settings_.emaAlpha_);
				} else {
					emaBuf.push_back(average);
				}
				while (static_cast<int>(emaBuf.size()) > settings_.emaDelay_) {
					emaBuf.pop_front();
				}
			}
		}
	}
	LOG_TRACE("SmokeDetectOnContrast::detect end"); 
	
	return result;
}
