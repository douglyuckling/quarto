/**
* @file BoardModel.cpp
*/
#include "BoardModel.hpp"

using peek::PI;

namespace quarto {

	const double BoardModel::markerRadius = 1.85;
	const double BoardModel::markerBorderThickness = 0.28;
	const double BoardModel::markerBorderDepthOffset = 0.0417;
	const double BoardModel::boardBorderThickness = 0.35;
	const double BoardModel::boardBorderDepthOffset = 0.0625;
	const double BoardModel::boardRadius = 12.475;
	const double BoardModel::markerSpacing = 0.4;
	const unsigned int BoardModel::markerMatrixSize = 4;
	const unsigned int BoardModel::numMarkerQuarterSegments = 10;
	const unsigned int BoardModel::numMarkerConcentricSegments = 3;
	const unsigned int BoardModel::numMarkerBorderConcentricSegments = 5;
	const unsigned int BoardModel::numBoardBorderConcentricSegments = 5;
	const unsigned int BoardModel::numBoardBorderSegments = 20;
	
	BoardModel::BoardModel() {

		generateModel();
	}

	/**
	 * @param i The i-coordinate of the desired marker
	 * @param j The j-coordinate of the desired marker
	 * @return The location of the specified marker
	 */
	Point3d BoardModel::getMarkerPosition(unsigned int i, unsigned int j) {
		Point3d p;
		double sin45 = sin(PI/4.0);
		double radius = markerRadius + markerBorderThickness;
		i = i % markerMatrixSize;
		j = j % markerMatrixSize;
		double dx = (2.0 * radius + markerSpacing)
			* ((double)i - 0.5 * (double)(markerMatrixSize - 1));
		double dy = (2.0 * radius + markerSpacing)
			* ((double)j - 0.5 * (double)(markerMatrixSize - 1));

		p.x += sin45 * (dx - dy);
		p.y += sin45 * (dx + dy);

		return p;
	}
}
