#include <cstddef>

#include "GameStatus.h"

//构造函数
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

//若王有移动，则设置
void GameStatus::setKingMove(PieceColor color)
{
	if (color == PieceColor::WHITE) whiteKingMove = true;
	else if (color == PieceColor::BLACK) blackKingMove = true;
}

//若形成了吃过路兵的局面，将其对应的颜色的该状态置为真
void GameStatus::setPieceEnPassantable(PieceColor color, Piece* piece)
{
	if (color == PieceColor::WHITE) whitePieceEnPassant = piece;
	else if (color == PieceColor::BLACK) blackPieceEnPassant = piece;
}

//若第一列城堡有移动，置为真
void GameStatus::setFirstColRookMove(PieceColor color)
{
	if (color == PieceColor::WHITE) whiteFirstColRookMove = true;
	else if (color == PieceColor::BLACK) blackFirstColRookMove = true;
}

//若最后一列城堡有移动，置为真
void GameStatus::setLastColRookMove(PieceColor color)
{
	if (color == PieceColor::WHITE) whiteLastColRookMove = true;
	else if (color == PieceColor::BLACK) blackLastColRookMove = true;
}

//判断王是否移动
bool GameStatus::isKingMove(PieceColor color)
{
	if (color == PieceColor::WHITE) return whiteKingMove;
	else if (color == PieceColor::BLACK) return blackKingMove;
}

//判断是否形成过路兵，若形成返回该棋子
Piece* GameStatus::pieceEnPassantable(PieceColor color)
{
	if (color == PieceColor::WHITE) return whitePieceEnPassant;
	else if (color == PieceColor::BLACK) return blackPieceEnPassant;
}

//判断第一列城堡是否有移动
bool GameStatus::isFirstColRookMove(PieceColor color)
{
	if (color == PieceColor::WHITE) return whiteFirstColRookMove;
	else if (color == PieceColor::BLACK) return blackFirstColRookMove;
}

//判断最后一列城堡是否有移动
bool GameStatus::isLastColRookMove(PieceColor color)
{
	if (color == PieceColor::WHITE) return whiteLastColRookMove;
	else if (color == PieceColor::BLACK) return blackLastColRookMove;
}
