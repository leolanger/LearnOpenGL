#include <cstddef>

#include "Square.h"

//���캯��
Square::Square()
{
	occupyingPiece = NULL;
}

Square::Square(const Square& square)
{
	this->occupyingPiece = square.occupyingPiece;
}

//�жϸ����֮ǰ�Ƿ�������
bool Square::occupiedState()
{
	return occupyingPiece == NULL ? false : true;
}

//���ж�֮ǰ�Ƿ������ӣ���û��������Ϊ�����Ӳ������棬���򷵻ؼ�
bool Square::occupySquare(Piece* piece)
{
	if (occupiedState())
		return false;
	else 
		occupyingPiece = piece;
	return true;
}

//��¼֮ǰ�����ӣ��������Ƴ�
Piece* Square::removeOccupyingPiece()
{
	Piece* removed = occupyingPiece;
	occupyingPiece = NULL;
	return removed;
}

//��ȡ��ǰ������
Piece* Square::getOccupyingPiece()
{
	return occupyingPiece;
}