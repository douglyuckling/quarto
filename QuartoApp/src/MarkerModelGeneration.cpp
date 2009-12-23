/**
* @file BoardModelGeneration.cpp
*/
#include "MarkerModel.hpp"
#include <Triangle.hpp>
#include <TriangleFan.hpp>
#include <SmoothMesh.hpp>

/* Implementation dependencies */
using peek::Vector3d;
using peek::parametricUnitCircle;
using peek::Primitive;
using peek::Triangle;
using peek::TriangleFan;
using peek::SmoothMesh;

namespace {

	using namespace quarto;

	const double markerRadius = MarkerModel::markerRadius;
	const unsigned int numMarkerQuarterSegments = MarkerModel::numMarkerQuarterSegments;
	
	void generateMarkerMeshes(Model *pModel);

	unsigned int markerCenterIndex;
	void generateMarkerCenterVert(Vertex3d::list &verts);
	inline Vertex3d::listIndex markerCenterVert();

	unsigned int markerIndex;
	void generateMarkerVerts(Vertex3d::list &verts);
	inline Vertex3d::listIndex markerVert(unsigned int i);

	void generateMarkerMeshes(Model *pModel) {

		// Generate vertices...
		Vertex3d::list verts;
		generateMarkerCenterVert(verts);
		generateMarkerVerts(verts);

		// Generate faces...
		
		Primitive::list primitives;
		TriangleFan::handle triangleFan(new TriangleFan());
		triangleFan->addVertex(markerCenterVert());

		for(unsigned int i = 0; i <= (numMarkerQuarterSegments * 4); i++) {
			triangleFan->addVertex(markerVert(i));
		}

		primitives.push_back(triangleFan);

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives)));
	}

	void generateMarkerCenterVert(Vertex3d::list &verts) {
		markerCenterIndex = (unsigned int)verts.size();
		verts.push_back(Point3d());
	}

	/**
	 * @return The index of the marker center vertex
	 */
	inline Vertex3d::listIndex markerCenterVert() {
		return markerCenterIndex;
	}

	void generateMarkerVerts(Vertex3d::list &verts) {
		double r = markerRadius;

		markerIndex = (unsigned int)verts.size();
		Point3d markerCenterPoint;
		for(unsigned int i = 0; i < (numMarkerQuarterSegments * 4); i++) {
			double t = (double)i/(double)(numMarkerQuarterSegments * 4) - 0.125;
			verts.push_back(markerCenterPoint + markerRadius * parametricUnitCircle(t));
		}
	}

	/**
	 * @param i The theta-index of the marker's desired vertex
	 * @return The index of the requested marker vertex
	 */
	inline Vertex3d::listIndex markerVert(unsigned int i) {
		return markerIndex + (i % (numMarkerQuarterSegments * 4));
	}

}

namespace quarto {

	void MarkerModel::generateModel() {
		Model *pModel = this;
		pModel->setOrigin(Point3d(0.0, 0.0, 0.0));

		generateMarkerMeshes(pModel);
	}

}
