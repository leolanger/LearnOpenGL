#include "Piece.h"

//���캯��
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

//��ȡ���ӵ�����
PieceType Piece::getType()
{
	return type;
}

//��ȡ���ӵ���ɫ
PieceColor Piece::getColor()
{
	return color;
}