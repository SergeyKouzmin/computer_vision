#include "LeftThingsDetectorSettings.h"

const int LeftThingsDetectorSettings::ALERT_TIME = 30;
const int LeftThingsDetectorSettings::MAX_SUPERVISION_TIME = 10;
const int LeftThingsDetectorSettings::NUM_BLOCK_WIDTH = 16;
const int LeftThingsDetectorSettings::NUM_BLOCK_HEIGHT = 16;
const double LeftThingsDetectorSettings::MIN_OBJECT_AREA = 0.5;
const double LeftThingsDetectorSettings::MAX_OBJECT_AREA = 30;
const int LeftThingsDetectorSettings::STARTING_LEARNING_TIME = 10;
const int LeftThingsDetectorSettings::MAX_MERGING_GAP_PERCENT = 3;
const int LeftThingsDetectorSettings::STARTING_LEARNING_PERCENT = 0;
const bool LeftThingsDetectorSettings::USE_MORPHOLOGY = false;
const bool LeftThingsDetectorSettings::USE_CONNECTED_COMP = true;



LeftThingsDetectorSettings::LeftThingsDetectorSettings(int alertTime
															,int maxSupervisionTime
															, double minObjectArea
															, double maxObjectArea
															, int numBlockWidth
															, int numBlockHeight
															, int startingLearningTime
															, int maxMergingGapPercent
															, int startingLearningPercent
															, bool useMorphology
															, bool useConnectedComp)
: alertTime_(alertTime)
, maxSupervisionTime_(maxSupervisionTime)
, numBlockWidth_(numBlockWidth)
, numBlockHeight_(numBlockHeight)
, minObjectArea_(minObjectArea)
, maxObjectArea_(maxObjectArea)
, startingLearningTime_(startingLearningTime)
, maxMergingGapPercent_(maxMergingGapPercent)
, startingLearningPercent_(startingLearningPercent)
, useMorphology_(useMorphology)
, useConnectedComp_(useConnectedComp)
{}

LeftThingsDetectorSettings::LeftThingsDetectorSettings()
	: alertTime_(ALERT_TIME)
	, maxSupervisionTime_(MAX_SUPERVISION_TIME)
	, numBlockWidth_(NUM_BLOCK_WIDTH)
	, numBlockHeight_(NUM_BLOCK_HEIGHT)
	, minObjectArea_(MIN_OBJECT_AREA)
	, maxObjectArea_(MAX_OBJECT_AREA)
	, startingLearningTime_(STARTING_LEARNING_TIME)
	, maxMergingGapPercent_(MAX_MERGING_GAP_PERCENT)
	, startingLearningPercent_(STARTING_LEARNING_PERCENT)
	, useMorphology_(USE_MORPHOLOGY)
	, useConnectedComp_(USE_CONNECTED_COMP)
{}

LeftThingsDetectorSettings::LeftThingsDetectorSettings(const LeftThingsDetectorSettings &settings) 
	: alertTime_(settings.alertTime_)
	, maxSupervisionTime_(settings.maxSupervisionTime_)
	, numBlockWidth_(settings.numBlockWidth_)
	, numBlockHeight_(settings.numBlockHeight_)
	, minObjectArea_(settings.minObjectArea_)
	, maxObjectArea_(settings.maxObjectArea_)
	, startingLearningTime_(settings.startingLearningTime_)
	, maxMergingGapPercent_(settings.maxMergingGapPercent_)
	, startingLearningPercent_(settings.startingLearningPercent_)
	, useMorphology_(settings.useMorphology_)
	, useConnectedComp_(settings.useConnectedComp_)
{}
