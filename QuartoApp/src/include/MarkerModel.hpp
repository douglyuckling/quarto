/**
 * @file MarkerModel.hpp
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

	class MarkerModel : public Model {
	public:

		MarkerModel();

		static const double markerRadius;
		static const unsigned int numMarkerQuarterSegments;

		typedef handle_traits<MarkerModel>::handle_type handle;

		typedef list_traits<MarkerModel::handle>::list_type list;

	private:

		void generateModel();

	};

}
