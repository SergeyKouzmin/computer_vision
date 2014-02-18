#include <boost/format.hpp>

#include "SmokeDetector.h"
#include <logging/logging.hpp>
#include "System.h"
#include <utils/macros.hpp>
#include <utils/maputils.hpp>
#include "AnCommon.h"
#include "RectMerger.h"

const std::string SmokeDetector::SMOKE_DETECTOR = "SMOKE_DETECTOR";

void SmokeDetector::on() {
	LOG_INFO("SmokeDetector::on");
	Detector::on();
}

void SmokeDetector::off() {
	LOG_INFO("SmokeDetector::off");
	Detector::off();
}
	
void SmokeDetector::execute(const cv::Mat& image, const boost::posix_time::ptime& imageTime, std::string& resultingXml) {
	LOG_INFO("SmokeDetector::execute begin");
	
	try {
		std::string dataDetector = detectSmoke(image);
		resultingXml = "<smokeDetect>" + dataDetector + "</smokeDetect>";
	} catch (std::string &error) {
		resultingXml = "<smokeDetect><error>" + error + "</error></smokeDetect>";
	} catch (...) {
		resultingXml = "<smokeDetect><error> undetifier error </error></smokeDetect>";
	}
	
	LOG_INFO("SmokeDetector::execute end");
}
	
std::string SmokeDetector::getType() {
	return SMOKE_DETECTOR;
}

std::string SmokeDetector::detectSmoke(const cv::Mat& image) {
	BEGIN_FUNCTION
	LOG_DEBUG("SmokeDetector::detectSmoke begin");

	int blockSizeX = image.size().width / settings_.numWidthBlocks_;
	int blockSizeY = image.size().height / settings_.numHeightBlocks_;

	LOG_DEBUG("detectSmoke detectorOnContrast");
	cv::Mat mask = smokeDetectOnContrastAlg_.detect(image, cv::Size(blockSizeX, blockSizeY));

	LOG_DEBUG("detectSmoke getXMLObjectList");
	std::string result = 
		AnCommon::getXMLObjectList(AnCommon::createObjectList(mask, 3.0), cv::Size(blockSizeX, blockSizeY), static_cast<int>(settings_.minSmokeArea_ * image.size().area() / 100)) + 
		AnCommon::getXMLBitMap(mask, cv::Size(blockSizeX, blockSizeY));

	LOG_DEBUG("SmokeDetector::detectSmoke end");

	return result;

	END_FUNCTION
}

void SmokeDetector::setSettings(xml::Request::Params params) {
	LOG_INFO("SmokeDetector::setSettings");

	try {
		
		AnCommon::StrSet usedSettings;
		
		std::string lexCastError = errors::ERR_05_SETTINGS_CAN_NOT_BE_APPLIED_INCORRECT_PARAMETER;
		std::string error = errors::ERR_06_SETTINGS_CAN_NOT_BE_APPLIED_PARAMETER_NOT_FOUND;
		
		// параметры для детектирования оставленных вещей
		{
			std::string paramName = "alertTime";
			settings_.alertTime_ = System::throwingLexCast<std::string, int>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "numWidthBlocks";
			settings_.numWidthBlocks_ = System::throwingLexCast<std::string, int>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "numHeightBlocks";
			settings_.numHeightBlocks_ = System::throwingLexCast<std::string, int>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "minSmokeArea";
			settings_.minSmokeArea_ = System::throwingLexCast<std::string, double>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		
		AnCommon::StrSet usedSettings1;
		// параметры для детектора отделения объектов от фона
		smokeDetectOnContrastAlg_.setSettings(params, usedSettings1);

		AnCommon::StrSet intersec;
		std::set_intersection(usedSettings.begin(), usedSettings.end(), usedSettings1.begin(), usedSettings1.end(),std::inserter(intersec, intersec.begin()));
		
		LOG_INFO("intersec.size: " << intersec.size());
		
		if (intersec.size()) {
			std::string intersecParams;
			for(AnCommon::StrSet::iterator i = intersec.begin(); i != intersec.end(); i++) {
				intersecParams += *i;
			}
			errors::throwException((boost::format("%s in SmokeDetector and %s") % errors::ERR_17_IDENTICAL_PARAMETERS_IN_DETECTOR % smokeDetectOnContrastAlg_.getType()).str());
		}

	} catch (std::string& err) {
		errors::throwException(err);
	} catch (...) {
		errors::throwException(errors::ERR_04_SETTINGS_CAN_NOT_BE_APPLIED);
	}
}

xml::Request::Params SmokeDetector::getSettings() {
	LOG_INFO("SmokeDetector::getSettings");

	xml::Request::Params set;
	// параметры для детектирования оставленных вещей
	set["alertTime"] = boost::lexical_cast<std::string>(settings_.alertTime_);
	set["numWidthBlocks"] = boost::lexical_cast<std::string>(settings_.numWidthBlocks_);
	set["numHeightBlocks"] = boost::lexical_cast<std::string>(settings_.numHeightBlocks_);
	set["minSmokeArea"] = boost::lexical_cast<std::string>(settings_.minSmokeArea_);

	// параметры для детектирования оставленных вещей
	smokeDetectOnContrastAlg_.getSettings(set);
	return set;
}
	
void SmokeDetector::clear() {
	smokeDetectOnContrastAlg_.clear();
}
