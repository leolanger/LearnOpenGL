#include <cstddef>
#include <cmath>
#include <iostream>

#include "Gameplay.h"

//移动棋子
bool Gameplay::move(GameStatus* status, Board* board, Move move)
{
	Piece* actPiece = move.getMovedPiece();//获取移动的棋子
	//获取出发格子
	int fromRow = move.getOriginPosition().first;
	int fromCol = move.getOriginPosition().second;
	//获取目的格子
	int toRow = move.getDestinationPosition().first;
	int toCol = move.getDestinationPosition().second;
	//根据移动的类型来进行不同的操作
	switch (move.getType())
	{
	//若只是普通的移动类型
	case MoveType::NORMAL:
	{
		//若棋子是士兵且移动了两格，置为过路兵状态
		if (actPiece->getType() == PieceType::PAWN && std::abs(toRow - fromRow) == 2)
			status->setPieceEnPassantable(actPiece->getColor(), actPiece);
		//若棋子是王，置王移动状态
		else if (actPiece->getType() == PieceType::KING)
			status->setKingMove(actPiece->getColor());
		//若棋子是城堡，置城堡移动状态
		else if (actPiece->getType() == PieceType::ROOK)
		{
			if (fromCol == board->MIN_COL_INDEX) status->setFirstColRookMove(actPiece->getColor());
			else if (fromCol == board->MAX_COL_INDEX) status->setLastColRookMove(actPiece->getColor());
		}
		//返回是否成功占有目标格子
		return board->getSquare(toRow, toCol)->occupySquare(board->getSquare(fromRow, fromCol)->removeOccupyingPiece());
	}
	//若是能俘获对方棋子类型
	case MoveType::CAPTURE:
	{
		//若棋子是王，置王移动状态
		if (actPiece->getType() == PieceType::KING)
			status->setKingMove(actPiece->getColor());
		//若棋子是城堡，置城堡移动状态
		else if (actPiece->getType() == PieceType::ROOK)
		{
			if (fromCol == board->MIN_COL_INDEX) status->setFirstColRookMove(actPiece->getColor());
			else if (fromCol == board->MAX_COL_INDEX) status->setLastColRookMove(actPiece->getColor());
		}
		//移出目标格子的棋子
		board->getSquare(toRow, toCol)->removeOccupyingPiece();
		//返回是否成功占有目标格子
		return board->getSquare(toRow, toCol)->occupySquare(board->getSquare(fromRow, fromCol)->removeOccupyingPiece());
	}
	//若为吃过路兵类型
	case MoveType::EN_PASSANT:
	{
		//移出目标格子的棋子
		board->getSquare(fromRow, toCol)->removeOccupyingPiece();
		//返回是否成功占有目标格子
		return board->getSquare(toRow, toCol)->occupySquare(board->getSquare(fromRow, fromCol)->removeOccupyingPiece());
	}
	//若为王车易位类型
	case MoveType::CASTLING:
	{
		//设置王车易位城堡初始和目的列
		int rookOriginCol = ((toCol < fromCol) ? board->MIN_COL_INDEX : board->MAX_COL_INDEX);
		int rookDestCol = ((toCol < fromCol) ? fromCol - 1 : fromCol + 1);
		//将原有目的地和王车易位后目的地分别用其原本的棋子占有
		board->getSquare(toRow, toCol)->occupySquare(board->getSquare(fromRow, fromCol)->removeOccupyingPiece());
		board->getSquare(toRow, rookDestCol)->occupySquare(board->getSquare(fromRow, rookOriginCol)->removeOccupyingPiece());
		//置王移动状态
		status->setKingMove(actPiece->getColor());
		//判断出是哪个城堡后，置城堡移动状态
		rookOriginCol > rookDestCol ? status->setLastColRookMove(actPiece->getColor()) : status->setFirstColRookMove(actPiece->getColor());
		return true;
	}
	default: break;
	}
	return false;
}

//获取可能的移动
std::vector<Move> Gameplay::getPossibleMoves(GameStatus* status, Board* board, Piece* piece, int fromRow, int fromCol)
{
	std::vector<Move> possibleMove;
	int row, col;

	//出现异常情况返回
	if (status == NULL || board == NULL || piece == NULL || fromRow < board->MIN_ROW_INDEX || fromRow > board->MAX_ROW_INDEX || fromCol < board->MIN_COL_INDEX || fromCol > board->MAX_COL_INDEX) 
		return possibleMove;

	//根据棋子的类型分别操作
	switch (piece->getType())
	{
	//棋子为士兵
	case PieceType::PAWN:
	{
		//如果是白色的
		if (piece->getColor() == PieceColor::WHITE)
		{
			/*向前移动一格*/

			row = fromRow + 1;
			col = fromCol;
			//还在棋盘里
			if (row <= board->MAX_ROW_INDEX)
			{
				//把移动类型，初始点，目的地，棋子压入vector
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
				//如果目的地有棋子，弹出
				if (board->getSquare(row, col)->occupiedState()) 
					possibleMove.pop_back();
			}

			//如果能向前移动一个且在初始位置，可以移动两格
			if (possibleMove.size() && fromRow == board->MIN_ROW_INDEX + 1)
			{
				row = fromRow + 2;
				col = fromCol;
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
				if (board->getSquare(row, col)->occupiedState()) possibleMove.pop_back();
			}

			/*斜方向俘获类型*/
			row = fromRow + 1;
			col = fromCol - 1;
			//如果目标在棋盘内
			if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
			{
				//如果目标位置上有棋子
				if (board->getSquare(row, col)->occupiedState())
				{
					//如果目标位置上的棋子不是本方棋子
					if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
						//把移动类型，初始点，目的地，棋子，以及俘获的棋子压入vector
						possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				}
			}

			row = fromRow + 1;
			col = fromCol + 1;
			if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
			{
				if (board->getSquare(row, col)->occupiedState())
				{
					if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
						possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				}
			}

			/*
				吃过路兵类型
			*/
			if (fromRow == board->MIN_ROW_INDEX + 4)
			{
				row = fromRow + 1;
				col = fromCol - 1;
				//若目标在棋盘内，且该位置上没有棋子
				if (col >= board->MIN_COL_INDEX && !board->getSquare(row, col)->occupiedState())
				{
					//若本棋子同行临近位置上有棋子
					if (board->getSquare(fromRow, col)->occupiedState())
					{
						//获取该棋子
						Piece* capPossibility = board->getSquare(fromRow, col)->getOccupyingPiece();
						//若同为士兵且不是本方棋子且是过路兵状态
						if (capPossibility->getType() == PieceType::PAWN && capPossibility->getColor() != piece->getColor() && status->pieceEnPassantable(capPossibility->getColor()) == capPossibility)
							possibleMove.push_back(Move(MoveType::EN_PASSANT, fromRow, fromCol, row, col, piece, capPossibility));
					}
				}

				col = fromCol + 1;
				if (col <= board->MAX_COL_INDEX && !board->getSquare(row, col)->occupiedState())
				{
					if (board->getSquare(fromRow, col)->occupiedState())
					{
						Piece* capPossibility = board->getSquare(fromRow, col)->getOccupyingPiece();
						if (capPossibility->getType() == PieceType::PAWN && capPossibility->getColor() != piece->getColor() && status->pieceEnPassantable(capPossibility->getColor()) == capPossibility)
							possibleMove.push_back(Move(MoveType::EN_PASSANT, fromRow, fromCol, row, col, piece, capPossibility));
					}
				}
			}
		}

		//如果棋子是黑色的
		else if (piece->getColor() == PieceColor::BLACK)
		{
			/*
				SINGLE MOVE
			*/
			row = fromRow - 1;
			col = fromCol;
			if (row >= board->MIN_ROW_INDEX)
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
				if (board->getSquare(row, col)->occupiedState()) possibleMove.pop_back();
			}

			/*
				DOUBLE MOVE
			*/
			if (possibleMove.size() && fromRow == board->MAX_ROW_INDEX - 1)
			{
				row = fromRow - 2;
				col = fromCol;
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
				if (board->getSquare(row, col)->occupiedState()) possibleMove.pop_back();
			}

			/*
				NORMAL CAPTURE
			*/
			row = fromRow - 1;
			col = fromCol - 1;
			if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
			{
				if (board->getSquare(row, col)->occupiedState())
				{
					if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
						possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				}
			}

			row = fromRow - 1;
			col = fromCol + 1;
			if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
			{
				if (board->getSquare(row, col)->occupiedState())
				{
					if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
						possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				}
			}

			/*
				EN PASSANT
			*/
			if (fromRow == board->MAX_ROW_INDEX - 4)
			{
				row = fromRow - 1;
				col = fromCol - 1;
				if (col >= board->MIN_COL_INDEX && !board->getSquare(row, col)->occupiedState())
				{
					if (board->getSquare(fromRow, col)->occupiedState())
					{
						Piece* capPossibility = board->getSquare(fromRow, col)->getOccupyingPiece();
						if (capPossibility->getType() == PieceType::PAWN && capPossibility->getColor() != piece->getColor() && status->pieceEnPassantable(capPossibility->getColor()) == capPossibility)
							possibleMove.push_back(Move(MoveType::EN_PASSANT, fromRow, fromCol, row, col, piece, capPossibility));
					}
				}

				col = fromCol + 1;
				if (col <= board->MAX_COL_INDEX && !board->getSquare(row, col)->occupiedState())
				{
					if (board->getSquare(fromRow, col)->occupiedState())
					{
						Piece* capPossibility = board->getSquare(fromRow, col)->getOccupyingPiece();
						if (capPossibility->getType() == PieceType::PAWN && capPossibility->getColor() != piece->getColor() && status->pieceEnPassantable(capPossibility->getColor()) == capPossibility)
							possibleMove.push_back(Move(MoveType::EN_PASSANT, fromRow, fromCol, row, col, piece, capPossibility));
					}
				}
			}
		}

		break;
	}
	//棋子是城堡类型
	case PieceType::ROOK:
	{
		/*
			该棋子向前移动
		*/
		row = fromRow - 1;
		col = fromCol;
		//只要还在棋盘内
		while (row >= board->MIN_ROW_INDEX)
		{
			//目标位置上没有棋子
			if (!board->getSquare(row, col)->occupiedState())
			{
				//把移动类型，初始点，目的地，棋子压入vector
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			//若有棋子
			else
			{
				//棋子不是本方的
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					//把移动类型，初始点，目的地，棋子，可俘获棋子压入vector
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			//递减
			row--;
		}

		/*
			棋子向后移动
		*/
		row = fromRow + 1;
		col = fromCol;
		while (row <= board->MAX_ROW_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			row++;
		}

		/*
			棋子向左移动
		*/
		row = fromRow;
		col = fromCol - 1;
		while (col >= board->MIN_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			col--;
		}

		/*
			棋子向右移动
		*/
		row = fromRow;
		col = fromCol + 1;
		while (col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			col++;
		}
		break;
	}
	//棋子为主教类型
	case PieceType::BISHOP:
	{
		/*
			斜向左上
		*/
		row = fromRow + 1;
		col = fromCol - 1;
		//目标还在棋盘内
		while (row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX)
		{
			//目标位置上没有棋子
			if (!board->getSquare(row, col)->occupiedState())
			{
				//把移动类型，初始点，目的地，棋子压入vector
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			//若有棋子
			else
			{
				//棋子不是本方的
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					//把移动类型，初始点，目的地，棋子，可俘获棋子压入vector
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			row++;
			col--;
		}

		row = fromRow + 1;
		col = fromCol + 1;
		while (row <= board->MAX_ROW_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			row++;
			col++;
		}

		row = fromRow - 1;
		col = fromCol + 1;
		while (row >= board->MIN_ROW_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			row--;
			col++;
		}

		row = fromRow - 1;
		col = fromCol - 1;
		while (row >= board->MIN_ROW_INDEX && col >= board->MIN_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			row--;
			col--;
		}

		break;
	}
	//棋子是骑士类型
	case PieceType::KNIGHT:
	{
		/*
			日字走
		*/
		row = fromRow - 2;
		col = fromCol - 1;
		//若目标位置还在棋盘内
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			//目标位置上没有棋子
			if (!board->getSquare(row, col)->occupiedState())
			{
				//把移动类型，初始点，目的地，棋子压入vector
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			//若有
			else
			{
				//不是本方的
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					//把移动类型，初始点，目的地，棋子，可俘获棋子压入vector
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow - 1;
		col = fromCol - 2;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow + 1;
		col = fromCol - 2;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow + 2;
		col = fromCol - 1;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow + 2;
		col = fromCol + 1;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow + 1;
		col = fromCol + 2;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow - 1;
		col = fromCol + 2;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow - 2;
		col = fromCol + 1;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		break;
	}
	//棋子是皇后类型
	case PieceType::QUEEN:
	{
		/*
			像城堡移动
		*/
		row = fromRow - 1;
		col = fromCol;
		while (row >= board->MIN_ROW_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			row--;
		}

		row = fromRow + 1;
		col = fromCol;
		while (row <= board->MAX_ROW_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			row++;
		}

		/*
			Get possible moves from its position to lower index column
		*/
		row = fromRow;
		col = fromCol - 1;
		while (col >= board->MIN_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			col--;
		}

		/*
			Get possible moves from its position to upper index column
		*/
		row = fromRow;
		col = fromCol + 1;
		while (col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			col++;
		}

		/*
			像传教士一样移动
		*/
		row = fromRow + 1;
		col = fromCol - 1;
		while (row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			row++;
			col--;
		}

		row = fromRow + 1;
		col = fromCol + 1;
		while (row <= board->MAX_ROW_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			row++;
			col++;
		}

		row = fromRow - 1;
		col = fromCol + 1;
		while (row >= board->MIN_ROW_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			row--;
			col++;
		}

		row = fromRow - 1;
		col = fromCol - 1;
		while (row >= board->MIN_ROW_INDEX && col >= board->MIN_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			row--;
			col--;
		}

		break;
	}
	case PieceType::KING:
	{
		row = fromRow - 1;
		col = fromCol - 1;
		//目标位置在棋盘内
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			//若目标位置上没有棋子
			if (!board->getSquare(row, col)->occupiedState())
			{
				//把移动类型，初始点，目的地，棋子压入vector
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			//若有
			else
			{
				//不是本方
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					//把移动类型，初始点，目的地，棋子，可俘获棋子压入vector
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow - 1;
		col = fromCol;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow - 1;
		col = fromCol + 1;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow;
		col = fromCol - 1;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow;
		col = fromCol + 1;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow + 1;
		col = fromCol - 1;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow + 1;
		col = fromCol;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		row = fromRow + 1;
		col = fromCol + 1;
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			if (!board->getSquare(row, col)->occupiedState())
			{
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			else
			{
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
			}
		}

		/*
			王车易位
		*/
		//如果王未移动，且第一列的城堡未移动
		if (!status->isKingMove(piece->getColor()) && !status->isFirstColRookMove(piece->getColor()))
		{
			//起始位置的左边两个格子都没有棋子
			if (!board->getSquare(fromRow, fromCol - 1)->occupiedState() && !board->getSquare(fromRow, fromCol - 2)->occupiedState())
				//把移动类型，初始点，目的地，棋子压入vector
				possibleMove.push_back(Move(MoveType::CASTLING, fromRow, fromCol, fromRow, fromCol - 2, piece));
		}
		if (!status->isKingMove(piece->getColor()) && !status->isLastColRookMove(piece->getColor()))
		{
			if (!board->getSquare(fromRow, fromCol + 1)->occupiedState() && !board->getSquare(fromRow, fromCol + 2)->occupiedState())
				possibleMove.push_back(Move(MoveType::CASTLING, fromRow, fromCol, fromRow, fromCol + 2, piece));
		}

		break;
	}
	}
	return possibleMove;
}

//获取能保证王不被俘获的移动
std::vector<Move> Gameplay::getValidMoves(GameStatus* status, Board* board, Piece* piece, int fromRow, int fromCol)
{
	std::vector<Move> validMoves = getPossibleMoves(status, board, piece, fromRow, fromCol);
	int sz = validMoves.size();
	for (int index = 0; index < sz; index++)
	{
		/* Check move will make "CHECK" or not */
		Board* nextBoard = new Board(*board);
		GameStatus* nextStatus = new GameStatus(*status);
		move(nextStatus, nextBoard, validMoves[index]);
		if (isCheckState(nextStatus, nextBoard, piece->getColor()))
		{
			validMoves.erase(validMoves.begin() + index);
			sz--;
			index--;
		}

		delete(nextBoard);
		delete(nextStatus);
	}
	return validMoves;
}

//判断是否处于check状态
bool Gameplay::isCheckState(GameStatus* status, Board* board, PieceColor color)
{
	//初始化王的位置
	int sz, kingRow = board->MIN_ROW_INDEX - 1, kingCol = board->MIN_COL_INDEX - 1;
	std::vector<Move> moves;
	Piece* actPiece;
	//遍历每一个格子
	for (int row = board->MIN_ROW_INDEX; row <= board->MAX_ROW_INDEX; row++)
	{
		for (int col = board->MIN_COL_INDEX; col <= board->MAX_COL_INDEX; col++)
		{
			//该格子上没有棋子，继续下一个循环
			if (!board->getSquare(row, col)->occupiedState()) 
				continue;
			//有棋子，获取该棋子
			actPiece = board->getSquare(row, col)->getOccupyingPiece();
			//如果是本方王，获取他的位置
			if (actPiece->getType() == PieceType::KING && actPiece->getColor() == color)
			{
				kingRow = row;
				kingCol = col;
				break;
			}
		}
		if (kingRow != board->MIN_ROW_INDEX - 1 && kingCol != board->MIN_COL_INDEX - 1)
			break;
	}

	//遍历每一个格子
	for (int row = board->MIN_ROW_INDEX; row <= board->MAX_ROW_INDEX; row++)
	{
		for (int col = board->MIN_COL_INDEX; col <= board->MAX_COL_INDEX; col++)
		{
			//该格子上没有棋子，继续下一个循环
			if (!board->getSquare(row, col)->occupiedState()) 
				continue;
			//有棋子，获取该棋子
			actPiece = board->getSquare(row, col)->getOccupyingPiece();
			//若不是本方棋子
			if (actPiece->getColor() != color)
			{
				//获取其可能的移动
				moves = getPossibleMoves(status, board, actPiece, row, col);
				sz = moves.size();
				//若能俘获本方王，返回真
				for (int index = 0; index < sz; index++)
				{
					if (moves[index].getDestinationPosition().first == kingRow && moves[index].getDestinationPosition().second == kingCol) 
						return true;
				}
				moves.clear();
			}
		}
	}
	//返回假
	return false;
}

//判断是否处于checkMate状态
bool Gameplay::isCheckMateState(GameStatus* status, Board* board, PieceColor color)
{
	return allValidMoveCount(status, board, color) == 0;
}

//还能走得格子数
int Gameplay::allValidMoveCount(GameStatus* status, Board* board, PieceColor color)
{
	int count = 0;
	Piece* actPiece;
	//遍历每一个格子
	for (int row = board->MIN_ROW_INDEX; row <= board->MAX_ROW_INDEX; row++)
	{
		for (int col = board->MIN_COL_INDEX; col <= board->MAX_COL_INDEX; col++)
		{
			//若该格子有棋子
			if (board->getSquare(row, col)->occupiedState())
			{
				//获取该棋子
				actPiece = board->getSquare(row, col)->getOccupyingPiece();
				//如果是本方棋子
				if (actPiece->getColor() == color)
				{
					//获取其能保证王不被俘获的移动的格子数
					count += getValidMoves(status, board, actPiece, row, col).size();
				}
			}
		}
	}

	return count;
}

//士兵晋升
bool Gameplay::pawnPromotion(Board* board, int row, int col, PieceType promoteTo)
{
	Square* activeSquare;
	//若该格子上有棋子
	if (board->getSquare(row, col)->occupiedState())
	{
		//获取该棋子
		activeSquare = board->getSquare(row, col);
		//如果是士兵
		if (activeSquare->getOccupyingPiece()->getType() == PieceType::PAWN)
		{
			//移除，并记录
			Piece* removed = activeSquare->removeOccupyingPiece();
			//更改为晋升后棋子
			activeSquare->occupySquare(new Piece(promoteTo, removed->getColor()));
			return true;
		}
	}
	return false;
}
