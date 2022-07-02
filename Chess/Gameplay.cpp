#include <cstddef>
#include <cmath>
#include <iostream>

#include "Gameplay.h"

//�ƶ�����
bool Gameplay::move(GameStatus* status, Board* board, Move move)
{
	Piece* actPiece = move.getMovedPiece();//��ȡ�ƶ�������
	//��ȡ��������
	int fromRow = move.getOriginPosition().first;
	int fromCol = move.getOriginPosition().second;
	//��ȡĿ�ĸ���
	int toRow = move.getDestinationPosition().first;
	int toCol = move.getDestinationPosition().second;
	//�����ƶ������������в�ͬ�Ĳ���
	switch (move.getType())
	{
	//��ֻ����ͨ���ƶ�����
	case MoveType::NORMAL:
	{
		//��������ʿ�����ƶ���������Ϊ��·��״̬
		if (actPiece->getType() == PieceType::PAWN && std::abs(toRow - fromRow) == 2)
			status->setPieceEnPassantable(actPiece->getColor(), actPiece);
		//�����������������ƶ�״̬
		else if (actPiece->getType() == PieceType::KING)
			status->setKingMove(actPiece->getColor());
		//�������ǳǱ����óǱ��ƶ�״̬
		else if (actPiece->getType() == PieceType::ROOK)
		{
			if (fromCol == board->MIN_COL_INDEX) status->setFirstColRookMove(actPiece->getColor());
			else if (fromCol == board->MAX_COL_INDEX) status->setLastColRookMove(actPiece->getColor());
		}
		//�����Ƿ�ɹ�ռ��Ŀ�����
		return board->getSquare(toRow, toCol)->occupySquare(board->getSquare(fromRow, fromCol)->removeOccupyingPiece());
	}
	//�����ܷ���Է���������
	case MoveType::CAPTURE:
	{
		//�����������������ƶ�״̬
		if (actPiece->getType() == PieceType::KING)
			status->setKingMove(actPiece->getColor());
		//�������ǳǱ����óǱ��ƶ�״̬
		else if (actPiece->getType() == PieceType::ROOK)
		{
			if (fromCol == board->MIN_COL_INDEX) status->setFirstColRookMove(actPiece->getColor());
			else if (fromCol == board->MAX_COL_INDEX) status->setLastColRookMove(actPiece->getColor());
		}
		//�Ƴ�Ŀ����ӵ�����
		board->getSquare(toRow, toCol)->removeOccupyingPiece();
		//�����Ƿ�ɹ�ռ��Ŀ�����
		return board->getSquare(toRow, toCol)->occupySquare(board->getSquare(fromRow, fromCol)->removeOccupyingPiece());
	}
	//��Ϊ�Թ�·������
	case MoveType::EN_PASSANT:
	{
		//�Ƴ�Ŀ����ӵ�����
		board->getSquare(fromRow, toCol)->removeOccupyingPiece();
		//�����Ƿ�ɹ�ռ��Ŀ�����
		return board->getSquare(toRow, toCol)->occupySquare(board->getSquare(fromRow, fromCol)->removeOccupyingPiece());
	}
	//��Ϊ������λ����
	case MoveType::CASTLING:
	{
		//����������λ�Ǳ���ʼ��Ŀ����
		int rookOriginCol = ((toCol < fromCol) ? board->MIN_COL_INDEX : board->MAX_COL_INDEX);
		int rookDestCol = ((toCol < fromCol) ? fromCol - 1 : fromCol + 1);
		//��ԭ��Ŀ�ĵغ�������λ��Ŀ�ĵطֱ�����ԭ��������ռ��
		board->getSquare(toRow, toCol)->occupySquare(board->getSquare(fromRow, fromCol)->removeOccupyingPiece());
		board->getSquare(toRow, rookDestCol)->occupySquare(board->getSquare(fromRow, rookOriginCol)->removeOccupyingPiece());
		//�����ƶ�״̬
		status->setKingMove(actPiece->getColor());
		//�жϳ����ĸ��Ǳ����óǱ��ƶ�״̬
		rookOriginCol > rookDestCol ? status->setLastColRookMove(actPiece->getColor()) : status->setFirstColRookMove(actPiece->getColor());
		return true;
	}
	default: break;
	}
	return false;
}

//��ȡ���ܵ��ƶ�
std::vector<Move> Gameplay::getPossibleMoves(GameStatus* status, Board* board, Piece* piece, int fromRow, int fromCol)
{
	std::vector<Move> possibleMove;
	int row, col;

	//�����쳣�������
	if (status == NULL || board == NULL || piece == NULL || fromRow < board->MIN_ROW_INDEX || fromRow > board->MAX_ROW_INDEX || fromCol < board->MIN_COL_INDEX || fromCol > board->MAX_COL_INDEX) 
		return possibleMove;

	//�������ӵ����ͷֱ����
	switch (piece->getType())
	{
	//����Ϊʿ��
	case PieceType::PAWN:
	{
		//����ǰ�ɫ��
		if (piece->getColor() == PieceColor::WHITE)
		{
			/*��ǰ�ƶ�һ��*/

			row = fromRow + 1;
			col = fromCol;
			//����������
			if (row <= board->MAX_ROW_INDEX)
			{
				//���ƶ����ͣ���ʼ�㣬Ŀ�ĵأ�����ѹ��vector
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
				//���Ŀ�ĵ������ӣ�����
				if (board->getSquare(row, col)->occupiedState()) 
					possibleMove.pop_back();
			}

			//�������ǰ�ƶ�һ�����ڳ�ʼλ�ã������ƶ�����
			if (possibleMove.size() && fromRow == board->MIN_ROW_INDEX + 1)
			{
				row = fromRow + 2;
				col = fromCol;
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
				if (board->getSquare(row, col)->occupiedState()) possibleMove.pop_back();
			}

			/*б�����������*/
			row = fromRow + 1;
			col = fromCol - 1;
			//���Ŀ����������
			if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
			{
				//���Ŀ��λ����������
				if (board->getSquare(row, col)->occupiedState())
				{
					//���Ŀ��λ���ϵ����Ӳ��Ǳ�������
					if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
						//���ƶ����ͣ���ʼ�㣬Ŀ�ĵأ����ӣ��Լ����������ѹ��vector
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
				�Թ�·������
			*/
			if (fromRow == board->MIN_ROW_INDEX + 4)
			{
				row = fromRow + 1;
				col = fromCol - 1;
				//��Ŀ���������ڣ��Ҹ�λ����û������
				if (col >= board->MIN_COL_INDEX && !board->getSquare(row, col)->occupiedState())
				{
					//��������ͬ���ٽ�λ����������
					if (board->getSquare(fromRow, col)->occupiedState())
					{
						//��ȡ������
						Piece* capPossibility = board->getSquare(fromRow, col)->getOccupyingPiece();
						//��ͬΪʿ���Ҳ��Ǳ����������ǹ�·��״̬
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

		//��������Ǻ�ɫ��
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
	//�����ǳǱ�����
	case PieceType::ROOK:
	{
		/*
			��������ǰ�ƶ�
		*/
		row = fromRow - 1;
		col = fromCol;
		//ֻҪ����������
		while (row >= board->MIN_ROW_INDEX)
		{
			//Ŀ��λ����û������
			if (!board->getSquare(row, col)->occupiedState())
			{
				//���ƶ����ͣ���ʼ�㣬Ŀ�ĵأ�����ѹ��vector
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			//��������
			else
			{
				//���Ӳ��Ǳ�����
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					//���ƶ����ͣ���ʼ�㣬Ŀ�ĵأ����ӣ��ɷ�������ѹ��vector
					possibleMove.push_back(Move(MoveType::CAPTURE, fromRow, fromCol, row, col, piece, board->getSquare(row, col)->getOccupyingPiece()));
				break;
			}
			//�ݼ�
			row--;
		}

		/*
			��������ƶ�
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
			���������ƶ�
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
			���������ƶ�
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
	//����Ϊ��������
	case PieceType::BISHOP:
	{
		/*
			б������
		*/
		row = fromRow + 1;
		col = fromCol - 1;
		//Ŀ�껹��������
		while (row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX)
		{
			//Ŀ��λ����û������
			if (!board->getSquare(row, col)->occupiedState())
			{
				//���ƶ����ͣ���ʼ�㣬Ŀ�ĵأ�����ѹ��vector
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			//��������
			else
			{
				//���Ӳ��Ǳ�����
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					//���ƶ����ͣ���ʼ�㣬Ŀ�ĵأ����ӣ��ɷ�������ѹ��vector
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
	//��������ʿ����
	case PieceType::KNIGHT:
	{
		/*
			������
		*/
		row = fromRow - 2;
		col = fromCol - 1;
		//��Ŀ��λ�û���������
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			//Ŀ��λ����û������
			if (!board->getSquare(row, col)->occupiedState())
			{
				//���ƶ����ͣ���ʼ�㣬Ŀ�ĵأ�����ѹ��vector
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			//����
			else
			{
				//���Ǳ�����
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					//���ƶ����ͣ���ʼ�㣬Ŀ�ĵأ����ӣ��ɷ�������ѹ��vector
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
	//�����ǻʺ�����
	case PieceType::QUEEN:
	{
		/*
			��Ǳ��ƶ�
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
			�񴫽�ʿһ���ƶ�
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
		//Ŀ��λ����������
		if (row >= board->MIN_ROW_INDEX && row <= board->MAX_ROW_INDEX && col >= board->MIN_COL_INDEX && col <= board->MAX_COL_INDEX)
		{
			//��Ŀ��λ����û������
			if (!board->getSquare(row, col)->occupiedState())
			{
				//���ƶ����ͣ���ʼ�㣬Ŀ�ĵأ�����ѹ��vector
				possibleMove.push_back(Move(MoveType::NORMAL, fromRow, fromCol, row, col, piece));
			}
			//����
			else
			{
				//���Ǳ���
				if (board->getSquare(row, col)->getOccupyingPiece()->getColor() != piece->getColor())
					//���ƶ����ͣ���ʼ�㣬Ŀ�ĵأ����ӣ��ɷ�������ѹ��vector
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
			������λ
		*/
		//�����δ�ƶ����ҵ�һ�еĳǱ�δ�ƶ�
		if (!status->isKingMove(piece->getColor()) && !status->isFirstColRookMove(piece->getColor()))
		{
			//��ʼλ�õ�����������Ӷ�û������
			if (!board->getSquare(fromRow, fromCol - 1)->occupiedState() && !board->getSquare(fromRow, fromCol - 2)->occupiedState())
				//���ƶ����ͣ���ʼ�㣬Ŀ�ĵأ�����ѹ��vector
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

//��ȡ�ܱ�֤������������ƶ�
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

//�ж��Ƿ���check״̬
bool Gameplay::isCheckState(GameStatus* status, Board* board, PieceColor color)
{
	//��ʼ������λ��
	int sz, kingRow = board->MIN_ROW_INDEX - 1, kingCol = board->MIN_COL_INDEX - 1;
	std::vector<Move> moves;
	Piece* actPiece;
	//����ÿһ������
	for (int row = board->MIN_ROW_INDEX; row <= board->MAX_ROW_INDEX; row++)
	{
		for (int col = board->MIN_COL_INDEX; col <= board->MAX_COL_INDEX; col++)
		{
			//�ø�����û�����ӣ�������һ��ѭ��
			if (!board->getSquare(row, col)->occupiedState()) 
				continue;
			//�����ӣ���ȡ������
			actPiece = board->getSquare(row, col)->getOccupyingPiece();
			//����Ǳ���������ȡ����λ��
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

	//����ÿһ������
	for (int row = board->MIN_ROW_INDEX; row <= board->MAX_ROW_INDEX; row++)
	{
		for (int col = board->MIN_COL_INDEX; col <= board->MAX_COL_INDEX; col++)
		{
			//�ø�����û�����ӣ�������һ��ѭ��
			if (!board->getSquare(row, col)->occupiedState()) 
				continue;
			//�����ӣ���ȡ������
			actPiece = board->getSquare(row, col)->getOccupyingPiece();
			//�����Ǳ�������
			if (actPiece->getColor() != color)
			{
				//��ȡ����ܵ��ƶ�
				moves = getPossibleMoves(status, board, actPiece, row, col);
				sz = moves.size();
				//���ܷ��񱾷�����������
				for (int index = 0; index < sz; index++)
				{
					if (moves[index].getDestinationPosition().first == kingRow && moves[index].getDestinationPosition().second == kingCol) 
						return true;
				}
				moves.clear();
			}
		}
	}
	//���ؼ�
	return false;
}

//�ж��Ƿ���checkMate״̬
bool Gameplay::isCheckMateState(GameStatus* status, Board* board, PieceColor color)
{
	return allValidMoveCount(status, board, color) == 0;
}

//�����ߵø�����
int Gameplay::allValidMoveCount(GameStatus* status, Board* board, PieceColor color)
{
	int count = 0;
	Piece* actPiece;
	//����ÿһ������
	for (int row = board->MIN_ROW_INDEX; row <= board->MAX_ROW_INDEX; row++)
	{
		for (int col = board->MIN_COL_INDEX; col <= board->MAX_COL_INDEX; col++)
		{
			//���ø���������
			if (board->getSquare(row, col)->occupiedState())
			{
				//��ȡ������
				actPiece = board->getSquare(row, col)->getOccupyingPiece();
				//����Ǳ�������
				if (actPiece->getColor() == color)
				{
					//��ȡ���ܱ�֤������������ƶ��ĸ�����
					count += getValidMoves(status, board, actPiece, row, col).size();
				}
			}
		}
	}

	return count;
}

//ʿ������
bool Gameplay::pawnPromotion(Board* board, int row, int col, PieceType promoteTo)
{
	Square* activeSquare;
	//���ø�����������
	if (board->getSquare(row, col)->occupiedState())
	{
		//��ȡ������
		activeSquare = board->getSquare(row, col);
		//�����ʿ��
		if (activeSquare->getOccupyingPiece()->getType() == PieceType::PAWN)
		{
			//�Ƴ�������¼
			Piece* removed = activeSquare->removeOccupyingPiece();
			//����Ϊ����������
			activeSquare->occupySquare(new Piece(promoteTo, removed->getColor()));
			return true;
		}
	}
	return false;
}
