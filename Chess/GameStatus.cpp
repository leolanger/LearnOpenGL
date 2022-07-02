#include <cstddef>

#include "GameStatus.h"

//���캯��
GameStatus::GameStatus()
{
	whiteKingMove = false;
	blackKingMove = false;
	whitePieceEnPassant = NULL;
	blackPieceEnPassant = NULL;
	whiteFirstColRookMove = false;
	whiteLastColRookMove = false;
	blackFirstColRookMove = false;
	blackLastColRookMove = false;
}

GameStatus::GameStatus(const GameStatus& gameStatus)
{
	this->whiteKingMove = gameStatus.whiteKingMove;
	this->blackKingMove = gameStatus.blackKingMove;
	this->whitePieceEnPassant = gameStatus.whitePieceEnPassant;
	this->blackPieceEnPassant = gameStatus.blackPieceEnPassant;
	this->whiteFirstColRookMove = gameStatus.whiteFirstColRookMove;
	this->whiteLastColRookMove = gameStatus.whiteLastColRookMove;
	this->blackFirstColRookMove = gameStatus.blackFirstColRookMove;
	this->blackLastColRookMove = gameStatus.blackLastColRookMove;
}

//�������ƶ���������
void GameStatus::setKingMove(PieceColor color)
{
	if (color == PieceColor::WHITE) whiteKingMove = true;
	else if (color == PieceColor::BLACK) blackKingMove = true;
}

//���γ��˳Թ�·���ľ��棬�����Ӧ����ɫ�ĸ�״̬��Ϊ��
void GameStatus::setPieceEnPassantable(PieceColor color, Piece* piece)
{
	if (color == PieceColor::WHITE) whitePieceEnPassant = piece;
	else if (color == PieceColor::BLACK) blackPieceEnPassant = piece;
}

//����һ�гǱ����ƶ�����Ϊ��
void GameStatus::setFirstColRookMove(PieceColor color)
{
	if (color == PieceColor::WHITE) whiteFirstColRookMove = true;
	else if (color == PieceColor::BLACK) blackFirstColRookMove = true;
}

//�����һ�гǱ����ƶ�����Ϊ��
void GameStatus::setLastColRookMove(PieceColor color)
{
	if (color == PieceColor::WHITE) whiteLastColRookMove = true;
	else if (color == PieceColor::BLACK) blackLastColRookMove = true;
}

//�ж����Ƿ��ƶ�
bool GameStatus::isKingMove(PieceColor color)
{
	if (color == PieceColor::WHITE) return whiteKingMove;
	else if (color == PieceColor::BLACK) return blackKingMove;
}

//�ж��Ƿ��γɹ�·�������γɷ��ظ�����
Piece* GameStatus::pieceEnPassantable(PieceColor color)
{
	if (color == PieceColor::WHITE) return whitePieceEnPassant;
	else if (color == PieceColor::BLACK) return blackPieceEnPassant;
}

//�жϵ�һ�гǱ��Ƿ����ƶ�
bool GameStatus::isFirstColRookMove(PieceColor color)
{
	if (color == PieceColor::WHITE) return whiteFirstColRookMove;
	else if (color == PieceColor::BLACK) return blackFirstColRookMove;
}

//�ж����һ�гǱ��Ƿ����ƶ�
bool GameStatus::isLastColRookMove(PieceColor color)
{
	if (color == PieceColor::WHITE) return whiteLastColRookMove;
	else if (color == PieceColor::BLACK) return blackLastColRookMove;
}
