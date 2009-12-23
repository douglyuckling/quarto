/**
 * @file Board.hpp
 */
#pragma once

#include "Piece.hpp"
#include <boost/multi_array.hpp>

using boost::multi_array;

namespace quarto {

	class Board {
		typedef multi_array<byte, 2> board_type;
		typedef board_type::index board_index;

	public:

		Board();

		void clear();

		bool placePiece(const Piece &piece, board_index row, board_index col);

	private:

		board_type space;

		bool isWinningConfiguration() const;

	};

}
