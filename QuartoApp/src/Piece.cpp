/**
* @file Piece.cpp
*/
#include "Piece.hpp"

namespace quarto {

	Piece::Piece(bool isRound, bool isTall, bool isHollow, bool isLight)
		: description((isRound ? ROUND : SQUARE) |
					  (isTall ? TALL : SHORT) |
					  (isHollow ? HOLLOW : SOLID) |
					  (isLight ? LIGHT : DARK)) {
	}

	Piece::Piece(byte description) : description(description) {
		validateDescription(description);
	}

	void Piece::validateDescription(byte description) {
		if (description & SQUARE && description & ROUND) {
			// Both shapes were specified
			throw 13;
		}
		if (!(description & SQUARE || description & ROUND)) {
			// Neither shape was specified
			throw 14;
		}
		if (description & TALL && description & SHORT) {
			// Both heights were specified
			throw 15;
		}
		if (!(description & TALL || description & SHORT)) {
			// Neither height was specified
			throw 16;
		}
		if (description & SOLID && description & HOLLOW) {
			// Both fills were specified
			throw 17;
		}
		if (!(description & SOLID || description & HOLLOW)) {
			// Neither fill was specified
			throw 18;
		}
		if (description & DARK && description & LIGHT) {
			// Both shades were specified
			throw 19;
		}
		if (!(description & DARK || description & LIGHT)) {
			// Neither shade was specified
			throw 20;
		}
	}

}
