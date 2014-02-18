#include "MorphologyFilter.h"

const int MorphologyFilter::MorphologyFilterSettings::CLOSE_ITR = 1;

MorphologyFilter::MorphologyFilterSettings::MorphologyFilterSettings()
	: closeItr_(CLOSE_ITR)
{}

MorphologyFilter::MorphologyFilterSettings::MorphologyFilterSettings(int closeItr)
	: closeItr_(closeItr)
{}

const std::string MorphologyFilter::MORPHOLOGY_FILTER_TYPE = "MORPHOLOGY_FILTER";

std::string MorphologyFilter::getType() {
	return MORPHOLOGY_FILTER_TYPE; 
}

cv::Mat MorphologyFilter::operator()(cv::Mat& img) {
	if (!img.isContinuous()) {
		errors::throwException(errors::ERR_08_IMAGE_MUST_BE_CONTINUOUS);
	}
	
	cv::Mat buf(img.size(), img.type());
	buf.setTo(0);
	cv::morphologyEx(img, buf, CV_MOP_OPEN, cv::Mat(), cv::Point(-1, -1), settings_.closeItr_ );
	cv::morphologyEx(img, buf, CV_MOP_CLOSE, cv::Mat(), cv::Point(-1, -1), settings_.closeItr_ );
	
	return buf;
}

void MorphologyFilter::setSettings(const MorphologyFilter::MorphologyFilterSettings &settings) {
	settings_ = settings;
}
	
MorphologyFilter::MorphologyFilterSettings MorphologyFilter::getSettings() {
	return settings_;
}

void MorphologyFilter::setSettings(const xml::Request::Params &settings, AnCommon::StrSet &usedSettings) {
	// если будут настройки зависящие от клиента, то их нужно будет тут описать
}
	
void MorphologyFilter::getSettings(xml::Request::Params &settings) {
	// если будут настройки зависящие от клиента, то их нужно будет тут описать
}

MorphologyFilter::MorphologyFilter()
{}

MorphologyFilter::MorphologyFilter(const MorphologyFilter::MorphologyFilterSettings &settings)
	: settings_(settings)
{}