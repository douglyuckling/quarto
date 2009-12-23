/**
* @file PieceModel.cpp
*/
#include "PieceModel.hpp"

namespace quarto {

	// Common parameters
	const double PieceModel::radius = 1.2;
	const double PieceModel::edgeRadius = 0.25;
	const double PieceModel::holeRadius = 0.8;
	const double PieceModel::shortHeight = 3.9;
	const double PieceModel::tallHeight = 5.9;
	const double PieceModel::beltHeight = 2.575;
	const double PieceModel::beltRadius = 0.225;
	const double PieceModel::holeDepth = 1.1;
	const double PieceModel::holeCenterDepth = 1.5;
	const unsigned int PieceModel::numShoulderSegments = 10;
	const unsigned int PieceModel::numBeltSegments = 8;

	// Parameters for round pieces
	const unsigned int PieceModel::numCylinderSegments = 32;

	// Parameters for square pieces
	const unsigned int PieceModel::numCorners = 4;
	const unsigned int PieceModel::numCornerSegments = 10;
	const unsigned int PieceModel::numQuarterHoleSegments = 10;
	
	PieceModel::PieceModel(bool isRound, bool isTall, bool isHollow, bool isWhite) {
		this->round = isRound;
		this->tall = isTall;
		this->hollow = isHollow;
		this->white = isWhite;

		if(isRound) {
			generateRoundModel();
		} else {
			generateSquareModel();
		}
	}

}
