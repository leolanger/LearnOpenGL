#include "Board.h"

Board::Board()
{
}

//构造函数
Board::Board(const Board& board)
{
	for (int row = 0; row < 10; row++)
	{
		for (int col = 0; col < 10; col++)
		{
			this->squares[row][col] = board.squares[row][col];
		}
	}
}

//获取棋格
Square* Board::getSquare(int row, int col)
{
	return &squares[row][col];
}
