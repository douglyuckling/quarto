/**
 * @file BoardModel.hpp
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

	class BoardModel : public Model {
	public:

		BoardModel();

		static Point3d getMarkerPosition(unsigned int i, unsigned int j);

		static const double markerRadius;
		static const double markerBorderThickness;
		static const double markerBorderDepthOffset;
		static const double markerSpacing;
		static const double boardRadius;
		static const double boardBorderThickness;
		static const double boardBorderDepthOffset;
		static const unsigned int markerMatrixSize;
		static const unsigned int numMarkerQuarterSegments;
		static const unsigned int numMarkerConcentricSegments;
		static const unsigned int numMarkerBorderConcentricSegments;
		static const unsigned int numBoardBorderConcentricSegments;
		static const unsigned int numBoardBorderSegments;

		typedef handle_traits<BoardModel>::handle_type handle;

		typedef list_traits<BoardModel::handle>::list_type list;

	private:

		void generateModel();

	};

}
