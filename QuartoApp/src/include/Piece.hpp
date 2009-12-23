/**
 * @file Piece.hpp
 */
#pragma once

typedef unsigned char byte;

namespace quarto {

	const byte SQUARE = 0x40;
	const byte ROUND  = 0x80;
	const byte SHORT  = 0x10;
	const byte TALL   = 0x20;
	const byte SOLID  = 0x04;
	const byte HOLLOW = 0x08;
	const byte DARK   = 0x01;
	const byte LIGHT  = 0x02;

	class Piece {
	public:

		Piece(bool isRound, bool isTall, bool isHollow, bool isLight);
		
		Piece(byte description);

		inline byte getInfo() const { return description; }

		inline bool isRound() const { return (description & ROUND) != 0; }
		inline bool isTall() const { return (description & TALL) != 0; }
		inline bool isHollow() const { return (description & HOLLOW) != 0; }
		inline bool isLight() const { return (description & LIGHT) != 0; }

		inline bool operator==(const Piece &piece) const { return description == piece.description; }

	private:

		byte description;

		void validateDescription(byte description);

	};

}
