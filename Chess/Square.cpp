#include <cstddef>

#include "Square.h"

//构造函数
Square::Square()
{
	occupyingPiece = NULL;
}

Square::Square(const Square& square)
{
	this->occupyingPiece = square.occupyingPiece;
}

//判断该棋格之前是否有棋子
bool Square::occupiedState()
{
	return occupyingPiece == NULL ? false : true;
}

//先判断之前是否有棋子，若没有则设置为该棋子并返回真，否则返回假
bool Square::occupySquare(Piece* piece)
{
	if (occupiedState())
		return false;
	else 
		occupyingPiece = piece;
	return true;
}

//记录之前的棋子，并把它移除
Piece* Square::removeOccupyingPiece()
{
	Piece* removed = occupyingPiece;
	occupyingPiece = NULL;
	return removed;
}

//获取当前的棋子
Piece* Square::getOccupyingPiece()
{
	return occupyingPiece;
}