#include "Piece.h"

//构造函数
Piece::Piece(PieceType pType, PieceColor pColor)
{
	type = pType;
	color = pColor;
}

Piece::Piece(const Piece& piece)
{
	this->type = piece.type;
	this->color = piece.color;
}

//获取棋子的类型
PieceType Piece::getType()
{
	return type;
}

//获取棋子的颜色
PieceColor Piece::getColor()
{
	return color;
}