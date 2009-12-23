/**
* @file Board.cpp
*/
#include "Board.hpp"

using boost::extents;

namespace quarto {

	Board::Board() : space(boost::extents[4][4]) {
		clear();
	}

	void Board::clear() {
		for(board_index i = 0; i < 4; i++) {
			for(board_index j = 0; j < 4; j++) {
				space[i][j] = 0x00;
			}
		}
	}

	/**
	 * \return true if the new board configuration is a win, false otherwise
	 */
	bool Board::placePiece(const Piece &piece, board_index row, board_index col) {
		space[row][col] = piece.getInfo();

		return isWinningConfiguration();
	}

	/**
	 * \return true if the new board configuration is a win, false otherwise
	 */
	bool Board::isWinningConfiguration() const {
		// Check each row
		for(board_index i = 0; i < 4; i++) {
			if(space[i][0] & space[i][1] & space[i][2] & space[i][3])
				return true;
		}
		
		// Check each column
		for(board_index i = 0; i < 4; i++) {
			if(space[0][i] & space[1][i] & space[2][i] & space[3][i])
				return true;
		}

		// Check the diagonals
		if(space[0][0] & space[1][1] & space[2][2] & space[3][3])
			return true;
		
		if(space[0][3] & space[1][2] & space[2][1] & space[3][0])
			return true;

		return false;
	}

}
