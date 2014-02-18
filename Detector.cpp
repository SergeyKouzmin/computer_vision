#include "Detector.h"

Detector::Detector()
	: on_(false)
{
}

void Detector::on() {
	on_ = true;
}

void Detector::off() {
	on_ = false;
}

bool Detector::state() {
	return on_;
}

xml::Request::Params Detector::getSettings() {
	return xml::Request::Params();
}
