#include "Move.h"

//构造函数
Move::Move(MoveType mType, int fRow, int fCol, int tRow, int tCol, Piece* moved, Piece* captured)
{
	type = mType;
	fromRow = fRow;
	fromCol = fCol;
	toRow = tRow;
	toCol = tCol;
	movePiece = moved;
	capturedPiece = captured;
}

Move::Move(const Move& move)
{
	this->type = move.type;
	this->fromRow = move.fromRow;
	this->fromCol = move.fromCol;
	this->toRow = move.toRow;
	this->toCol = move.toCol;
	this->movePiece = move.movePiece;
	this->capturedPiece = move.capturedPiece;
}

//获取移动前的位置
std::pair<int, int> Move::getOriginPosition()
{
	return std::make_pair(fromRow, fromCol);
}

//获取移动后的位置
std::pair<int, int> Move::getDestinationPosition()
{
	return std::make_pair(toRow, toCol);
}

//获取移动的棋子
Piece* Move::getMovedPiece()
{
	return movePiece;
}

//获取吃掉的棋子
Piece* Move::getCapturedPiece()
{
	return capturedPiece;
}

//获取移动的类型
MoveType Move::getType()
{
	return type;
}