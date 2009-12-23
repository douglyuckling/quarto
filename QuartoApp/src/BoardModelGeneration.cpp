/**
* @file BoardModelGeneration.cpp
*/
#include "BoardModel.hpp"
#include "Materials.hpp"
#include <Triangle.hpp>
#include <Quadrilateral.hpp>
#include <TriangleFan.hpp>
#include <TriangleStrip.hpp>
#include <SmoothMesh.hpp>

using peek::PI;
using peek::Vector3d;
using peek::parametricUnitCircle;
using peek::Primitive;
using peek::Triangle;
using peek::Quadrilateral;
using peek::TriangleFan;
using peek::TriangleStrip;
using peek::SmoothMesh;

namespace {

	using namespace quarto;

	const double markerRadius = BoardModel::markerRadius;
	const double markerBorderThickness = BoardModel::markerBorderThickness;
	const double markerBorderDepthOffset = BoardModel::markerBorderDepthOffset;
	const double boardBorderThickness = BoardModel::boardBorderThickness;
	const double boardBorderDepthOffset = BoardModel::boardBorderDepthOffset;
	const double boardRadius = BoardModel::boardRadius;
	const double markerSpacing = BoardModel::markerSpacing;
	const unsigned int markerMatrixSize = BoardModel::markerMatrixSize;
	const unsigned int numMarkerQuarterSegments = BoardModel::numMarkerQuarterSegments;
	const unsigned int numMarkerConcentricSegments = BoardModel::numMarkerConcentricSegments;
	const unsigned int numMarkerBorderConcentricSegments = BoardModel::numMarkerBorderConcentricSegments;
	const unsigned int numBoardBorderConcentricSegments = BoardModel::numBoardBorderConcentricSegments;
	const unsigned int numBoardBorderSegments = BoardModel::numBoardBorderSegments;
	
	void generateMarkerMeshes(Model *pModel);
	void generateMarkerBorderMeshes(Model *pModel);
	void generateMarkerBoundingBoxMeshes(Model *pModel);
	void generateInterMarkerBoundingBoxMeshes(Model *pModel);
	void generateBoardBorderMesh(Model *pModel);
	void generateBoardBorderPaddingMesh(Model *pModel);

	unsigned int markerCenterIndex;
	void generateMarkerCenterVerts(Vertex3d::list &verts);
	inline Vertex3d::listIndex markerCenterVert(unsigned int i, unsigned int j);

	unsigned int markerConcentricIndex;
	void generateMarkerConcentricVerts(Vertex3d::list &verts);
	inline Vertex3d::listIndex markerConcentricVert(unsigned int i, unsigned int j,
		unsigned int k, unsigned int l);
	inline Vertex3d::listIndex markerEdgeVert(unsigned int i, unsigned int j,
		unsigned int k);

	unsigned int markerBorderIndex;
	void generateMarkerBorderVerts(Vertex3d::list &verts);
	inline Vertex3d::listIndex markerBorderVert(unsigned int i, unsigned int j,
		unsigned int k, unsigned int l);

	unsigned int markerBorderEdgeIndex;
	void generateMarkerBorderEdgeVerts(Vertex3d::list &verts);
	inline Vertex3d::listIndex markerBorderEdgeVert(unsigned int i, unsigned int j,
		unsigned int k);

	unsigned int markerBoundingBoxIndex;
	void generateMarkerBoundingBoxVerts(Vertex3d::list &verts);
	inline Vertex3d::listIndex markerBoundingBoxVert(unsigned int i, unsigned int j,
		unsigned int k, int l);

	unsigned int boardBorderIndex;
	void generateBoardBorderVerts(Vertex3d::list &verts);
	inline Vertex3d::listIndex boardBorderVert(unsigned int k, unsigned int l);

	unsigned int boardBorderInsideIndex;
	void generateBoardBorderInsideVerts(Vertex3d::list &verts);
	inline Vertex3d::listIndex boardBorderInsideVert(unsigned int k);

	unsigned int boardBorderOutsideIndex;
	void generateBoardBorderOutsideVerts(Vertex3d::list &verts);
	inline Vertex3d::listIndex boardBorderOutsideVert(unsigned int k);

	unsigned int boardEdgeTopIndex;
	void generateBoardEdgeTopVerts(Vertex3d::list &verts);
	inline Vertex3d::listIndex boardEdgeTopVert(unsigned int k);

	unsigned int markerMatrixPerimeterIndex;
	void generateMarkerMatrixPerimeterVerts(Vertex3d::list &verts);
	inline Vertex3d::listIndex markerMatrixPerimeterVert(unsigned int l);

	void generateMarkerMeshes(Model *pModel) {

		// Generate vertices...
		Vertex3d::list verts;
		generateMarkerCenterVerts(verts);
		generateMarkerConcentricVerts(verts);

		// Generate faces...
		
		Primitive::list primitives;

		for(unsigned int i = 0; i < markerMatrixSize; i++) {
			for(unsigned int j = 0; j < markerMatrixSize; j++) {
				TriangleFan::handle triangleFan(new TriangleFan());

				triangleFan->addVertex(markerCenterVert(i, j));
				for(unsigned int k = 0; k <= (numMarkerQuarterSegments * 4); k++) {
					triangleFan->addVertex(markerConcentricVert(i, j, k, 0));
				}

				primitives.push_back(triangleFan);

				for(unsigned int l = 0; l < numMarkerConcentricSegments-1; l++) {
					TriangleStrip::handle triangleStrip(new TriangleStrip());

					for(unsigned int k = 0; k <= (numMarkerQuarterSegments * 4); k++) {
						triangleStrip->addVertex(markerConcentricVert(i, j, k, l));
						triangleStrip->addVertex(markerConcentricVert(i, j, k, l+1));
					}

					primitives.push_back(triangleStrip);
				}
			}
		}

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, boardSurfMaterial)));
	}

	void generateMarkerBorderMeshes(Model *pModel) {

		// Generate vertices...
		Vertex3d::list verts;
		generateMarkerConcentricVerts(verts);
		generateMarkerBorderVerts(verts);
		generateMarkerBorderEdgeVerts(verts);

		// Generate faces...
		
		Primitive::list primitives;

		for(unsigned int i = 0; i < markerMatrixSize; i++) {
			for(unsigned int j = 0; j < markerMatrixSize; j++) {
				TriangleStrip::handle triangleStrip(new TriangleStrip());
				for(unsigned int k = 0; k <= (numMarkerQuarterSegments * 4); k++) {
					triangleStrip->addVertex(markerEdgeVert(i, j, k));
					triangleStrip->addVertex(markerBorderVert(i, j, k, 1));
				}

				primitives.push_back(triangleStrip);

				for(unsigned int l = 1; l < numMarkerBorderConcentricSegments-1; l++) {
					triangleStrip = TriangleStrip::handle(new TriangleStrip());

					for(unsigned int k = 0; k <= (numMarkerQuarterSegments * 4); k++) {
						triangleStrip->addVertex(markerBorderVert(i, j, k, l));
						triangleStrip->addVertex(markerBorderVert(i, j, k, l+1));
					}

					primitives.push_back(triangleStrip);
				}

				triangleStrip = TriangleStrip::handle(new TriangleStrip());

				for(unsigned int k = 0; k <= (numMarkerQuarterSegments * 4); k++) {
					triangleStrip->addVertex(markerBorderVert(i, j, k, numMarkerBorderConcentricSegments-1));
					triangleStrip->addVertex(markerBorderEdgeVert(i, j, k));
				}

				primitives.push_back(triangleStrip);
			}
		}

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, boardGrooveMaterial)));
	}

	void generateMarkerBoundingBoxMeshes(Model *pModel) {

		// Generate vertices...
		Vertex3d::list verts;
		generateMarkerBorderEdgeVerts(verts);
		generateMarkerBoundingBoxVerts(verts);

		// Generate faces...
		
		Primitive::list primitives;

		for(unsigned int i = 0; i < markerMatrixSize; i++) {
			for(unsigned int j = 0; j < markerMatrixSize; j++) {
				for(unsigned int k = 0; k < 4; k++) {
					unsigned int x = markerMatrixSize-1;
					if( (i == 0 && j == 0 && k == 3) ||
						(i == x && j == 0 && k == 0) ||
						(i == x && j == x && k == 1) ||
						(i == 0 && j == x && k == 2))
						continue;

					TriangleStrip::handle triangleStrip(new TriangleStrip());
					
					for(unsigned int l = 0; l <= numMarkerQuarterSegments; l++) {
						int h = ((int) numMarkerQuarterSegments) / 2;
						int g = (int)l - (int)h;

						triangleStrip->addVertex(markerBoundingBoxVert(i, j, k, g));
						if (l == 0 || l == numMarkerQuarterSegments) continue;
						triangleStrip->addVertex(markerBorderEdgeVert(i, j, k*numMarkerQuarterSegments+l));
					}

					primitives.push_back(triangleStrip);
				}
			}
		}

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, boardSurfMaterial)));
	}

	void generateInterMarkerBoundingBoxMeshes(Model *pModel) {

		// Generate vertices...
		Vertex3d::list verts;
		generateMarkerBorderEdgeVerts(verts);
		generateMarkerBoundingBoxVerts(verts);

		// Generate faces...
		
		Primitive::list primitives;

		for(unsigned int i = 0; i < markerMatrixSize-1; i++) {
			for(unsigned int j = 0; j < markerMatrixSize-1; j++) {
				Vertex3d::listIndex v1 = markerBoundingBoxVert(i, j, 1, 0);
				Vertex3d::listIndex v2 = markerBoundingBoxVert(i+1, j, 2, 0);
				Vertex3d::listIndex v3 = markerBoundingBoxVert(i+1, j+1, 3, 0);
				Vertex3d::listIndex v4 = markerBoundingBoxVert(i, j+1, 0, 0);
				primitives.push_back(Quadrilateral::handle(new Quadrilateral(v1, v2, v3, v4)));
			}
		}

		for(unsigned int i = 0; i < markerMatrixSize-1; i++) {
			for(unsigned int j = 0; j < markerMatrixSize; j++) {
				TriangleStrip::handle triangleStrip(new TriangleStrip());

				for(unsigned int k = 0; k <= numMarkerQuarterSegments; k++) {
					triangleStrip->addVertex(markerBoundingBoxVert(i, j, 0, k));
					triangleStrip->addVertex(markerBoundingBoxVert(i+1, j, 3, -(int)k));
				}

				primitives.push_back(triangleStrip);
			}
		}

		for(unsigned int i = 0; i < markerMatrixSize; i++) {
			for(unsigned int j = 0; j < markerMatrixSize-1; j++) {
				TriangleStrip::handle triangleStrip(new TriangleStrip());

				for(unsigned int k = 0; k <= numMarkerQuarterSegments; k++) {
					triangleStrip->addVertex(markerBoundingBoxVert(i, j, 1, k));
					triangleStrip->addVertex(markerBoundingBoxVert(i, j+1, 0, -(int)k));
				}

				primitives.push_back(triangleStrip);
			}
		}

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, boardSurfMaterial)));
	}

	void generateBoardBorderMesh(Model *pModel) {

		// Generate vertices...
		Vertex3d::list verts;
		generateBoardBorderVerts(verts);

		// Generate faces...
		
		Primitive::list primitives;

		unsigned int numBoardBorderSegments = 4*((markerMatrixSize-1)*(numMarkerQuarterSegments+1) + numMarkerQuarterSegments);
		for(unsigned int l = 0; l < numBoardBorderConcentricSegments; l++) {
			TriangleStrip::handle triangleStrip(new TriangleStrip());
			
			for(unsigned int k = 0; k <= numBoardBorderSegments; k++) {
				triangleStrip->addVertex(boardBorderVert(k, l));
				triangleStrip->addVertex(boardBorderVert(k, l+1));
			}

			primitives.push_back(triangleStrip);
		}

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, boardGrooveMaterial)));
	}

	void generateBoardBorderPaddingMesh(Model *pModel) {

		// Generate vertices...
		Vertex3d::list verts;
		generateMarkerMatrixPerimeterVerts(verts);
		generateBoardBorderInsideVerts(verts);

		// Generate faces...
		
		Primitive::list primitives;

		unsigned int numBoardBorderSegments = 4 * ((markerMatrixSize-1)*(numMarkerQuarterSegments+1) + numMarkerQuarterSegments);
		TriangleStrip::handle triangleStrip(new TriangleStrip());	

		for(unsigned int i = 0; i <= numBoardBorderSegments; i++) {
			triangleStrip->addVertex(markerMatrixPerimeterVert(i));
			triangleStrip->addVertex(boardBorderInsideVert(i));
		}

		primitives.push_back(triangleStrip);

		/*
		unsigned int numBoardBorderSegments = 4*(3*markerMatrixSize-3 + numMarkerQuarterSegments);
		for(unsigned int n = 0; n < 4; n++) {
			for(unsigned int a = 0; a < numMarkerQuarterSegments; a++) {
				unsigned int k1 = n*(3*markerMatrixSize-3 + numMarkerQuarterSegments) + a;
				unsigned int k2 = n*(2*(3*markerMatrixSize-3) + numMarkerQuarterSegments) + a;
				Vertex3d::listIndex v1 = markerMatrixPerimeterVert(k1);
				Vertex3d::listIndex v2 = boardBorderInsideVert(k2);
				Vertex3d::listIndex v3 = markerMatrixPerimeterVert(k1+1);
				Vertex3d::listIndex v4 = boardBorderInsideVert(k2+1);
				faces.push_back(Triangle(v1, v2, v3));
				faces.push_back(Triangle(v3, v2, v4));
			}
			for(unsigned int b = 0; b < 3*markerMatrixSize-3; b++) {
				unsigned int k1 = n*(3*markerMatrixSize-3 + numMarkerQuarterSegments) + numMarkerQuarterSegments + b;
				unsigned int k2 = n*(2*(3*markerMatrixSize-3) + numMarkerQuarterSegments) + numMarkerQuarterSegments + 2*b;
				Vertex3d::listIndex v1 = markerMatrixPerimeterVert(k1);
				Vertex3d::listIndex v2 = boardBorderInsideVert(k2);
				Vertex3d::listIndex v3 = boardBorderInsideVert(k2+1);
				Vertex3d::listIndex v4 = markerMatrixPerimeterVert(k1+1);
				Vertex3d::listIndex v5 = boardBorderInsideVert(k2+2);
				faces.push_back(Triangle(v1, v2, v3));
				faces.push_back(Triangle(v1, v3, v4));
				faces.push_back(Triangle(v4, v3, v5));
			}
		}
		*/
		/*
		for(unsigned int k = 0; k < numBoardBorderSegments; k++) {
			Vertex3d::listIndex v1 = markerMatrixPerimeterVert(k);
			Vertex3d::listIndex v2 = boardBorderInsideVert(k);
			Vertex3d::listIndex v3 = markerMatrixPerimeterVert(k+1);
			Vertex3d::listIndex v4 = boardBorderInsideVert(k+1);
			faces.push_back(Triangle(v1, v2, v3));
			faces.push_back(Triangle(v3, v2, v4));
		}
		*/

		pModel->addMesh(SmoothMesh::handle(new SmoothMesh(verts, primitives, boardSurfMaterial)));
	}

	void generateMarkerCenterVerts(Vertex3d::list &verts) {
		markerCenterIndex = (unsigned int)verts.size();
		for(unsigned int i = 0; i < markerMatrixSize; i++) {
			for(unsigned int j = 0; j < markerMatrixSize; j++) {
				verts.push_back(BoardModel::getMarkerPosition(i, j));
			}
		}
	}

	/**
	 * @param i The i-coordinate of the desired marker
	 * @param j The j-coordinate of the desired marker
	 * @return The index of the requested marker center vertex
	 */
	inline Vertex3d::listIndex markerCenterVert(unsigned int i, unsigned int j) {
		return markerCenterIndex
			+ (i % markerMatrixSize) * markerMatrixSize 
			+ (j % markerMatrixSize);
	}

	void generateMarkerConcentricVerts(Vertex3d::list &verts) {
		double r = markerRadius / (double) numMarkerConcentricSegments;

		markerConcentricIndex = (unsigned int)verts.size();
		for(unsigned int i = 0; i < markerMatrixSize; i++) {
			for(unsigned int j = 0; j < markerMatrixSize; j++) {
				Point3d markerCenterPoint = BoardModel::getMarkerPosition(i, j);
				for(unsigned int k = 0; k < (numMarkerQuarterSegments * 4); k++) {
					double t = (double)k/(double)(numMarkerQuarterSegments * 4) - 0.125;
					for (unsigned int l = 1; l < numMarkerConcentricSegments; l++) {
						verts.push_back(markerCenterPoint + ((double)l*r) * parametricUnitCircle(t));
					}
					verts.push_back(markerCenterPoint + markerRadius * parametricUnitCircle(t));
				}
			}
		}
	}

	/**
	 * @param i The i-coordinate of the desired marker
	 * @param j The j-coordinate of the desired marker
	 * @param k The theta-index of the marker's desired vertex
	 * @param l The index of the marker's desired concentric ring
	 * @return The index of the requested marker edge vertex
	 */
	inline Vertex3d::listIndex markerConcentricVert(unsigned int i, unsigned int j, unsigned int k, unsigned int l) {
		return markerConcentricIndex
			+ (i % markerMatrixSize) * markerMatrixSize * numMarkerQuarterSegments * 4 * numMarkerConcentricSegments
			+ (j % markerMatrixSize) * numMarkerQuarterSegments * 4 * numMarkerConcentricSegments
			+ (k % (numMarkerQuarterSegments * 4)) * numMarkerConcentricSegments
			+ (l % numMarkerConcentricSegments);
	}

	/**
	 * @param i The i-coordinate of the desired marker
	 * @param j The j-coordinate of the desired marker
	 * @param k The index of the marker's desired edge vertex
	 * @return The index of the requested marker edge vertex
	 */
	inline Vertex3d::listIndex markerEdgeVert(unsigned int i, unsigned int j, unsigned int k) {
		return markerConcentricVert(i, j, k, numMarkerConcentricSegments-1);
	}

	void generateMarkerBorderVerts(Vertex3d::list &verts) {
		static const double markerBorderRadius = sqrt(pow(markerBorderThickness/2.0, 2.0) + pow(markerBorderDepthOffset, 2.0));
		static const double d = acos(markerBorderDepthOffset / markerBorderRadius) / (2.0 * PI);

		markerBorderIndex = (unsigned int)verts.size();
		for(unsigned int i = 0; i < markerMatrixSize; i++) {
			for(unsigned int j = 0; j < markerMatrixSize; j++) {
				Point3d markerCenterPoint = BoardModel::getMarkerPosition(i, j);
				for(unsigned int k = 0; k < (numMarkerQuarterSegments * 4); k++) {
					double t = (double)k / (double)(numMarkerQuarterSegments * 4) - 0.125;
					for(unsigned int l = 0; l <= numMarkerBorderConcentricSegments; l++) {
						double u = (double)l / (double)numMarkerBorderConcentricSegments;
						Vector3d v = markerBorderRadius * parametricUnitCircle(2.0 * u * d + (0.25 - d));
						double radius = markerRadius + markerBorderThickness/2.0 - v.x;
						double dip = markerBorderDepthOffset - v.y;
						verts.push_back(markerCenterPoint
							+ radius * parametricUnitCircle(t)
							+ Vector3d(0, 0, dip));
					}
				}
			}
		}
	}

	/**
	 * @param i The i-coordinate of the desired marker
	 * @param j The j-coordinate of the desired marker
	 * @param k The theta-index of the marker's desired border vertex
	 * @param l The border-index of the marker's desired border vertex
	 * @return The index of the requested marker border vertex
	 */
	inline Vertex3d::listIndex markerBorderVert(unsigned int i, unsigned int j, unsigned int k, unsigned int l) {
		return markerBorderIndex
			+ (i % markerMatrixSize) * markerMatrixSize * numMarkerQuarterSegments * 4 * (numMarkerBorderConcentricSegments+1)
			+ (j % markerMatrixSize) * numMarkerQuarterSegments * 4 * (numMarkerBorderConcentricSegments+1)
			+ (k % (numMarkerQuarterSegments * 4)) * (numMarkerBorderConcentricSegments+1)
			+ (l % (numMarkerBorderConcentricSegments+1));
	}

	void generateMarkerBorderEdgeVerts(Vertex3d::list &verts) {
		double radius = markerRadius + markerBorderThickness;

		markerBorderEdgeIndex = (unsigned int)verts.size();
		for(unsigned int i = 0; i < markerMatrixSize; i++) {
			for(unsigned int j = 0; j < markerMatrixSize; j++) {
				Point3d markerCenterPoint = BoardModel::getMarkerPosition(i, j);
				for(unsigned int k = 0; k < (numMarkerQuarterSegments * 4); k++) {
					double t = (double)k / (double)(numMarkerQuarterSegments * 4) - 0.125;
					verts.push_back(markerCenterPoint + radius * parametricUnitCircle(t));
				}
			}
		}
	}

	/**
	 * @param i The i-coordinate of the desired marker
	 * @param j The j-coordinate of the desired marker
	 * @param k The theta-index of the marker's desired border edge vertex
	 * @return The index of the requested marker border edge vertex
	 */
	inline Vertex3d::listIndex markerBorderEdgeVert(unsigned int i, unsigned int j, unsigned int k) {
		return markerBorderEdgeIndex
			+ (i % markerMatrixSize) * markerMatrixSize * numMarkerQuarterSegments * 4
			+ (j % markerMatrixSize) * numMarkerQuarterSegments * 4
			+ (k % (numMarkerQuarterSegments * 4));
	}

	void generateMarkerBoundingBoxVerts(Vertex3d::list &verts) {
		static const double r = sqrt(2.0 * pow(markerRadius + markerBorderThickness, 2.0));

		markerBoundingBoxIndex = (unsigned int)verts.size();
		for(unsigned int i = 0; i < markerMatrixSize; i++) {
			for(unsigned int j = 0; j < markerMatrixSize; j++) {
				Point3d markerCenterPoint = BoardModel::getMarkerPosition(i, j);
				for(unsigned int k = 0; k < 4; k++) {
					double t1 = (double)k / 4.0;
					double t2 = (double)(k+1) / 4.0;
					Point3d p1 = markerCenterPoint + r * parametricUnitCircle(t1);
					Point3d p2 = markerCenterPoint + r * parametricUnitCircle(t2);
					Vector3d v = p2 - p1;

					for(unsigned int l = 0; l < numMarkerQuarterSegments; l++) {
						double s = (double) l / (double) numMarkerQuarterSegments;
						verts.push_back(p1 + s * v);
					}
				}
			}
		}
	}

	/**
	 * @param i The i-coordinate of the desired marker
	 * @param j The j-coordinate of the desired marker
	 * @param k The side-index of the marker's desired bounding box side
	 * @param l The index of the desired vertex along the requested side
	 * @return The index of the requested marker bounding box vertex
	 */
	inline Vertex3d::listIndex markerBoundingBoxVert(unsigned int i, unsigned int j, unsigned int k, int l) {
		int m = (k % 4) * numMarkerQuarterSegments + (l);

		while(m < 0) {
			m += 4 * numMarkerQuarterSegments;
		}

		m %= 4 * numMarkerQuarterSegments;

		return markerBoundingBoxIndex
			+ (i % markerMatrixSize) * markerMatrixSize * 4 * numMarkerQuarterSegments
			+ (j % markerMatrixSize) * 4 * numMarkerQuarterSegments
			+ m;
	}

	void generateBoardBorderVerts(Vertex3d::list &verts) {
		static const double cornerAngle = 0.03125; // Portion of cicle from 0 to 1
		static const double sideAngle = 0.25 - cornerAngle; // Portion of circle from 0 to 1
		static const double boardBorderRadius = sqrt(pow(boardBorderThickness/2.0, 2.0) + pow(boardBorderDepthOffset, 2.0));
		static const double d = acos(boardBorderDepthOffset / boardBorderRadius) / (2.0 * PI);

		boardBorderIndex = (unsigned int)verts.size();
		Point3d boardCenterPoint;
		for(unsigned int n = 0; n < 4; n++) {
			for(unsigned int k = 0; k < numMarkerQuarterSegments; k++) {
				double t = (double)k / (double)numMarkerQuarterSegments;
				for(unsigned int l = 0; l <= numBoardBorderConcentricSegments; l++) {
					double u = (double)l / (double)numBoardBorderConcentricSegments;
					Vector3d v = boardBorderRadius * parametricUnitCircle(2.0 * u * d + (0.25 - d));
					double radius = boardRadius + boardBorderThickness/2.0 - v.x;
					double dip = boardBorderDepthOffset - v.y;
					verts.push_back(boardCenterPoint
						+ radius * parametricUnitCircle((double)n * 0.25 + t * cornerAngle - cornerAngle / 2.0)
						+ Vector3d(0, 0, dip));
				}
			}

			unsigned int numBoardBorderSideSegments = (markerMatrixSize-1)*(numMarkerQuarterSegments+1);
			for(unsigned int k = 0; k < numBoardBorderSideSegments; k++) {
				double t = (double)k / (double)numBoardBorderSideSegments;
				for(unsigned int l = 0; l <= numBoardBorderConcentricSegments; l++) {
					double u = (double)l / (double)numBoardBorderConcentricSegments;
					Vector3d v = boardBorderRadius * parametricUnitCircle(2.0 * u * d + (0.25 - d));
					double radius = boardRadius + boardBorderThickness/2.0 - v.x;
					double dip = boardBorderDepthOffset - v.y;
					verts.push_back(boardCenterPoint
						+ radius * parametricUnitCircle((double)n * 0.25 + t * sideAngle + cornerAngle / 2.0)
						+ Vector3d(0, 0, dip));
				}
			}
		}
	}

	/**
	 * @param k The theta-index of the board's desired border  vertex
	 * @param l The concentric-index of the board's desired border vertex
	 * @return The index of the requested marker border  vertex
	 */
	inline Vertex3d::listIndex boardBorderVert(unsigned int k, unsigned int l) {
		unsigned int numBoardBorderSegments = 4*((markerMatrixSize-1)*(numMarkerQuarterSegments+1) + numMarkerQuarterSegments);
		return boardBorderIndex
			+ (k % numBoardBorderSegments) * (numBoardBorderConcentricSegments+1)
			+ (l % (numBoardBorderConcentricSegments+1));
	}

	void generateBoardBorderInsideVerts(Vertex3d::list &verts) {
		double radius = boardRadius;
		static const double cornerAngle = 0.03125; // Portion of cicle from 0 to 1
		static const double sideAngle = 0.25 - cornerAngle; // Portion of circle from 0 to 1

		boardBorderInsideIndex = (unsigned int)verts.size();
		Point3d boardCenterPoint;
		for(unsigned int n = 0; n < 4; n++) {
			for(unsigned int k = 0; k < numMarkerQuarterSegments; k++) {
				double t = (double)k / (double)numMarkerQuarterSegments;
				verts.push_back(boardCenterPoint
					+ radius * parametricUnitCircle((double)n * 0.25 + t * cornerAngle - cornerAngle / 2.0));
			}
			//for(unsigned int k = 0; k < (3 * (numMarkerQuarterSegments+1) + 1); k++) {
			for(unsigned int k = 0; k < (markerMatrixSize-1)*(numMarkerQuarterSegments+1); k++) {
				double t = (double)k / ((markerMatrixSize-1)*(numMarkerQuarterSegments+1));
				verts.push_back(boardCenterPoint
					+ radius * parametricUnitCircle((double)n * 0.25 + t * sideAngle + cornerAngle / 2.0));
			}
		}
	}

	/**
	 * @param k The theta-index of the board's desired border inside vertex
	 * @return The index of the requested marker border inside vertex
	 */
	inline Vertex3d::listIndex boardBorderInsideVert(unsigned int k) {
		unsigned int numBoardBorderSegments = 4 * ((markerMatrixSize-1)*(numMarkerQuarterSegments+1) + numMarkerQuarterSegments);
		return boardBorderInsideIndex + (k % numBoardBorderSegments);
	}

	void generateBoardBorderOutsideVerts(Vertex3d::list &verts) {
		double radius = boardRadius + boardBorderThickness;
		static const double cornerAngle = 0.03125; // Portion of cicle from 0 to 1
		static const double sideAngle = 0.25 - cornerAngle; // Portion of circle from 0 to 1

		boardBorderOutsideIndex = (unsigned int)verts.size();
		Point3d boardCenterPoint;
		for(unsigned int n = 0; n < 4; n++) {
			for(unsigned int k = 0; k < numMarkerQuarterSegments; k++) {
				double t = (double)k / (double)numMarkerQuarterSegments;
				verts.push_back(boardCenterPoint
					+ radius * parametricUnitCircle((double)n * 0.25 + t * cornerAngle - cornerAngle / 2.0));
			}
			for(unsigned int k = 0; k < 2 * (3 * markerMatrixSize - 3); k++) {
				double t = (double)k / (double)(2 * (3 * markerMatrixSize - 3));
				verts.push_back(boardCenterPoint
					+ radius * parametricUnitCircle((double)n * 0.25 + t * sideAngle + cornerAngle / 2.0));
			}
		}
	}

	/**
	 * @param k The theta-index of the board's desired border outside vertex
	 * @return The index of the requested marker border outside vertex
	 */
	inline Vertex3d::listIndex boardBorderOutsideVert(unsigned int k) {
		unsigned int numBoardBorderSegments = 4*(2*(3*markerMatrixSize-3) + numMarkerQuarterSegments);
		return boardBorderOutsideIndex + (k % numBoardBorderSegments);
	}

	void generateBoardEdgeTopVerts(Vertex3d::list &verts) {
		double radius = boardRadius + boardBorderThickness;
		static const double cornerAngle = 0.03125; // Portion of cicle from 0 to 1
		static const double sideAngle = 0.25 - cornerAngle; // Portion of circle from 0 to 1

		boardEdgeTopIndex = (unsigned int)verts.size();
		Point3d boardCenterPoint;
		for(unsigned int n = 0; n < 4; n++) {
			for(unsigned int k = 0; k < numMarkerQuarterSegments; k++) {
				double t = (double)k / (double)numMarkerQuarterSegments;
				verts.push_back(boardCenterPoint
					+ radius * parametricUnitCircle((double)n * 0.25 + t * cornerAngle - cornerAngle / 2.0));
			}
			for(unsigned int k = 0; k < 2 * (3 * markerMatrixSize - 3); k++) {
				double t = (double)k / (double)(2 * (3 * markerMatrixSize - 3));
				verts.push_back(boardCenterPoint
					+ radius * parametricUnitCircle((double)n * 0.25 + t * sideAngle + cornerAngle / 2.0));
			}
		}
	}

	/**
	 * @param k The theta-index of the board's desired border outside vertex
	 * @return The index of the requested marker border outside vertex
	 */
	inline Vertex3d::listIndex boardEdgeTopVert(unsigned int k) {
		unsigned int numBoardBorderSegments = 4*(2*(3*markerMatrixSize-3) + numMarkerQuarterSegments);
		return boardBorderOutsideIndex + (k % numBoardBorderSegments);
	}

	void generateMarkerMatrixPerimeterVerts(Vertex3d::list &verts) {
		static const double r = sqrt(2.0 * pow(markerRadius + markerBorderThickness, 2.0));
		static const double r2 = markerRadius + markerBorderThickness;

		markerMatrixPerimeterIndex = (unsigned int)verts.size();
		for(unsigned int n = 0; n < 4; n++) {
			unsigned int i = (n == 2 || n == 3) ? 0 : markerMatrixSize-1;
			unsigned int j = (n == 0 || n == 3) ? 0 : markerMatrixSize-1;
			Point3d markerCenterPoint = BoardModel::getMarkerPosition(i, j);
			
			for(unsigned int k = 0; k < numMarkerQuarterSegments; k++) {
					double t = (double)k / (double)(numMarkerQuarterSegments * 4) + (double)n * 0.25 - 0.125;
					verts.push_back(markerCenterPoint + r2 * parametricUnitCircle(t));
			}
			
			for(unsigned int k = 0; k < markerMatrixSize; k++) {
				markerCenterPoint = BoardModel::getMarkerPosition(i, j);
				double t1 = (double)n*0.25;
				double t2 = (double)(n+1)*0.25;
				Point3d p1 = markerCenterPoint + r * parametricUnitCircle(t1);
				Point3d p2 = markerCenterPoint + r * parametricUnitCircle(t2);
				Vector3d v = p2 - p1;

				for(unsigned int l = 0; l <= numMarkerQuarterSegments; l++) {
					if(k == 0 && l < (numMarkerQuarterSegments/2)) continue;
					if(k == markerMatrixSize-1 && l >= (numMarkerQuarterSegments/2)) continue;

					double s = (double) l / (double) numMarkerQuarterSegments;
					verts.push_back(p1 + s * v);
				}
				int delta = (n == 0 || n == 3) ? 1 : -1;
				if(n % 2 == 0) {
					j += delta;
				}
				else {
					//i
					i += delta;
				}
			}
		}
	}

	/**
	 * @param k The index of the desired marker matrix perimeter vertex
	 * @return The index of the requested marker matrix perimeter vertex
	 */
	inline Vertex3d::listIndex markerMatrixPerimeterVert(unsigned int k) {
		unsigned int numBoardBorderSegments = 4*((markerMatrixSize-1)*(numMarkerQuarterSegments+1) + numMarkerQuarterSegments);
		return markerMatrixPerimeterIndex + (k % numBoardBorderSegments);
	}

}

namespace quarto {

	void BoardModel::generateModel() {
		Model *pModel = this;
		pModel->setOrigin(Point3d(0.0, 0.0, 0.0));

		generateMarkerMeshes(pModel);
		generateMarkerBorderMeshes(pModel);
		generateMarkerBoundingBoxMeshes(pModel);
		generateInterMarkerBoundingBoxMeshes(pModel);
		generateBoardBorderMesh(pModel);
		generateBoardBorderPaddingMesh(pModel);
	}

}
