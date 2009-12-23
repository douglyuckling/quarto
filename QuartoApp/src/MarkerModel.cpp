/**
* @file MarkerModel.cpp
*/
#include "MarkerModel.hpp"

namespace quarto {

	const double MarkerModel::markerRadius = 1.85;
	const unsigned int MarkerModel::numMarkerQuarterSegments = 10;

	MarkerModel::MarkerModel() {

		generateModel();
	}

}
