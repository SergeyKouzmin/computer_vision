#include "LeftThingsDetector.h"
#include "System.h"
#include <utils/maputils.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <logging/logging.hpp>
#include <utils/macros.hpp>
#include "AnCommon.h"
#include "RectMerger.h"
#include <algorithm>
#include "CodeBookAlgorithm.h"
#include "CodeBookAlgorithmV1.h"

const std::string LeftThingsDetector::LEFT_THINGS_DETECTOR = "LEFT_THINGS_DETECTOR";

LeftThingsDetector::LeftThingsDetector()
	: backgroundSeparationAlgorithm_(new CodeBookAlgorithm())
{}

void LeftThingsDetector::on() {
	LOG_INFO("LeftThingsDetector::on");
	Detector::on();
	LOG_INFO(" setActivationTime() ");
	setActivationTime();
}

void LeftThingsDetector::setSettings(xml::Request::Params params) {
	LOG_INFO("LeftThingsDetector::setSettings");

	try {
		
		AnCommon::StrSet usedSettings;
		
		std::string lexCastError = errors::ERR_05_SETTINGS_CAN_NOT_BE_APPLIED_INCORRECT_PARAMETER;
		std::string error = errors::ERR_06_SETTINGS_CAN_NOT_BE_APPLIED_PARAMETER_NOT_FOUND + " ";
		
		// параметры для детектирования оставленных вещей
		{
			std::string paramName = "alertTime";
			settings_.alertTime_ = System::throwingLexCast<std::string, int>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert("alertTime");
		}
		{
			std::string paramName = "interval";
			settings_.maxSupervisionTime_ = System::throwingLexCast<std::string, int>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "minObjectArea";
			settings_.minObjectArea_ = System::throwingLexCast<std::string, double>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "maxObjectArea";
			settings_.maxObjectArea_ = System::throwingLexCast<std::string, double>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "startingLearningPercent";
			settings_.startingLearningPercent_ = System::throwingLexCast<std::string, int>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "startingLearningTime";
			settings_.startingLearningTime_ = System::throwingLexCast<std::string, int>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "maxMergingGapPercent";
			settings_.maxMergingGapPercent_ = System::throwingLexCast<std::string, int>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "morphology";
			settings_.useMorphology_ = System::throwingLexCast<std::string, int>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "connectedComp";
			settings_.useConnectedComp_ = System::throwingLexCast<std::string, int>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			usedSettings.insert(paramName);
		}
		{
			std::string paramName = "mode";
			int mode = System::throwingLexCast<std::string, int>(MapUtils::value(params, paramName, error + paramName), lexCastError);
			if (mode < 0 || mode >= AnCommon::MODE_COUNT) {
				errors::throwException(errors::ERR_18_INVALID_MODE_VALUE);
			}
			mode_ = static_cast<AnCommon::Mode>(mode);
			usedSettings.insert(paramName);
		}

		AnCommon::StrSet usedSettings1;
		// параметры для детектора отделения объектов от фона
		backgroundSeparationAlgorithm_->setSettings(params, usedSettings1);

		AnCommon::StrSet intersec;
		std::set_intersection(usedSettings.begin(), usedSettings.end(), usedSettings1.begin(), usedSettings1.end(),std::inserter(intersec, intersec.begin()));
		
		LOG_INFO("intersec.size: " << intersec.size());
		
		if (intersec.size()) {
			std::string intersecParams;
			for(AnCommon::StrSet::iterator i = intersec.begin(); i != intersec.end(); i++) {
				intersecParams += *i;
			}
			throw std::string(std::string(errors::ERR_17_IDENTICAL_PARAMETERS_IN_DETECTOR)  + " in LeftThingsDetector and " + backgroundSeparationAlgorithm_->getType());
		}

	} catch (std::string& err) {
		LOG_DEBUG("Caught in LeftThingsDetector::setSettings:" << err);
		errors::throwException(err);
	} catch (...) {
		errors::throwException(errors::ERR_04_SETTINGS_CAN_NOT_BE_APPLIED);
	}
}

xml::Request::Params LeftThingsDetector::getSettings() {
	LOG_INFO("LeftThingsDetector::getSettings");
	
	xml::Request::Params set;

	// параметры для детектирования оставленных вещей
	set["alertTime"] = boost::lexical_cast<std::string>(settings_.alertTime_);
	set["interval"] = boost::lexical_cast<std::string>(settings_.maxSupervisionTime_);
	set["minObjectArea"] = boost::lexical_cast<std::string>(settings_.minObjectArea_);
	set["maxObjectArea"] = boost::lexical_cast<std::string>(settings_.maxObjectArea_);
	set["startingLearningTime"] = boost::lexical_cast<std::string>(settings_.startingLearningTime_);
	set["maxMergingGapPercent"] = boost::lexical_cast<std::string>(settings_.maxMergingGapPercent_);
	set["mode"] = boost::lexical_cast<std::string>(mode_);
	set["startingLearningPercent"] = boost::lexical_cast<std::string>(settings_.startingLearningPercent_);
	set["morphology"] = boost::lexical_cast<std::string>(settings_.useMorphology_);
	set["connectedComp"] = boost::lexical_cast<std::string>(settings_.useConnectedComp_);

	// параметры для детектирования оставленных вещей
	LOG_INFO("backgroundSeparationAlgorithm_->getSettings");
	backgroundSeparationAlgorithm_->getSettings(set);
	
	LOG_INFO("LeftThingsDetector::getSettings end");
	return set;
}

void LeftThingsDetector::execute(const cv::Mat& image, const boost::posix_time::ptime& imageTime, std::string& resultingXml) {
	LOG_INFO("LeftThingsDetector::execute");
	
	try {

		std::string dataDetector = findStandingObjects(image, imageTime);
		resultingXml = "<leftThings>" + dataDetector + "</leftThings>";


	} catch (std::string &error) {
		resultingXml = "<leftThings><error>" + error + "</error></leftThings>";
	} catch (...) {
		resultingXml = "<leftThings><error> undetifier error </error></leftThings>";
	}
	
	LOG_INFO("LeftThingsDetector::execute end");
}

void LeftThingsDetector::off() {
	LOG_INFO("LeftThingsDetector::off");
	clear();
	Detector::off();
}

std::string LeftThingsDetector::getType() {
	return LEFT_THINGS_DETECTOR;
}

std::string LeftThingsDetector::findStandingObjects(const cv::Mat& image, const boost::posix_time::ptime& date_time) {
	BEGIN_FUNCTION 

	if (activationTime_ != boost::posix_time::not_a_date_time) { 
		if (mode_ == AnCommon::CLASSIFICATION || 
			(boost::posix_time::microsec_clock::local_time() - activationTime_).total_milliseconds() > (settings_.startingLearningTime_ * 1000) 
		) {
			mode_ = AnCommon::CLASSIFICATION;
			activationTime_ = boost::posix_time::not_a_date_time;
		}
		
		if (mode_ == AnCommon::LEARNING) {
			settings_.startingLearningPercent_ = ceil((boost::posix_time::microsec_clock::local_time() - activationTime_).total_milliseconds() * 0.1  / settings_.startingLearningTime_);
			LOG_INFO("settings_.startingLearningPercent_ = " << settings_.startingLearningPercent_);	
		}
	}
	
	std::string result;
	
	LOG_INFO(" if (prevMode_ != mode_) ");
	if (prevMode_ != mode_) {
		LOG_INFO(" backgroundSeparationAlgorithm_->reset() ");
		backgroundSeparationAlgorithm_->reset();
		LOG_INFO(" backgroundSeparationAlgorithm_->clearStaleEntries() ");
		backgroundSeparationAlgorithm_->clearStaleEntries();
	}

	double delayForCodebookAlg = 1.1 * settings_.maxSupervisionTime_;

	if (mode_ == AnCommon::LEARNING) {

		LOG_INFO("Left things LEARNING");
		
		LOG_INFO("codeBookAlgorithm_.setLearningDelaySeconds");
		backgroundSeparationAlgorithm_->setLearningDelaySeconds(static_cast<int>(delayForCodebookAlg));
		
		LOG_INFO("codeBookAlgorithm_.learn");
		backgroundSeparationAlgorithm_->learn(image);
		
		LOG_INFO("codeBookAlgorithm_.detect end");
		
	} else if (mode_ == AnCommon::CLASSIFICATION) {

		LOG_INFO("Left things CLASSIFICATION");

		curFrame_ = image;

		LOG_INFO("AnCommon::getSizeInBlocks");
		CvSize blocks = AnCommon::getSizeInBlocks(image);

		settings_.numBlockWidth_ = blocks.width;
		settings_.numBlockHeight_ = blocks.height;


		LOG_DEBUG("settings_.num_block_width_ = " << settings_.numBlockWidth_);
		LOG_DEBUG("settings_.num_block_height_ = " << settings_.numBlockHeight_);
		LOG_DEBUG("settings_.max_supervision_time_1 = " << settings_.maxSupervisionTime_);

		// При измененившемся размере очередного кадра вся предыдущая история
		size_t newBlockCount = settings_.numBlockWidth_ * settings_.numBlockHeight_;
		if (objectsBlocks_.size() != newBlockCount) {
			objectsBlocks_ = std::vector<Block>(newBlockCount);
		}

		blockSize_.width = image.size().width / blocks.width;
		blockSize_.height = image.size().height / blocks.height;

		subFoneFrame_ = backgroundSeparationAlgorithm_->detect(curFrame_);

		// выполняется постобработка сегментированного изображения фильтром, который выбрал пользователь
		if (settings_.useMorphology_) {
			morphologyFilter_(subFoneFrame_);
		}
		
		if (settings_.useConnectedComp_) {
			connectedComponentsFilter_(subFoneFrame_);
		}
			
		cv::Mat img = subFoneFrame_;
		
		LOG_DEBUG("detectBlocksWithObject");
		detectBlocksWithObject(img, objectsBlocks_);
		
		LOG_DEBUG("detectStandingBlocks");
		detectStandingBlocks(objectsBlocks_, date_time);
		
		LOG_DEBUG("generateResultMatrix");
		cv::Mat mask = generateResultMatrix(objectsBlocks_);
		
		// Объединение близких прямоугольников
		std::list<AnCommon::Object> rects = AnCommon::createObjectList(mask, AnCommon::STANDARD_APPROX_LEVEL);
		RectMerger::Rect bounds = { 0, 0, mask.size().width - 1, mask.size().height - 1 };
		RectMerger merger(bounds);
		const std::list<AnCommon::Object>& mergedRects = merger.getMergedRects(rects, settings_.maxMergingGapPercent_ * mask.size().width / 100);

		LOG_DEBUG("getXMLBitMap");
		std::string bitMapStr = AnCommon::getXMLBitMap(mask, blockSize_);

		LOG_DEBUG("getXMLObjectList");
		result = AnCommon::getXMLObjectList(mergedRects, blockSize_, static_cast<int>(settings_.minObjectArea_ * image.size().area() / 100), 
														static_cast<int>(settings_.maxObjectArea_ * image.size().area() / 100));
		result += bitMapStr;
	}

	prevMode_ = mode_;
	
	LOG_DEBUG("returning from findStandingObjects");
	
	return result;
	
	END_FUNCTION
}

void LeftThingsDetector::detectBlocksWithObject(cv::Mat& img, std::vector<Block>& objectsBlocks) {
	int i = 0;
	for(int y = 0; y < img.size().height; y += blockSize_.height ) {
		for(int x = 0; x < img.size().width; x += blockSize_.width, i++) {
			objectsBlocks[i].isObject_ = isObjectBlock(img, x, y, std::min(blockSize_.width, img.size().width - x), std::min(blockSize_.height, img.size().height - y));
		}
	}
}

bool LeftThingsDetector::isObjectBlock(
									cv::Mat& img,
									int blockX,
									int blockY,
									int blockWidth,
									int blockHeight) {

	long int counter = 0;
	for(int y = blockY; y < blockY + blockHeight; y++ ) {
		CV_Assert(y >= 0 && y < img.size().height);
		uchar* ptr = (uchar*) (img.row(y).ptr());
		for(int x = blockX; x < blockX + blockWidth; x++) {
			if (ptr[x] != 0) {
				++counter;
			}
		}
	}

	return (counter > blockWidth * blockHeight * 0.95);
}

void LeftThingsDetector::detectStandingBlocks(std::vector<Block>& objectsBlocks, const boost::posix_time::ptime& dateTime) {
	for(size_t i = 0; i < objectsBlocks.size(); i++) {
		if (objectsBlocks[i].isObject_) {
			if (objectsBlocks[i].appearTime_ == boost::posix_time::ptime()) {
				objectsBlocks[i].appearTime_ = dateTime;
			} else if (dateTime > objectsBlocks[i].appearTime_ + boost::posix_time::seconds(settings_.maxSupervisionTime_)) {
				objectsBlocks[i].isStanding_ = true;
			}
		} else {
			objectsBlocks[i] = Block();
		}
	}
}

cv::Mat LeftThingsDetector::generateResultMatrix(const std::vector<Block>& objectsBlocks) {
	BEGIN_FUNCTION
	u_int8_t zero = 0;
	cv::Mat resultMask = cv::Mat(cv::Size(settings_.numBlockWidth_, settings_.numBlockHeight_), CV_8U, zero);
	
	for (int y = 0; y < resultMask.size().height; y++) {
		for (int x = 0; x < resultMask.size().width; x++) {
			int index = y * settings_.numBlockWidth_ + x;
			resultMask.ptr(y)[x] = objectsBlocks[index].isStanding_;
		}
	}
	
	return resultMask;
	END_FUNCTION
}

void LeftThingsDetector::clear() {
	backgroundSeparationAlgorithm_->clear();
	prevMode_ = AnCommon::IDLE;
	mode_ = AnCommon::IDLE;
	settings_ = LeftThingsDetectorSettings();
}

void LeftThingsDetector::setActivationTime() {
	activationTime_ =  boost::posix_time::microsec_clock::local_time();
	mode_ = AnCommon::LEARNING;
}
