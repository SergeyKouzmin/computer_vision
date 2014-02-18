#include "SmokeDetectSettings.h"
#include <algorithm>

const int SmokeDetectSettings::ALERT_TIME = 30; 
const int SmokeDetectSettings::NUM_WIDTH_BLOCKS = 2; 
const int SmokeDetectSettings::NUM_HEIGHT_BLOCKS = 2; 
const double SmokeDetectSettings::MIN_SMOKE_AREA = 5.0; 
	
SmokeDetectSettings::SmokeDetectSettings()
	: alertTime_(ALERT_TIME)
	, numWidthBlocks_(NUM_WIDTH_BLOCKS)
	, numHeightBlocks_(NUM_HEIGHT_BLOCKS)
	, minSmokeArea_(MIN_SMOKE_AREA)
{}

SmokeDetectSettings::SmokeDetectSettings(
						int alertTime, 
						int numWidthBlocks, 
						int numHeightBlocks, 
						double minSmokeArea)
	: alertTime_(alertTime)
	, numWidthBlocks_(numWidthBlocks)
	, numHeightBlocks_(numHeightBlocks)
	, minSmokeArea_(minSmokeArea)
{}

int SmokeDetectSettings::getBlockSizeX(int sizeX) {
	return std::max(1, sizeX / BLOCKS_PER_DIMENSION);
}

int SmokeDetectSettings::getBlockSizeY(int sizeY) {
	return std::max(1, sizeY / BLOCKS_PER_DIMENSION);
}
