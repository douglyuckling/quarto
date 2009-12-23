/**
 * @file PieceModel.hpp
 */
#pragma once

#include <handle_traits.hpp>
#include <list_traits.hpp>
#include <Model.hpp>

// Implementation dependencies
using peek::handle_traits;
using peek::list_traits;
using peek::Point3d;
using peek::Vertex3d;
using peek::Model;

namespace quarto {

	class PieceModel : public Model {
	public:

		PieceModel(bool isRound, bool isTall, bool isHollow, bool isWhite);

		// Common parameters
		static const double radius;
		static const double edgeRadius;
		static const double holeRadius;
		static const double shortHeight;
		static const double tallHeight;
		static const double beltHeight;
		static const double beltRadius;
		static const double holeDepth;
		static const double holeCenterDepth;
		static const unsigned int numShoulderSegments;
		static const unsigned int numBeltSegments;

		// Parameters for round pieces
		static const unsigned int numCylinderSegments;

		// Parameters for square pieces
		static const unsigned int numCorners;
		static const unsigned int numCornerSegments;
		static const unsigned int numQuarterHoleSegments;

		inline bool isRound() const { return round; }
		inline bool isTall() const { return tall; }
		inline bool isHollow() const { return hollow; }
		inline bool isWhite() const { return white; }

		typedef handle_traits<PieceModel>::handle_type handle;

		typedef list_traits<PieceModel::handle>::list_type list;

	private:
		bool round;
		bool tall;
		bool hollow;
		bool white;

		void generateRoundModel();
		void generateSquareModel();
	};

}
