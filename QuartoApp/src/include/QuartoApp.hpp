/**
* @file QuartoApp.hpp
*/
#pragma once

#include "BoardModel.hpp"
#include "PieceModel.hpp"
#include "MarkerModel.hpp"
#include "Game.hpp"
#include <PerspectiveCamera.hpp>
#include <FirstPersonCameraRigging.hpp>
#include <Model.hpp>
#include <Drawable.hpp>
#include <CustomEventHandler.hpp>
#include <MouseButtonEventHandler.hpp>
#include <MouseMotionEventHandler.hpp>
#include <ResizeEventHandler.hpp>
#include <Engine.hpp>
#include <SceneGraphNode.hpp>
#include <SceneGraphLeaf.hpp>

using std::vector;
using peek::PerspectiveCamera;
using peek::FirstPersonCameraRigging;
using peek::Model;
using peek::Drawable;
using peek::SceneGraphNodeBase;
using peek::CustomEventHandler;
using peek::MouseButtonEventHandler;
using peek::MouseMotionEventHandler;
using peek::ResizeEventHandler;
using peek::Engine;

namespace quarto {

	/**
	* @brief The Quarto application
	*/
	class QuartoApp : public Drawable, public CustomEventHandler, public MouseButtonEventHandler, public MouseMotionEventHandler, public ResizeEventHandler {
	public:

		/** Constructs an instance of the application from the filename of a model to load */
		QuartoApp();

		/** Runs the application */
		void run();

		/** Draws the scene */
		virtual void draw();

		/** Picks objects in the scene */
		void pickPiece(int x, int y);

		/** Picks objects in the scene */
		void pickMarker(int x, int y);

		/** Handles a custom event */
		void handleCustomEvent(int customEvent);

		/** Handles a mouse button event */
		virtual void handleMouseButtonEvent(const SDL_MouseButtonEvent &buttonEvent);

		/** Handles a mouse motion event */
		virtual void handleMouseMotionEvent(const SDL_MouseMotionEvent &motionEvent);

		/** Handles a window-resize event */
		virtual void handleResizeEvent(int sizeX, int sizeY);

	protected:

		/** The game engine */
		Engine engine;

		/** The game instance */
		Game game;

		/** The board model */
		BoardModel::handle boardModel;

		/** The piece models */
		PieceModel::list pieceModels;

		/** The piece models available for selection */
		PieceModel::list availablePieceModels;

		/** The scene graph */
		SceneGraphNodeBase::handle sceneGraph;

		/** The marker model */
		MarkerModel markerModel;

		/** The camera to view the scene with */
		FirstPersonCameraRigging::handle cameraRigging;

		/** Whether the left mouse button is depressed */
		bool leftMouseButtonDown;

		/** Whether the right mouse button is depressed */
		bool rightMouseButtonDown;

		static const int BUFSIZE = 512;
		GLuint selectBuf[BUFSIZE];

		void generateModels();

		void placeModels();

		void buildSceneGraph();

		PieceModel::handle getPieceModel(const Piece &piece);

		void calculateAvailablePieceModels();

		void bindKeys();

		void restartGame();

	};

	// Custom Quarto events...
	static const int QE_NONE = 0;
	static const int QE_EXIT = 1;
	static const int QE_MOVE_FORWARD = 2;
	static const int QE_MOVE_BACKWARD = 3;
	static const int QE_MOVE_LEFT = 4;
	static const int QE_MOVE_RIGHT = 5;
	static const int QE_MOVE_UP = 6;
	static const int QE_MOVE_DOWN = 7;
	static const int QE_TOGGLE_SOLID_GEOMETRY = 8;
	static const int QE_TOGGLE_WIREFRAME = 9;
	static const int QE_TOGGLE_NORMALS = 10;
	static const int QE_TOGGLE_SMOOTH_SHADING = 11;
	static const int QE_DECREASE_NORMAL_SCALE = 12;
	static const int QE_INCREASE_NORMAL_SCALE = 13;
	static const int QE_RESET_GAME = 14;

}