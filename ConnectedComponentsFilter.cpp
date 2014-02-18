#include "ConnectedComponentsFilter.h"
#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <opencv/cxcore.h>

const int ConnectedComponentsFilter::ConnectedComponentsFilterSettings::COUNTOUR_APPROX_LEVEL = 2;
const ConnectedComponentsFilter::ContourRefinementMethod ConnectedComponentsFilter::ConnectedComponentsFilterSettings::METHOD = ConnectedComponentsFilter::RM_APPROX;
const float ConnectedComponentsFilter::ConnectedComponentsFilterSettings::PERIM_SCALE = 20.0;
	
ConnectedComponentsFilter::ConnectedComponentsFilterSettings::ConnectedComponentsFilterSettings()
	: contourApproxLevel_(COUNTOUR_APPROX_LEVEL)
	, method_(METHOD)
	, perimScale_(PERIM_SCALE)
{}

ConnectedComponentsFilter::ConnectedComponentsFilterSettings::ConnectedComponentsFilterSettings(int contourApproxLevel, 
																								ConnectedComponentsFilter::ContourRefinementMethod method, 
																								float perimScale)
	: contourApproxLevel_(contourApproxLevel)
	, method_(method)
	, perimScale_(perimScale)
{}

const std::string ConnectedComponentsFilter::CONNECTED_COMPONENTS_FILTER_TYPE = "CONNECTED_COMPONENTS_FILTER"; 

ConnectedComponentsFilter::ConnectedComponentsFilter() 
{}

ConnectedComponentsFilter::ConnectedComponentsFilter(const ConnectedComponentsFilterSettings& settings, MorphologyFilter::MorphologyFilterSettings morphSettings) 
	: morphology_(morphSettings) 
	, settings_(settings)
{}
	
cv::Mat ConnectedComponentsFilter::operator()(cv::Mat& img) {
	
	if (!img.isContinuous()) {
		errors::throwException(errors::ERR_08_IMAGE_MUST_BE_CONTINUOUS);
	}
	
	morphology_(img);

	maskContours_.clear();
	
	cv::findContours(img, maskContours_, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	size_t i = 0;
	
	while (i < maskContours_.size()) {
		AnCommon::Contour& contour = maskContours_[i];
		
		cv::Mat contourMat(contour, false);
		double len = cv::arcLength(contourMat, true);
		if (len * settings_.perimScale_ < img.size().height + img.size().width) {
			// Отбрасываем контуры со слишком маленьким периметром.
			maskContours_.erase(maskContours_.begin() + i);
		} else {
			// Достаточно большие контуры аппроксимируем указанным методом.
			AnCommon::Contour newContour;
			if (settings_.method_ == RM_APPROX) {
				cv::approxPolyDP(contourMat, newContour, settings_.contourApproxLevel_, true);
			} else if (settings_.method_ == RM_CONVEX_HULL){
				cv::convexHull(contourMat, newContour, true);
			} else {
				errors::throwException(errors::ERR_12_INVALID_COUNTOUR_REFINEMENT_METHOD);
			}
			maskContours_[i] = newContour;
				
			i++;
		}
	}
	
	// Отрисовать найденные области обратно в маску
	cv::Mat result(img.size(), img.type());
	result.setTo(0);
	if (!maskContours_.empty()) { // Обходим баг OpenCV 2.1.0; в 2.3.1 он уже исправлен.
		cv::drawContours(result, maskContours_, -1, CV_CVX_WHITE, CV_FILLED);
	}
	return result;
}

void ConnectedComponentsFilter::setSettings(const xml::Request::Params &settings, AnCommon::StrSet &usedSettings) {
	// если будут настройки зависящие от клиента, то их нужно будет тут описать
}
	
void ConnectedComponentsFilter::getSettings(xml::Request::Params &settings) {
	// если будут настройки зависящие от клиента, то их нужно будет тут описать
}

std::string ConnectedComponentsFilter::getType() {
	return CONNECTED_COMPONENTS_FILTER_TYPE;
}