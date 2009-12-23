/**
* @file SquarePieceModelGeneration.cpp
*/
#include "PieceModel.hpp"
#include "Materials.hpp"
#include <Triangle.hpp>
#include <Quadrilateral.hpp>
#include <TriangleFan.hpp>
#include <TriangleStrip.hpp>
#include <SmoothMesh.hpp>

/* Implementation dependencies */
using peek::Vertex3d;
using peek::Vector3d;
using peek::Point3d;
using peek::parametricUnitCircle;
using peek::Primitive;
using peek::Triangle;
using peek::Quadrilateral;
using peek::TriangleFan;
using peek::TriangleStrip;
using peek::SmoothMesh;

namespace {

	using namespace quarto;

	const unsigned int numCorners = PieceModel::numCorners;
	const double radius = PieceModel::radius;
	const double edgeRadius = PieceModel::edgeRadius;
	const double holeRadius = PieceModel::holeRadius;
	const double shortHeight = PieceModel::shortHeight;
	const double tallHeight = PieceModel::tallHeight;
	const double beltHeight = PieceModel::beltHeight;
	const double beltRadius = PieceModel::beltRadius;
	const double holeDepth = PieceModel::holeDepth;
	const double holeCenterDepth = PieceModel::holeCenterDepth;
	const unsigned int numCornerSegments = PieceModel::numCornerSegments;
	const unsigned int numShoulderSegments = PieceModel::numShoulderSegments;
	const unsigned int numQuarterHoleSegments = PieceModel::numQuarterHoleSegments;
	const unsigned int numBeltSegments = PieceModel::numBeltSegments;

	Material material;
	double height;
	bool notSolid;
	
	void generateBottomMesh(Model *pModel);
	void generateTopMesh(Model *pModel);
	void generateBeltMesh(Model *pModel);
	void generateLegMesh(Model *pModel);
	void generateHoleMeshes(Model *pModel);

	Point3d::list generateCornerPoints(Point3d center, double radius);

	Vertex3d::listIndex bottomCenterIndex;
	void generateBottomCenterVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex bottomCenter();
	Vertex3d::listIndex bottomSquareCornerIndex;
	void generateBottomSquareCornerVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex bottomSquareCorner(unsigned int c);
	Vertex3d::listIndex bottomEdgeCornerIndex;
	void generateBottomEdgeCornerVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex bottomEdgeCorner(unsigned int c, unsigned int i);

	Vertex3d::listIndex topCenterIndex;
	void generateTopCenterVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex topCenter();
	Vertex3d::listIndex topSquareCornerIndex;
	void generateTopSquareCornerVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex topSquareCorner(unsigned int c);
	Vertex3d::listIndex topShoulderCornerIndex;
	void generateShoulderCornerVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex topShoulderCorner(unsigned int c, unsigned int i, unsigned int j);

	Vertex3d::listIndex beltIndex;
	void generateBeltVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex belt(unsigned int c, unsigned int i, unsigned int j);
	Vertex3d::listIndex beltTopIndex;
	void generateBeltTopVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex beltTop(unsigned int c, unsigned int i);
	Vertex3d::listIndex beltBottomIndex;
	void generateBeltBottomVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex beltBottom(unsigned int c, unsigned int i);

	Vertex3d::listIndex holeTopIndex;
	void generateHoleTopVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex holeTopQuarter(unsigned int c, unsigned int i);
	Vertex3d::listIndex holeBottomIndex;
	void generateHoleBottomVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex holeBottomQuarter(unsigned int c, unsigned int i);
	Vertex3d::listIndex holeBottomCenterIndex;
	void generateHoleBottomCenterVerts(Vertex3d::list &verts, Point3d origin);
	inline Vertex3d::listIndex holeBottomCenter();

	void generateBottomMesh(Model *pModel) {

		// Generate vertices...
		Vertex3d::list verts;
		generateBottomCenterVerts(verts, pModel->getOrigin());
		generateBottomSquareCornerVerts(verts, pModel->getOrigin());
		generateBottomEdgeCornerVerts(verts, pModel->getOrigin());

		// Generate faces...
		
		Primitive::list primitives;
		TriangleFan::handle triangleFan;
		
		// Bottom square...
		triangleFan = TriangleFan::handle(new TriangleFan());
		triangleFan->addVertex(bottomCenter());

		for(unsigned int c = 0; c <= numCorners; c++) {
			triangleFan->addVertex(bottomSquareCorner(numCorners-c));
		}

		primitives.push_back(triangleFan);

		// Edges...
		for(unsigned int c = 0; c < numCorners; c++) {
			// Edges (rectangles between the edge corners)...
			Vertex3d::listIndex v1 = bottomEdgeCorner(c, numCornerSegments);
			Vertex3d::listIndex v2 = bottomEdgeCorner(c+1, 0);
			Vertex3d::listIndex v3 = bottomSquareCorner(c+1);
			Vertex3d::listIndex v4 = bottomSquareCorner(c);
			primitives.push_back(Quadrilateral::handle(new Quadrilateral(v4, v3, v2, v1)));

			// Edge corners...
			triangleFan = TriangleFan::handle(new TriangleFan());
			triangleFan->addVertex(bottomSquareCorner(c));
			for(unsigned int i = 0; i <= numCornerSegments; i++) {
				triangleFan->addVertex(bottomEdgeCorner(c, numCornerSegments-i));
			}

			primitives.push_back(triangleFan);
		}

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
		generateTopSquareCornerVerts(verts, pModel->getOrigin());
		generateShoulderCornerVerts(verts, pModel->getOrigin());
		generateBeltTopVerts(verts, pModel->getOrigin());

		// Generate faces...
		
		Primitive::list primitives;

		if(notSolid) {
			// Top square with a hole in the center...
			for(unsigned int c = 0; c < numCorners; c++) {
				Vertex3d::listIndex v1 = topSquareCorner(c);
				Vertex3d::listIndex v2 = topSquareCorner(c+1);
				Vertex3d::listIndex v3 = holeTopQuarter(c+1, 0);
				primitives.push_back(Triangle::handle(new Triangle(v1, v2, v3)));

				TriangleFan::handle triangleFan(new TriangleFan());
				triangleFan->addVertex(topSquareCorner(c));

				for(unsigned int i = 0; i <= numQuarterHoleSegments; i++) {
					triangleFan->addVertex(holeTopQuarter(c, numQuarterHoleSegments-i));
				}

				primitives.push_back(triangleFan);
			}
		}
		else {
			// Top square with no hole in the center..
			TriangleFan::handle triangleFan(new TriangleFan());
			triangleFan->addVertex(topCenter());

			for(unsigned int c = 0; c <= numCorners; c++) {
				triangleFan->addVertex(topSquareCorner(c));
			}

			primitives.push_back(triangleFan);
		}

		// Shoulder corners...
		for(unsigned int c = 0; c < numCorners; c++) {
			TriangleFan::handle triangleFan(new TriangleFan());
			triangleFan->addVertex(topSquareCorner(c));
			for(unsigned int i = 0; i <= numCornerSegments; i++) {
				triangleFan->addVertex(topShoulderCorner(c, i, 0));
			}

			primitives.push_back(triangleFan);

			for(unsigned int j = 0; j < numShoulderSegments-1; j++) {
				TriangleStrip::handle triangleStrip(new TriangleStrip());

				for(unsigned int i = 0; i <= numCornerSegments; i++) {
					triangleStrip->addVertex(topShoulderCorner(c, i, j));
					triangleStrip->addVertex(topShoulderCorner(c, i, j+1));
				}
				
				primitives.push_back(triangleStrip);
			}

		}

		// Shoulder edges...
		for(unsigned int c = 0; c < numCorners; c++) {
			TriangleStrip::handle triangleStrip(new TriangleStrip());
			triangleStrip->addVertex(topSquareCorner(c+1));
			triangleStrip->addVertex(topSquareCorner(c));

			for(unsigned int j = 0; j < numShoulderSegments; j++) {
				triangleStrip->addVertex(topShoulderCorner(c+1, 0, j));
				triangleStrip->addVertex(topShoulderCorner(c, numCornerSegments, j));
			}

			primitives.push_back(triangleStrip);
		}

		// Torso...
		TriangleStrip::handle triangleStrip(new TriangleStrip());

		for(unsigned int c = 0; c < numCorners; c++) {
			for(unsigned int i = 0; i < numCornerSegments; i++) {
				triangleStrip->addVertex(topShoulderCorner(c, i, numShoulderSegments-1));
				triangleStrip->addVertex(beltTop(c, i));
			}

		}

		// Last side...
		triangleStrip->addVertex(topShoulderCorner(0, 0, numShoulderSegments-1));
		triangleStrip->addVertex(beltTop(0, 0));
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

			for(unsigned int c = 0; c < numCorners; c++) {
				for(unsigned int i = 0; i <= numCornerSegments;  i++) {
					triangleStrip->addVertex(belt(c, i, j));
					triangleStrip->addVertex(belt(c, i, j+1));
				}
			}

			triangleStrip->addVertex(belt(0, 0, j));
			triangleStrip->addVertex(belt(0, 0, j+1));

			primitives.push_back(triangleStrip);
		}

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, material)));

	}

	void generateLegMesh(Model *pModel) {

		// Generate vertices...
		Vertex3d::list verts;
		generateBeltBottomVerts(verts, pModel->getOrigin());
		generateBottomEdgeCornerVerts(verts, pModel->getOrigin());

		// Generate faces...
		Primitive::list primitives;
		TriangleStrip::handle triangleStrip(new TriangleStrip());

		for(unsigned int c = 0; c < numCorners; c++) {
			for(unsigned int i = 0; i <= numCornerSegments;  i++) {
				triangleStrip->addVertex(beltBottom(c, i));
				triangleStrip->addVertex(bottomEdgeCorner(c, i));
			}
		}

		triangleStrip->addVertex(beltBottom(0, 0));
		triangleStrip->addVertex(bottomEdgeCorner(0, 0));

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

		for(unsigned int c = 0; c < numCorners; c++) {
			for(unsigned int i = 0; i < numQuarterHoleSegments;  i++) {
				triangleStrip->addVertex(holeBottomQuarter(c, i));
				triangleStrip->addVertex(holeTopQuarter(c, i));
			}
		}

		triangleStrip->addVertex(holeBottomQuarter(0, 0));
		triangleStrip->addVertex(holeTopQuarter(0, 0));
		primitives.push_back(triangleStrip);

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, material)));

		// Hole bottom...

		verts.clear();
		generateHoleBottomCenterVerts(verts, pModel->getOrigin());
		generateHoleBottomVerts(verts, pModel->getOrigin());

		primitives.clear();
		TriangleFan::handle triangleFan(new TriangleFan());
		triangleFan->addVertex(holeBottomCenter());

		for(unsigned int c = 0; c < numCorners; c++) {
			for(unsigned int i = 0; i < numQuarterHoleSegments;  i++) {
				triangleFan->addVertex(holeBottomQuarter(c, i));
			}
		}

		triangleFan->addVertex(holeBottomQuarter(0, 0));
		primitives.push_back(triangleFan);
		
		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, material)));

	}

	Point3d::list generateCornerPoints(Point3d center, double radius) {
		Point3d::list cornerPoints;
		cornerPoints.push_back(Vertex3d(center.x + radius, center.y + radius, center.z));
		cornerPoints.push_back(Vertex3d(center.x - radius, center.y + radius, center.z));
		cornerPoints.push_back(Vertex3d(center.x - radius, center.y - radius, center.z));
		cornerPoints.push_back(Vertex3d(center.x + radius, center.y - radius, center.z));
		return cornerPoints;
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

	void generateBottomSquareCornerVerts(Vertex3d::list &verts, Point3d origin) {
		Point3d bottomCenterPoint = origin;
		double innerRadius = radius - edgeRadius;

		bottomSquareCornerIndex = (Vertex3d::listIndex)verts.size();
		Point3d::list cornerPoints = generateCornerPoints(bottomCenterPoint, innerRadius);
		for(unsigned int c = 0; c < numCorners; c++) {
			verts.push_back(cornerPoints.at(c));
		}
	}

	/**
	 * @param c A corner index (0 through 3)
	 * @return The index of the vertex for the requested bottom square corner
	 */
	inline Vertex3d::listIndex bottomSquareCorner(unsigned int c) {
		return bottomSquareCornerIndex + (c % numCorners);
	}

	void generateBottomEdgeCornerVerts(Vertex3d::list &verts, Point3d origin) {
		double innerRadius = radius - edgeRadius;
		Point3d::list cornerPoints = generateCornerPoints(origin, innerRadius);

		bottomEdgeCornerIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int c = 0; c < numCorners; c++) {
			double tStart = (double)c / (double)numCorners;
			for(unsigned int i = 0; i <= numCornerSegments; i++) {
				double t = tStart + ((double)i/(double)numCorners)/(double)numCornerSegments;
				verts.push_back(cornerPoints.at(c) + edgeRadius * parametricUnitCircle(t));
			}
		}
	}

	/**
	 * @param c A corner index (0 through 3)
 	 * @param i An index of a corner segment break (0 through numCornerSegments)
	 * @return The index of the requested bottom edge corner vertex
	 */
	inline Vertex3d::listIndex bottomEdgeCorner(unsigned int c, unsigned int i) {
		return bottomEdgeCornerIndex
			+ (c % numCorners) * (numCornerSegments + 1)
			+ (i % (numCornerSegments + 1));
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

	void generateTopSquareCornerVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d topCenterOffset = Vector3d(0, 0, height);
		Point3d topCenterPoint = origin + topCenterOffset;
		double innerRadius = radius - edgeRadius;

		topSquareCornerIndex = (Vertex3d::listIndex)verts.size();
		Point3d::list cornerPoints = generateCornerPoints(topCenterPoint, innerRadius);
		for(unsigned int c = 0; c < numCorners; c++) {
			verts.push_back(cornerPoints.at(c));
		}
	}

	/**
	 * @param c A corner index (0 through 3)
	 * @return The index of the vertex for the requested top square corner
	 */
	inline Vertex3d::listIndex topSquareCorner(unsigned int c) {
		return topSquareCornerIndex + (c % numCorners);
	}
	
	void generateShoulderCornerVerts(Vertex3d::list &verts, Point3d origin) {
		Point3d topCenterPoint = origin + Vector3d(0, 0, height);
		double innerRadius = radius - edgeRadius;
		Point3d::list cornerPoints = generateCornerPoints(topCenterPoint, innerRadius);

		topShoulderCornerIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int c = 0; c < numCorners; c++) {
			double tIStart = (double)c / (double)numCorners;
			for(unsigned int i = 0; i <= numCornerSegments; i++) {
				double tI = tIStart + ((double)i/(double)numCorners)/(double)numCornerSegments;
				for(unsigned int j = 0; j < numShoulderSegments; j++) {
					double tJ = (double)(j+1) / (double)(numShoulderSegments * 4);
					Vector3d quarterUnitCircle = parametricUnitCircle(tJ);
					Vector3d shoulderVector = parametricUnitCircle(tI);
					shoulderVector.x *= quarterUnitCircle.y * edgeRadius;
					shoulderVector.y *= quarterUnitCircle.y * edgeRadius;
					shoulderVector.z -= edgeRadius * (1.0 - quarterUnitCircle.x);
					verts.push_back(cornerPoints.at(c) + shoulderVector);
				}
			}
		}
	}

	/**
	 * @param c A corner index (0 through 3)
	 * @param i An index of a corner segment break (0 through numCornerSegments)
	 * @param j An index of a shoulder segment (0 through numShoulderSegments)
	 * @return The index of the requested shoulder corner vertex
	 */
	inline Vertex3d::listIndex topShoulderCorner(unsigned int c, unsigned int i, unsigned int j) {
		return topShoulderCornerIndex
			+ (c % numCorners) * (numCornerSegments + 1) * numShoulderSegments
			+ (i % (numCornerSegments + 1)) * numShoulderSegments
			+ (j % (numShoulderSegments));
	}

	void generateBeltVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d beltCenterOffset = Vector3d(0, 0, beltHeight);
		Point3d beltCenterPoint = origin + beltCenterOffset;
		double innerRadius = radius - edgeRadius;
		Point3d::list cornerPoints = generateCornerPoints(beltCenterPoint, innerRadius);

		beltIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int c = 0; c < numCorners; c++) {
			double tStart = (double)c / (double)numCorners;
			for(unsigned int i = 0; i <= numCornerSegments; i++) {
				double t = tStart + ((double)i/(double)numCorners)/(double)numCornerSegments;
				for(unsigned int j = 0; j <= numBeltSegments; j++) {
					double s = (double)j/(2.0*(double)numBeltSegments);
					Vector3d v1 = beltRadius * parametricUnitCircle(s);
					Vector3d v2 = (edgeRadius - v1.y) * parametricUnitCircle(t);
					v2.z = v1.x;
					verts.push_back(cornerPoints.at(c) + v2);
				}
			}
		}
	}

	/**
	 * @param c A corner index (0 through 3)
	 * @param i An index of a corner segment break (0 through numCornerSegments)
	 * @param j An index of a belt segment break (0 through numBeltSegments)
	 * @return The index of the requested belt vertex
	 */
	inline Vertex3d::listIndex belt(unsigned int c, unsigned int i, unsigned int j) {
		return beltIndex
			+ (c % numCorners) * (numCornerSegments + 1) * (numBeltSegments + 1)
			+ (i % (numCornerSegments + 1)) * (numBeltSegments + 1)
			+ (j % (numBeltSegments + 1));
	}

	void generateBeltTopVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d beltTopCenterOffset = Vector3d(0, 0, beltHeight + beltRadius);
		Point3d beltTopCenterPoint = origin + beltTopCenterOffset;
		double innerRadius = radius - edgeRadius;
		Point3d::list cornerPoints = generateCornerPoints(beltTopCenterPoint, innerRadius);

		beltTopIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int c = 0; c < numCorners; c++) {
			double tStart = (double)c / (double)numCorners;
			for(unsigned int i = 0; i <= numCornerSegments; i++) {
				double t = tStart + ((double)i/(double)numCorners)/(double)numCornerSegments;
				verts.push_back(cornerPoints.at(c) + edgeRadius * parametricUnitCircle(t));
			}
		}
	}

	/**
	 * @param c A corner index (0 through 3)
	 * @param i An index of a corner segment break (0 through numCornerSegments)
	 * @return The index of the requested outer belt top vertex
	 */
	inline Vertex3d::listIndex beltTop(unsigned int c, unsigned int i) {
		return beltTopIndex
			+ (c % numCorners) * (numCornerSegments + 1)
			+ (i % (numCornerSegments + 1));
	}

	void generateBeltBottomVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d beltBottomCenterOffset = Vector3d(0, 0, beltHeight - beltRadius);
		Point3d beltBottomCenterPoint = origin + beltBottomCenterOffset;
		double innerRadius = radius - edgeRadius;
		Point3d::list cornerPoints = generateCornerPoints(beltBottomCenterPoint, innerRadius);

		beltBottomIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int c = 0; c < numCorners; c++) {
			double tStart = (double)c / (double)numCorners;
			for(unsigned int i = 0; i <= numCornerSegments; i++) {
				double t = tStart + ((double)i/(double)numCorners)/(double)numCornerSegments;
				verts.push_back(cornerPoints.at(c) + edgeRadius * parametricUnitCircle(t));
			}
		}
	}

	/**
	 * @param c A corner index (0 through 3)
	 * @param i An index of a corner segment break (0 through numCornerSegments)
	 * @return The index of the requested outer belt bottom vertex
	 */
	inline Vertex3d::listIndex beltBottom(unsigned int c, unsigned int i) {
		return beltBottomIndex
			+ (c % numCorners) * (numCornerSegments + 1)
			+ (i % (numCornerSegments + 1));
	}

	void generateHoleTopVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d topCenterOffset = Vector3d(0, 0, height);
		Point3d topCenterPoint = origin + topCenterOffset;

		holeTopIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int i = 0; i < numQuarterHoleSegments * 4; i++) {
			double t = (double)i/(double)(numQuarterHoleSegments * 4);
			verts.push_back(topCenterPoint + holeRadius * parametricUnitCircle(t));
		}
	}

	/**
	 * @param c A corner index (0 through 3)
	 * @param i An index of the quarter-hole (0 through numQuarterHoleSegments)
	 * @return The index of the requested top quarter-hole vertex
	 */
	inline Vertex3d::listIndex holeTopQuarter(unsigned int c, unsigned int i) {
		return holeTopIndex
			+ ((c % numCorners) * numQuarterHoleSegments
			+ (i % (numQuarterHoleSegments + 1)))
			% (numQuarterHoleSegments * 4);
	}

	void generateHoleBottomVerts(Vertex3d::list &verts, Point3d origin) {
		Vector3d holeBottomCenterOffset = Vector3d(0, 0, height - holeDepth);
		Point3d holeBottomCenterPoint = origin + holeBottomCenterOffset;

		holeBottomIndex = (Vertex3d::listIndex)verts.size();
		for(unsigned int i = 0; i < numQuarterHoleSegments * 4; i++) {
			double t = (double)i/(double)(numQuarterHoleSegments * 4);
			verts.push_back(holeBottomCenterPoint + holeRadius * parametricUnitCircle(t));
		}
	}

	/**
	 * @param c A corner index (0 through 3)
	 * @param i An index of the quarter-hole (0 through numQuarterHoleSegments)
	 * @return The index of the requested bottom quarter-hole vertex
	 */
	inline Vertex3d::listIndex holeBottomQuarter(unsigned int c, unsigned int i) {
		return holeBottomIndex
			+ ((c % numCorners) * numQuarterHoleSegments
			+ (i % (numQuarterHoleSegments + 1)))
			% (numQuarterHoleSegments * 4);
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

	void PieceModel::generateSquareModel() {
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
