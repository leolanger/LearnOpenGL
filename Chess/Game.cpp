#include "Game.h"

//构造函数
Game::Game()
{
	gameplay = new Gameplay();
	board = new Board();
	status = new GameStatus();
	setInitialPieces(PieceColor::WHITE);
	setInitialPieces(PieceColor::BLACK);
	turn = 1;
}

//初始化棋子布局
void Game::setInitialPieces(PieceColor color)
{
	int firstRow = (color == PieceColor::WHITE) ? board->MIN_ROW_INDEX : board->MAX_ROW_INDEX;//最后一行和第一行
	int secondRow = (color == PieceColor::WHITE) ? board->MIN_ROW_INDEX + 1 : board->MAX_ROW_INDEX - 1;//倒数第二行和第二行
	int firstCol = board->MIN_COL_INDEX;
	//根据行和列得到的棋格初始化生成棋子
	/* PAWN */
	for (int col = firstCol; col <= board->MAX_COL_INDEX; col++)
	{
		board->getSquare(secondRow, col)->occupySquare(new Piece(PieceType::PAWN, color));
	}
	/* ROOK */
	board->getSquare(firstRow, firstCol)->occupySquare(new Piece(PieceType::ROOK, color));
	board->getSquare(firstRow, firstCol + 7)->occupySquare(new Piece(PieceType::ROOK, color));
	/* KNIGHT */
	board->getSquare(firstRow, firstCol + 1)->occupySquare(new Piece(PieceType::KNIGHT, color));
	board->getSquare(firstRow, firstCol + 6)->occupySquare(new Piece(PieceType::KNIGHT, color));
	/* BISHOP */
	board->getSquare(firstRow, firstCol + 2)->occupySquare(new Piece(PieceType::BISHOP, color));
	board->getSquare(firstRow, firstCol + 5)->occupySquare(new Piece(PieceType::BISHOP, color));
	/* QUEEN */
	board->getSquare(firstRow, firstCol + 3)->occupySquare(new Piece(PieceType::QUEEN, color));
	/* KING */
	board->getSquare(firstRow, firstCol + 4)->occupySquare(new Piece(PieceType::KING, color));
}

//获取记录
std::stack<Move> Game::getAllLog()
{
	return log;
}

//获取棋盘
Board* Game::getBoard()
{
	return board;
}

//获取当前游戏状态
GameStatus* Game::getGameStatus()
{
	return status;
}

//获取可移动至的位置
std::vector<Move> Game::getValidMoves(int fromRow, int fromCol)
{
	return gameplay->getValidMoves(status, board, board->getSquare(fromRow, fromCol)->getOccupyingPiece(), fromRow, fromCol);
}

//根据可移动至的位置判断能否移动至该位置
bool Game::move(int fromRow, int fromCol, int toRow, int toCol)
{
	std::vector<Move> valid = getValidMoves(fromRow, fromCol);
	int sz = valid.size();
	for (int index = 0; index < sz; index++)
		if (valid[index].getDestinationPosition().first == toRow && valid[index].getDestinationPosition().second == toCol)
			if (gameplay->move(status, board, valid[index]))
			{
				log.push(valid[index]);
				return true;
			}

	return false;
}

//获取该位置的棋子的颜色
PieceColor Game::getPieceColor(int fromRow, int fromCol)
{
	return board->getSquare(fromRow, fromCol)->getOccupyingPiece()->getColor();
}

//获取该位置的棋子
Piece* Game::getPiece(int fromRow, int fromCol)
{
	return board->getSquare(fromRow, fromCol)->getOccupyingPiece();
}

//判断该位置是否有棋子
bool Game::isSquareOccupied(int fromRow, int fromCol)
{
	return board->getSquare(fromRow, fromCol)->occupiedState();
}

//判断是否在将军状态
bool Game::inCheckState()
{
	return gameplay->isCheckState(status, board, getTurnColor());
}

//判断是否处于分出胜负状态
bool Game::inCheckMateState()
{
	return gameplay->isCheckMateState(status, board, getTurnColor());
}

//获取当前回合数
int Game::getTurn()
{
	return turn;
}

//到下一回合
void Game::nextTurn()
{
	turn++;
	status->setPieceEnPassantable(getTurnColor(), NULL);
}

//获取当前回合的棋子颜色
PieceColor Game::getTurnColor()
{
	return turn & 1 ? PieceColor::WHITE : PieceColor::BLACK;
}

//判断是否晋升
bool Game::promote(int row, int col, PieceType type)
{
	return gameplay->pawnPromotion(board, row, col, type);
}
