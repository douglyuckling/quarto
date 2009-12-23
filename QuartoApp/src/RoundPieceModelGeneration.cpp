/**
* @file RoundPieceModelGeneration.cpp
*/
#include "PieceModel.hpp"
#include "Materials.hpp"
#include <Triangle.hpp>
#include <TriangleFan.hpp>
#include <TriangleStrip.hpp>
#include <SmoothMesh.hpp>

/* Implementation dependencies */
using peek::Vector3d;
using peek::parametricUnitCircle;
using peek::Primitive;
using peek::Triangle;
using peek::TriangleFan;
using peek::TriangleStrip;
using peek::SmoothMesh;

namespace {

	using namespace quarto;

	const double radius = PieceModel::radius;
	const double edgeRadius = PieceModel::edgeRadius;
	const double holeRadius = PieceModel::holeRadius;
	const double shortHeight = PieceModel::shortHeight;
	const double tallHeight = PieceModel::tallHeight;
	const double beltHeight = PieceModel::beltHeight;
	const double beltRadius = PieceModel::beltRadius;
	const double holeDepth = PieceModel::holeDepth;
	const double holeCenterDepth = PieceModel::holeCenterDepth;
	const unsigned int numCylinderSegments = PieceModel::numCylinderSegments;
	const unsigned int numShoulderSegments = PieceModel::numShoulderSegments;
	const unsigned int numBeltSegments = PieceModel::numBeltSegments;

	Material material;
	double height;
	bool notSolid;
	
	void generateBottomMesh(Model *pModel);
	void generateTopMesh(Model *pModel);
	void generateBeltMesh(Model *pModel);
	void generateLegMesh(Model *pModel);
	void generateHoleMeshes(Model *pModel);

	Vertex3d::listIndex bottomCenterIndex;
	void generateBottomCenterVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex bottomCenter();
	Vertex3d::listIndex bottomEdgeIndex;
	void generateBottomEdgeVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex bottomEdge(unsigned int i);

	Vertex3d::listIndex topCenterIndex;
	void generateTopCenterVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex topCenter();
	Vertex3d::listIndex topCircleIndex;
	void generateTopCircleVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex topCircle(unsigned int i);
	Vertex3d::listIndex topShoulderIndex;
	void generateShoulderVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex topShoulder(unsigned int i, unsigned int j);

	Vertex3d::listIndex beltIndex;
	void generateBeltVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex belt(unsigned int i, unsigned int j);
	Vertex3d::listIndex beltTopIndex;
	void generateBeltTopVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex beltTop(unsigned int i);
	Vertex3d::listIndex beltBottomIndex;
	void generateBeltBottomVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex beltBottom(unsigned int i);

	Vertex3d::listIndex holeTopIndex;
	void generateHoleTopVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex holeTop(unsigned int i);
	Vertex3d::listIndex holeBottomIndex;
	void generateHoleBottomVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex holeBottom(unsigned int i);
	Vertex3d::listIndex holeBottomCenterIndex;
	void generateHoleBottomCenterVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex holeBottomCenter();

	void generateBottomMesh(Model *pModel) {

		// Generate vertices...
		Vertex3d::list verts;
		generateBottomCenterVerts(verts, pModel->getOrigin());
		generateBottomEdgeVerts(verts, pModel->getOrigin());

		// Generate faces...
		
		Primitive::list primitives;
		TriangleFan::handle triangleFan(new TriangleFan());
		triangleFan->addVertex(bottomCenter());

		for(unsigned int i = 0; i <= numCylinderSegments; i++) {
			triangleFan->addVertex(bottomEdge(numCylinderSegments-i));
		}

		primitives.push_back(triangleFan);

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, material)));
	}

	void generateTopMesh(Model *pModel) {

		// Generate vertices...
		
		Vertex3d::list verts;

		if(notSolid) {
			generateHoleTopVerts(verts, pModel->getOrigin());
		}
		else {
			generateTopCenterVerts(verts, pModel->getOrigin());
		}
		generateTopCircleVerts(verts, pModel->getOrigin());
		generateShoulderVerts(verts, pModel->getOrigin());
		generateBeltTopVerts(verts, pModel->getOrigin());

		// Generate faces...
		
		Primitive::list primitives;

		TriangleStrip::handle triangleStrip;
		TriangleFan::handle triangleFan;

		if(notSolid) {
			// Top circle with a hole in the center...
			triangleStrip = TriangleStrip::handle(new TriangleStrip());

			for(unsigned int i = 0; i <= numCylinderSegments; i++) {
				triangleStrip->addVertex(holeTop(i));
				triangleStrip->addVertex(topCircle(i));
			}

			primitives.push_back(triangleStrip);
		}
		else {
			// Top circle with no hole in the center...
			triangleFan = TriangleFan::handle(new TriangleFan());
			triangleFan->addVertex(topCenter());
			
			for(unsigned int i = 0; i <= numCylinderSegments; i++) {
				triangleFan->addVertex(topCircle(i));
			}

			primitives.push_back(triangleFan);
		}

		// Shoulders...
		for(unsigned int j = 0; j < numShoulderSegments-1; j++) {
			triangleStrip = TriangleStrip::handle(new TriangleStrip());

			for(unsigned int i = 0; i <= numCylinderSegments; i++) {
				triangleStrip->addVertex(topShoulder(i, j));
				triangleStrip->addVertex(topShoulder(i, j+1));
			}

			primitives.push_back(triangleStrip);
		}

		// Torso...
		triangleStrip = TriangleStrip::handle(new TriangleStrip());

		for(unsigned int i = 0; i <= numCylinderSegments; i++) {
			triangleStrip->addVertex(topShoulder(i, numShoulderSegments-1));
			triangleStrip->addVertex(beltTop(i));
		}

		primitives.push_back(triangleStrip);

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, material)));
	}

	void generateBeltMesh(Model *pModel) {
		
		// Generate vertices...

		Vertex3d::list verts;
		generateBeltVerts(verts, pModel->getOrigin());

		// Generate faces...

		Primitive::list primitives;

		for(unsigned int j = 0; j < numBeltSegments;  j++) {
			TriangleStrip::handle triangleStrip(new TriangleStrip());
			
			for(unsigned int i = 0; i <= numCylinderSegments;  i++) {
				triangleStrip->addVertex(belt(i, j));
				triangleStrip->addVertex(belt(i, j+1));
			}
			
			primitives.push_back(triangleStrip);
		}


		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, material)));

	}

	void generateLegMesh(Model *pModel) {

		// Generate vertices...
		Vertex3d::list verts;
		generateBeltBottomVerts(verts, pModel->getOrigin());
		generateBottomEdgeVerts(verts, pModel->getOrigin());

		// Generate faces...
		Primitive::list primitives;
		TriangleStrip::handle triangleStrip(new TriangleStrip());

		for(unsigned int i = 0; i <= numCylinderSegments;  i++) {
			triangleStrip->addVertex(beltBottom(i));
			triangleStrip->addVertex(bottomEdge(i));
		}

		primitives.push_back(triangleStrip);

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, material)));

	}

	void generateHoleMeshes(Model *pModel) {
		if(!notSolid) {
			return;
		}

		Vertex3d::list verts;
		Primitive::list primitives;

		// Hole sides...

		verts.clear();
		generateHoleTopVerts(verts, pModel->getOrigin());
		generateHoleBottomVerts(verts, pModel->getOrigin());

		primitives.clear();
		TriangleStrip::handle triangleStrip(new TriangleStrip());

		for(unsigned int i = 0; i <= numCylinderSegments;  i++) {
			triangleStrip->addVertex(holeBottom(i));
			triangleStrip->addVertex(holeTop(i));
		}

		primitives.push_back(triangleStrip);
		
		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, material)));

		// Hole bottom...

		verts.clear();
		generateHoleBottomCenterVerts(verts, pModel->getOrigin());
		generateHoleBottomVerts(verts, pModel->getOrigin());

		primitives.clear();
		TriangleFan::handle triangleFan(new TriangleFan());
		triangleFan->addVertex(holeBottomCenter());

		for(unsigned int i = 0; i <= numCylinderSegments;  i++) {
			triangleFan->addVertex(holeBottom(i));
		}

		primitives.push_back(triangleFan);

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, material)));

	}

	void generateBottomCenterVerts(Vertex3d::list &verts, Point3d origin) {
		Point3d bottomCenterPoint = origin;

		bottomCenterIndex = (Vertex3d::listIndex)verts.size();
		verts.push_back(bottomCenterPoint);
	}

	/**
	 * @return The index of the bottom center vertex
	 */
	inline Vertex3d::listIndex bottomCenter() {
		return bottomCenterIndex;
	}

	void generateBottomEdgeVerts(Vertex3d::list &verts, Point3d origin) {
		Point3d bottomCenterPoint = origin;

		bottomEdgeIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int i = 0; i < numCylinderSegments; i++) {
			double t = (double)i/(double)numCylinderSegments;
			verts.push_back(bottomCenterPoint + radius * parametricUnitCircle(t));
		}
	}

	/**
 	 * @param i An index of a cylinder segment break (0 through numCylinderSegments-1)
	 * @return The index of the requested bottom edge vertex
	 */
	inline Vertex3d::listIndex bottomEdge(unsigned int i) {
		return bottomEdgeIndex + (i % numCylinderSegments);
	}

	void generateTopCenterVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d topCenterOffset = Vector3d(0, 0, height);
		Point3d topCenterPoint = origin + topCenterOffset;

		topCenterIndex = (Vertex3d::listIndex)verts.size();
		verts.push_back(topCenterPoint);
	}

	/**
	 * @return The index of the top center vertex
	 */
	inline Vertex3d::listIndex topCenter() {
		return bottomCenterIndex;
	}

	void generateTopCircleVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d topCenterOffset = Vector3d(0, 0, height);
		Point3d topCenterPoint = origin + topCenterOffset;
		double innerRadius = radius - edgeRadius;

		topCircleIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int i = 0; i < numCylinderSegments; i++) {
			double t = (double)i/(double)numCylinderSegments;
			verts.push_back(topCenterPoint + innerRadius * parametricUnitCircle(t));
		}
	}

	/**
 	 * @param i An index of a cylinder segment break (0 through numCylinderSegments)
	 * @return The index of the requested top circle vertex
	 */
	inline Vertex3d::listIndex topCircle(unsigned int i) {
		return topCircleIndex + (i % numCylinderSegments);
	}
	
	void generateShoulderVerts(Vertex3d::list &verts, Point3d origin) {
		Point3d topCenterPoint = origin + Vector3d(0, 0, height);
		double innerRadius = radius - edgeRadius;

		topShoulderIndex = (Vertex3d::listIndex)verts.size();
 		for(unsigned int i = 0; i < numCylinderSegments; i++) {
			double tI = (double)i/(double)numCylinderSegments;
			for(unsigned int j = 0; j <= numShoulderSegments; j++) {
				double tJ = (double)j / (double)(numShoulderSegments * 4);
				Vector3d quarterUnitCircle = parametricUnitCircle(tJ);
				Vector3d shoulderVector = parametricUnitCircle(tI);
				double curRadius = radius - ((1.0 - quarterUnitCircle.y) * edgeRadius);
				shoulderVector.x *= curRadius;
				shoulderVector.y *= curRadius;
				shoulderVector.z -= edgeRadius * (1.0 - quarterUnitCircle.x);
				verts.push_back(topCenterPoint + shoulderVector);
			}
		}
	}

	/**
	 * @param i An index of a cylinder segment break (0 through numCylinderSegments-1)
	 * @param j An index of a shoulder segment (0 through numShoulderSegments)
	 * @return The index of the requested shoulder vertex
	 */
	inline Vertex3d::listIndex topShoulder(unsigned int i, unsigned int j) {
		return topShoulderIndex
			+ (i % numCylinderSegments) * (numShoulderSegments+1)
			+ (j % (numShoulderSegments+1));
	}

	void generateBeltVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d beltCenterOffset = Vector3d(0, 0, beltHeight);
		Point3d beltCenterPoint = origin + beltCenterOffset;

		beltIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int i = 0; i < numCylinderSegments; i++) {
			double t = (double)i/(double)numCylinderSegments;
			for(unsigned int j = 0; j <= numBeltSegments; j++) {
				double s = (double)j/(2.0*(double)numBeltSegments);
				Vector3d v1 = beltRadius * parametricUnitCircle(s);
				Vector3d v2 = (radius - v1.y) * parametricUnitCircle(t);
				v2.z = v1.x;
				verts.push_back(beltCenterPoint + v2);
			}
		}
	}

	/**
	 * @param i An index of a cylinder segment break (0 through numCylinderSegments-1)
	 * @param j An index of a belt segment break (0 through numBeltSegments)
	 * @return The index of the requested belt vertex
	 */
	inline Vertex3d::listIndex belt(unsigned int i, unsigned int j) {
		return beltIndex
			+ (i % numCylinderSegments) * (numBeltSegments + 1)
			+ (j % (numBeltSegments + 1));
	}

	void generateBeltTopVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d beltTopCenterOffset = Vector3d(0, 0, beltHeight + beltRadius);
		Point3d beltTopCenterPoint = origin + beltTopCenterOffset;

		beltTopIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int i = 0; i < numCylinderSegments; i++) {
			double t = (double)i/(double)numCylinderSegments;
			verts.push_back(beltTopCenterPoint + radius * parametricUnitCircle(t));
		}
	}

	/**
	 * @param i An index of a cylinder segment break (0 through numCylinderSegments-1)
	 * @return The index of the requested outer belt top vertex
	 */
	inline Vertex3d::listIndex beltTop(unsigned int i) {
		return beltTopIndex + (i % numCylinderSegments);
	}

	void generateBeltBottomVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d beltBottomCenterOffset = Vector3d(0, 0, beltHeight - beltRadius);
		Point3d beltBottomCenterPoint = origin + beltBottomCenterOffset;

		beltBottomIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int i = 0; i < numCylinderSegments; i++) {
			double t = (double)i/(double)numCylinderSegments;
			verts.push_back(beltBottomCenterPoint + radius * parametricUnitCircle(t));
		}
	}

	/**
	 * @param i An index of a cylinder segment break (0 through numCylinderSegments-1)
	 * @return The index of the requested outer belt bottom vertex
	 */
	inline Vertex3d::listIndex beltBottom(unsigned int i) {
		return beltBottomIndex + (i % numCylinderSegments);
	}

	void generateHoleTopVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d topCenterOffset = Vector3d(0, 0, height);
		Point3d topCenterPoint = origin + topCenterOffset;

		holeTopIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int i = 0; i < numCylinderSegments; i++) {
			double t = (double)i/(double)numCylinderSegments;
			verts.push_back(topCenterPoint + holeRadius * parametricUnitCircle(t));
		}
	}

	/**
	 * @param i An index of a cylinder segment break (0 through numCylinderSegments-1)
	 * @return The index of the requested hole-top vertex
	 */
	inline Vertex3d::listIndex holeTop(unsigned int i) {
		return holeTopIndex + (i % numCylinderSegments);
	}

	void generateHoleBottomVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d holeBottomCenterOffset = Vector3d(0, 0, height - holeDepth);
		Point3d holeBottomCenterPoint = origin + holeBottomCenterOffset;

		holeBottomIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int i = 0; i < numCylinderSegments; i++) {
			double t = (double)i/(double)numCylinderSegments;
			verts.push_back(holeBottomCenterPoint + holeRadius * parametricUnitCircle(t));
		}
	}

	/**
	 * @param i An index of a cylinder segment break (0 through numCylinderSegments-1)
	 * @return The index of the requested hole-bottom vertex
	 */
	inline Vertex3d::listIndex holeBottom(unsigned int i) {
		return holeBottomIndex + (i % numCylinderSegments);
	}

	void generateHoleBottomCenterVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d holeBottomCenterOffset = Vector3d(0, 0, height - holeCenterDepth);

		holeBottomCenterIndex = (Vertex3d::listIndex)verts.size();
		verts.push_back(origin + holeBottomCenterOffset);
	}

	/**
	 * @return The index of the hole bottom center vertex
	 */
	inline Vertex3d::listIndex holeBottomCenter() {
		return holeBottomCenterIndex;
	}

}

namespace quarto {

	void PieceModel::generateRoundModel() {
		Model *pModel = this;
		pModel->setOrigin(Point3d(0.0, 0.0, 0.0));

		material = (this->white ? whitePieceMaterial : blackPieceMaterial);
		height = (this->tall ? tallHeight : shortHeight);
		notSolid = this->hollow;

		generateBottomMesh(pModel);
		generateTopMesh(pModel);
		generateBeltMesh(pModel);
		generateLegMesh(pModel);
		generateHoleMeshes(pModel);
	}

}
