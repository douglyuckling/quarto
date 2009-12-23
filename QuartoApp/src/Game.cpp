/**
* @file Game.cpp
*/
#include "Game.hpp"
#include <iostream>

using std::cout;
using std::endl;

namespace quarto {

	Game::Game() : chosenPiece(Piece(ROUND|TALL|HOLLOW|LIGHT)) {
		reset();
	}

	State Game::getState() const {
		return this->state;
	}

	vector<Piece> Game::getAvailablePieces() const {
		return this->availablePieces;
	}

	Piece Game::getChosenPiece() const {
		return this->chosenPiece;
	}

	void Game::start() {
		switch(this->state) {
			case NOT_STARTED:
				this->state = P1_CHOOSE;
				break;
			default:
				// Illegal state.
				throw 43;
		}
	}

	void Game::choosePiece(const Piece &piece) {
		this->chosenPiece = piece;

		bool found = false;
		for(vector<Piece>::const_iterator i = this->availablePieces.begin(); i != this->availablePieces.end(); ++i) {
			if(*i == this->chosenPiece) {
				this->availablePieces.erase(i);
				found = true;
				break;
			}
		}

		if(found == false) {
			// Illegal choice.
			throw 44;
		}

		switch(this->state) {
			case P1_CHOOSE:
				this->state = P2_PLACE;
				break;
			case P2_CHOOSE:
				this->state = P1_PLACE;
				break;
			default:
				// Illegal state.
				throw 43;
		}
	}

	void Game::placePiece(unsigned int i, unsigned int j) {
		bool win = this->board.placePiece(this->chosenPiece, i, j);

		switch(this->state) {
			case P1_PLACE:
				this->state = (win ? P1_WIN : P1_CHOOSE);
				break;
			case P2_PLACE:
				this->state = (win ? P2_WIN : P2_CHOOSE);
				break;
			default:
				// Illegal state.
				throw 43;
		}
	}

	void Game::printStateMessage() {
		switch(this->state) {
			case NOT_STARTED:
				cout << "Welcome to quarto!" << endl;
				break;
			case P1_CHOOSE:
				cout << "Player 1, choose a piece for player 2 to place." << endl;
				break;
			case P2_CHOOSE:
				cout << "Player 2, choose a piece for player 1 to place." << endl;
				break;
			case P1_PLACE:
				cout << "Player 1, place the piece." << endl;
				break;
			case P2_PLACE:
				cout << "Player 2, place the piece." << endl;
				break;
			case P1_WIN:
				cout << "Player 1 wins!" << endl;
				break;
			case P2_WIN:
				cout << "Player 2 wins!" << endl;
				break;
			default:
				throw 42;
		}
	}

	void Game::reset() {
		this->state = NOT_STARTED;
		this->board.clear();

		this->availablePieces.clear();
		this->availablePieces.push_back(Piece(ROUND|TALL|HOLLOW|LIGHT));
		this->availablePieces.push_back(Piece(SQUARE|TALL|HOLLOW|LIGHT));
		this->availablePieces.push_back(Piece(ROUND|SHORT|HOLLOW|LIGHT));
		this->availablePieces.push_back(Piece(SQUARE|SHORT|HOLLOW|LIGHT));
		this->availablePieces.push_back(Piece(ROUND|TALL|SOLID|LIGHT));
		this->availablePieces.push_back(Piece(SQUARE|TALL|SOLID|LIGHT));
		this->availablePieces.push_back(Piece(ROUND|SHORT|SOLID|LIGHT));
		this->availablePieces.push_back(Piece(SQUARE|SHORT|SOLID|LIGHT));
		this->availablePieces.push_back(Piece(ROUND|TALL|HOLLOW|DARK));
		this->availablePieces.push_back(Piece(SQUARE|TALL|HOLLOW|DARK));
		this->availablePieces.push_back(Piece(ROUND|SHORT|HOLLOW|DARK));
		this->availablePieces.push_back(Piece(SQUARE|SHORT|HOLLOW|DARK));
		this->availablePieces.push_back(Piece(ROUND|TALL|SOLID|DARK));
		this->availablePieces.push_back(Piece(SQUARE|TALL|SOLID|DARK));
		this->availablePieces.push_back(Piece(ROUND|SHORT|SOLID|DARK));
		this->availablePieces.push_back(Piece(SQUARE|SHORT|SOLID|DARK));
	}

}
