#include "Move.h"

//���캯��
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

//��ȡ�ƶ�ǰ��λ��
std::pair<int, int> Move::getOriginPosition()
{
	return std::make_pair(fromRow, fromCol);
}

//��ȡ�ƶ����λ��
std::pair<int, int> Move::getDestinationPosition()
{
	return std::make_pair(toRow, toCol);
}

//��ȡ�ƶ�������
Piece* Move::getMovedPiece()
{
	return movePiece;
}

//��ȡ�Ե�������
Piece* Move::getCapturedPiece()
{
	return capturedPiece;
}

//��ȡ�ƶ�������
MoveType Move::getType()
{
	return type;
}