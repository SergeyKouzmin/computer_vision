#include "LeftThings.h"

LeftThings::LeftThings()
	: isLeftThings_(false)
{}

void LeftThings::setRestTime(const boost::posix_time::ptime &restTime) {
	restTime_ = restTime;
}
