/**
 * @file Game.hpp
 */
#pragma once

#include "Piece.hpp"
#include "Board.hpp"
#include <vector>

using std::vector;

namespace quarto {

	enum State { NOT_STARTED, P1_CHOOSE, P2_CHOOSE, P1_PLACE, P2_PLACE, P1_WIN, P2_WIN };

	class Game {
	public:

		Game();
		
		void start();
		void choosePiece(const Piece &piece);
		void placePiece(unsigned int i, unsigned int j);
		void printStateMessage();
		void reset();

		State getState() const;
		vector<Piece> getAvailablePieces() const;
		Piece getChosenPiece() const;

	private:

		State state;
		Piece chosenPiece;
		vector<Piece> availablePieces;
		Board board;

	};

}
