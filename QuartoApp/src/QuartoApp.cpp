/**
* @file QuartoApp.cpp
*/
#include "QuartoApp.hpp"
#include "Lights.hpp"
#include <GlWrappers.hpp>

using peek::Camera;
using peek::PerspectiveCamera;
using peek::Point3d;
using peek::parametricUnitCircle;
using peek::SceneGraphNode;
using peek::SceneGraphLeaf;

namespace quarto {

	QuartoApp::QuartoApp() {
		this->leftMouseButtonDown = false;
		this->rightMouseButtonDown = false;
		Camera::handle camera = PerspectiveCamera::handle(new PerspectiveCamera());
		this->cameraRigging.reset(new FirstPersonCameraRigging(camera, Point3d(0, -30, 24), 90.0, 50.0));
	}

	void QuartoApp::run() {
		generateModels();
		buildSceneGraph();
		bindKeys();

		this->engine.setDrawable(this);
		this->engine.setCustomEventHandler(this);
		this->engine.setMouseButtonEventHandler(this);
		this->engine.setMouseMotionEventHandler(this);
		this->engine.setResizeEventHandler(this);

		restartGame();
		this->engine.run();
	}

	void QuartoApp::draw() {
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->cameraRigging->getCamera()->setProjectionMatrix();
		this->cameraRigging->initModelviewMatrix();

		// Place and enable the lights
		for(int i=0; i < numLights; i++) {
			initLight(GL_LIGHT0+i, lights[i]);
			glEnable(GL_LIGHT0+i);
		}

		// Draw the scene
		this->sceneGraph->draw();

		glFlush();
	}

	void QuartoApp::pickPiece(int x, int y) {
		this->cameraRigging->getCamera()->setPickingMatrix(x, y);
		this->cameraRigging->initModelviewMatrix();

		glSelectBuffer(BUFSIZE, this->selectBuf);
		glRenderMode(GL_SELECT);

		glInitNames();

		// Draw the pieces
		for(unsigned int i = 0; i < this->availablePieceModels.size(); i++) {
			glPushName(i);
			this->availablePieceModels.at(i)->pick();
			glPopName();
		}

		int numHits = glRenderMode(GL_RENDER);

		GLuint *curHit = this->selectBuf;
		GLuint nearestHitName;
		GLuint nearestHitDepth = 0xffffffff;

		if(numHits > 0) {
			for(int i = 0; i < numHits; i++) {
				GLuint numNames = curHit[0];
				GLuint hitMinDepth = curHit[1];
				GLuint hitMaxDepth = curHit[2];

				if(numNames > 0) {
					GLuint *names = &curHit[3];

					if(hitMinDepth < nearestHitDepth) {
						nearestHitName = names[0];
						nearestHitDepth = hitMinDepth;
					}
				}

				curHit += (numNames + 3);
			}

			PieceModel::handle pieceModel = this->availablePieceModels.at(nearestHitName);

			game.choosePiece(Piece(pieceModel->isRound(), pieceModel->isTall(), pieceModel->isHollow(), pieceModel->isWhite()));
		}
	}

	void QuartoApp::pickMarker(int x, int y) {
		this->cameraRigging->getCamera()->setPickingMatrix(x, y);
		this->cameraRigging->initModelviewMatrix();

		glSelectBuffer(BUFSIZE, this->selectBuf);
		glRenderMode(GL_SELECT);

		glInitNames();

		// Draw the markers
		for(unsigned int i = 0; i < 4; i++) {
			glPushName(i);
			for(unsigned int j = 0; j < 4; j++) {
				glPushName(j);
				this->markerModel.setOrigin(this->boardModel->getMarkerPosition(i, j));
				this->markerModel.pick();
				glPopName();
			}
			glPopName();
		}

		int numHits = glRenderMode(GL_RENDER);

		GLuint *curHit = this->selectBuf;
		GLuint nearestHitNameI;
		GLuint nearestHitNameJ;
		GLuint nearestHitDepth = 0xffffffff;

		if(numHits > 0) {
			for(int i = 0; i < numHits; i++) {
				GLuint numNames = curHit[0];
				GLuint hitMinDepth = curHit[1];
				GLuint hitMaxDepth = curHit[2];

				if(numNames > 1) {
					GLuint *names = &curHit[3];

					if(hitMinDepth < nearestHitDepth) {
						nearestHitNameI = names[0];
						nearestHitNameJ = names[1];
						nearestHitDepth = hitMinDepth;
					}
				}

				curHit += (numNames + 3);
			}

			game.placePiece(nearestHitNameI, nearestHitNameJ);

			PieceModel::handle pieceModel = getPieceModel(game.getChosenPiece());
			pieceModel->setOrigin(this->boardModel->getMarkerPosition(nearestHitNameI, nearestHitNameJ));
		}
	}

	/*!
	* @param action The action to take
	*/
	void QuartoApp::handleCustomEvent(int customEvent) {
		switch(customEvent) {
		case QE_EXIT:
			exit(0);
		case QE_MOVE_FORWARD:
			this->cameraRigging->moveForward();
			this->engine.invalidate();
			break;
		case QE_MOVE_BACKWARD:
			this->cameraRigging->moveBackward();
			this->engine.invalidate();
			break;
		case QE_MOVE_LEFT:
			this->cameraRigging->moveLeft();
			this->engine.invalidate();
			break;
		case QE_MOVE_RIGHT:
			this->cameraRigging->moveRight();
			this->engine.invalidate();
			break;
		case QE_MOVE_UP:
			this->cameraRigging->moveUp();
			this->engine.invalidate();
			break;
		case QE_MOVE_DOWN:
			this->cameraRigging->moveDown();
			this->engine.invalidate();
			break;
		case QE_TOGGLE_SOLID_GEOMETRY:
			this->boardModel->toggleShowSolidGeometry();
			for(unsigned int i = 0; i < this->pieceModels.size(); i++)
				this->pieceModels.at(i)->toggleShowSolidGeometry();
			this->engine.invalidate();
			break;
		case QE_TOGGLE_WIREFRAME:
			this->boardModel->toggleShowWireframe();
			for(unsigned int i = 0; i < this->pieceModels.size(); i++)
				this->pieceModels.at(i)->toggleShowWireframe();
			this->engine.invalidate();
			break;
		case QE_TOGGLE_NORMALS:
			this->boardModel->toggleShowNormals();
			for(unsigned int i = 0; i < this->pieceModels.size(); i++)
				this->pieceModels.at(i)->toggleShowNormals();
			this->engine.invalidate();
			break;
		case QE_TOGGLE_SMOOTH_SHADING:
			this->boardModel->toggleSmoothShading();
			for(unsigned int i = 0; i < this->pieceModels.size(); i++)
				this->pieceModels.at(i)->toggleSmoothShading();
			this->engine.invalidate();
			break;
		case QE_DECREASE_NORMAL_SCALE:
			this->boardModel->decreaseNormalScale();
			for(unsigned int i = 0; i < this->pieceModels.size(); i++)
				this->pieceModels.at(i)->decreaseNormalScale();
			this->engine.invalidate();
			break;
		case QE_INCREASE_NORMAL_SCALE:
			this->boardModel->increaseNormalScale();
			for(unsigned int i = 0; i < this->pieceModels.size(); i++)
				this->pieceModels.at(i)->increaseNormalScale();
			this->engine.invalidate();
			break;
		case QE_RESET_GAME:
			restartGame();
			this->engine.invalidate();
			break;
		default: break;
		}
	}

	void QuartoApp::restartGame() {
		this->game.reset();
		placeModels();
		calculateAvailablePieceModels();
		this->game.printStateMessage();
		this->game.start();
		this->game.printStateMessage();
	}

	/*!
	* @param buttonEvent The button event
	*/
	void QuartoApp::handleMouseButtonEvent(const SDL_MouseButtonEvent &buttonEvent) {
		const int button = buttonEvent.button;
		const int state = buttonEvent.state;
		const int x = buttonEvent.x;
		const int y = buttonEvent.y;

		if(button == SDL_BUTTON_LEFT) {
			if(state == SDL_PRESSED) {
				this->leftMouseButtonDown = true;
			} else {
				this->leftMouseButtonDown = false;

				if (game.getState() == P1_CHOOSE || game.getState() == P2_CHOOSE) {
					pickPiece(x, y);
					game.printStateMessage();
				}
				else if (game.getState() == P1_PLACE || game.getState() == P2_PLACE) {
					pickMarker(x, y);
					calculateAvailablePieceModels();
					game.printStateMessage();
					this->engine.invalidate();
				}
			}
		} else if(button == SDL_BUTTON_RIGHT) {
			if(state == SDL_PRESSED) {
				this->rightMouseButtonDown = true;
				SDL_ShowCursor(SDL_DISABLE);
				SDL_WM_GrabInput(SDL_GRAB_ON);
			} else {
				this->rightMouseButtonDown = false;
				SDL_ShowCursor(SDL_ENABLE);
				SDL_WM_GrabInput(SDL_GRAB_OFF);
			}
		} else if(button == SDL_BUTTON_WHEELUP) {
			this->engine.invalidate();
			this->cameraRigging->moveUp();
		} else if(button == SDL_BUTTON_WHEELDOWN) {
			this->cameraRigging->moveDown();
			this->engine.invalidate();
		}
	}

	/*!
	* @param motionEvent The motion event
	*/
	void QuartoApp::handleMouseMotionEvent(const SDL_MouseMotionEvent &motionEvent) {
		int dx = motionEvent.xrel;
		int dy = motionEvent.yrel;

		if(this->rightMouseButtonDown == true) {
			this->cameraRigging->look(dx, dy);
			this->engine.invalidate();
		}
	}

	/*!
	* @param sizeX The new x-size of the window
	* @param sizeY The new y-size of the window
	*/
	void QuartoApp::handleResizeEvent(int sizeX, int sizeY) {
		this->cameraRigging->getCamera()->setAspectRatio((double)sizeX/(double)sizeY);
	}

	void QuartoApp::generateModels() {
		this->boardModel = BoardModel::handle(new BoardModel());
		
		this->pieceModels.clear();
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(false, false, false, false)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(false, false, false, true)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(false, false, true, false)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(false, false, true, true)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(false, true, false, false)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(false, true, false, true)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(false, true, true, false)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(false, true, true, true)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(true, false, false, false)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(true, false, false, true)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(true, false, true, false)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(true, false, true, true)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(true, true, false, false)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(true, true, false, true)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(true, true, true, false)));
		this->pieceModels.push_back(PieceModel::handle(new PieceModel(true, true, true, true)));
	}

	void QuartoApp::placeModels() {
		double r = 15.0;

		for(unsigned int i = 0; i < this->pieceModels.size(); i++) {
			double t = (double) i / (double) this->pieceModels.size();
			this->pieceModels.at(i)->setOrigin(r * parametricUnitCircle(t));
		}
	}

	void QuartoApp::buildSceneGraph() {
		SceneGraphNode::handle rootNode(new SceneGraphNode());

		SceneGraphNode::handle boardNode(new SceneGraphNode());
		SceneGraphLeaf::handle boardLeaf(new SceneGraphLeaf(this->boardModel));
		boardNode->addChild(boardLeaf);
		rootNode->addChild(boardNode);

		SceneGraphNode::handle piecesNode(new SceneGraphNode());

		for(unsigned int i = 0; i < this->pieceModels.size(); i++) {
			SceneGraphLeaf::handle pieceLeaf(new SceneGraphLeaf(this->pieceModels.at(i)));
			piecesNode->addChild(pieceLeaf);
		}

		rootNode->addChild(piecesNode);
		this->sceneGraph = rootNode;
	}

	PieceModel::handle QuartoApp::getPieceModel(const Piece &piece) {
		for(vector<PieceModel::handle>::const_iterator i = this->pieceModels.begin(); i != this->pieceModels.end(); ++i) {
			if((*i)->isRound() == piece.isRound() &&
					(*i)->isTall() == piece.isTall() &&
					(*i)->isHollow() == piece.isHollow() &&
					(*i)->isWhite() == piece.isLight()) {
				return (*i);
			}
		}

		// No such piece?
		throw 43;
	}

	void QuartoApp::calculateAvailablePieceModels() {
		vector<Piece> availablePieces = this->game.getAvailablePieces();
		this->availablePieceModels.clear();

		for(vector<Piece>::const_iterator i = availablePieces.begin(); i != availablePieces.end(); ++i) {
			this->availablePieceModels.push_back(getPieceModel(*i));
		}
	}

	void QuartoApp::bindKeys() {
		this->engine.bindKey(SDLK_ESCAPE, QE_EXIT);
		this->engine.bindKey(SDLK_UP, QE_MOVE_FORWARD);
		this->engine.bindKey(SDLK_e, QE_MOVE_FORWARD);
		this->engine.bindKey(SDLK_DOWN, QE_MOVE_BACKWARD);
		this->engine.bindKey(SDLK_d, QE_MOVE_BACKWARD);
		this->engine.bindKey(SDLK_LEFT, QE_MOVE_LEFT);
		this->engine.bindKey(SDLK_s, QE_MOVE_LEFT);
		this->engine.bindKey(SDLK_RIGHT, QE_MOVE_RIGHT);
		this->engine.bindKey(SDLK_f, QE_MOVE_RIGHT);
		this->engine.bindKey(SDLK_PAGEUP, QE_MOVE_UP);
		this->engine.bindKey(SDLK_PAGEDOWN, QE_MOVE_DOWN);
		this->engine.bindKey(SDLK_g, QE_TOGGLE_SOLID_GEOMETRY);
		this->engine.bindKey(SDLK_w, QE_TOGGLE_WIREFRAME);
		this->engine.bindKey(SDLK_n, QE_TOGGLE_NORMALS);
		this->engine.bindKey(SDLK_l, QE_TOGGLE_SMOOTH_SHADING);
		this->engine.bindKey(SDLK_LEFTBRACKET, QE_DECREASE_NORMAL_SCALE);
		this->engine.bindKey(SDLK_RIGHTBRACKET, QE_INCREASE_NORMAL_SCALE);
		this->engine.bindKey(SDLK_r, QE_RESET_GAME);
	}

}