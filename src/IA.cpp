#include "../includes/Gomoku.hpp"

double time_IsPointIn = 0;
double time_isMoveAuthorized = 0;
double time_newBoard = 0;
double time_doCaptures = 0;
double time_EvaluateBoard = 0;
double time_IsInList = 0;
double time_alphaBeta = 0;
double time_generatePossibleBoards = 0;
double time_generateBoardsFromPoint = 0;

int n_newBoard = 0;
int n_EvaluateBoard = 0;


IA::IA()
{

}

IA::~IA()
{

}

// ------------------------------------------------------------------ //
//	The main method for the IA Class. This will send back the		  //
//	ia's decided move.												  //
//																	  //
// ------------------------------------------------------------------ //

t_vec2		IA::decideMove(t_GameDatas &gameDatas)
{
	t_vec2 decidedMove;
	Board	*finalMove;
	srand (time(NULL));

	decidedMove.x = 0;
	decidedMove.y = 0;

	/**/double start_alphaBeta = clock();
	finalMove = alphaBeta(&gameDatas.Board, IA_DEEP, ALPHA, BETA, WHITE, WHITE);
	/**/time_alphaBeta += (clock() - start_alphaBeta) / double(CLOCKS_PER_SEC) * 1000;

	BoardTools::printParents(finalMove);
	decidedMove = BoardTools::getFistMove(finalMove);
	//cout << "final move: " << BoardTools::countChild(&gameDatas.Board) << " | heuristic to find: " << heuristic << endl << endl;
	for (vector<Board *>::iterator it = gameDatas.Board.next.begin() ; it != gameDatas.Board.next.end() ; ++it)
	{
		delete *it;
	}
	gameDatas.Board.next.clear();
	return (decidedMove);
}

Board	*IA::alphaBeta(Board *board, int deep, int alpha, int beta, t_Color player, t_Color decideMoveFor)
{
	Board	*valBoard = NULL;
	Board	*bestBoard = NULL;

	/**/int start_EvaluateBoard = clock();
	board->heuristic = Heuristic::EvaluateBoard(*board, decideMoveFor);
	/**/time_EvaluateBoard += (clock() - start_EvaluateBoard) / double(CLOCKS_PER_SEC) * 1000;
	/**/n_EvaluateBoard++;

	if (deep == 0 || board->isVictory)
	{
		if (player != decideMoveFor)
			board->heuristic = -board->heuristic;
		return (board);
	}
	else
	{
		bestBoard = new Board();
		bestBoard->heuristic = ALPHA;
		generatePossibleBoards(board, player, decideMoveFor);
		for (vector<Board *>::iterator it = board->next.begin() ; it != board->next.end() ; ++it)
		{
			
			valBoard = alphaBeta(*it, deep - 1, -beta, -alpha, Tools::inverseColorPlayer(player), decideMoveFor);
			valBoard->heuristic = -valBoard->heuristic;
			if (valBoard->heuristic > bestBoard->heuristic)
			{
				bestBoard = valBoard;
				if (bestBoard->heuristic > alpha)
				{
					alpha = bestBoard->heuristic;
					if (alpha >= beta)
						return (bestBoard);
				}
			}
		}
		return (bestBoard);
	}
}

void	IA::generatePossibleBoards(Board *board, t_Color player, t_Color decideMoveFor)
{
	/**/int start_generatePossibleBoards = clock();
	static t_vec2	move_tmp;
	static int		x;
	static int		y;

	for (y = 0; y < 19; y++)
	{
		for (x = 0; x < 19; x++)
		{
			if (board->map[y][x] == WHITE || board->map[y][x] == BLACK)
			{
				move_tmp.x = x;
				move_tmp.y = y;
				generateBoardsFromPoint(board, move_tmp, board->next, player, decideMoveFor);
			}
		}
	}
	/**/time_generatePossibleBoards += (clock() - start_generatePossibleBoards) / double(CLOCKS_PER_SEC) * 1000;
	//std::cout << "nb of possible boards for current board: " << board.next.size() << std::endl;
}

/*
**	Generate all the boards for the given point into the given possibleBoards, from the curBoard.
**	We know the board's point is either BLACK or WHITE.
**	We check the adjacent points, and create boards for them.
*/

void	IA::generateBoardsFromPoint(Board *curBoard, t_vec2 point, vector<Board*> &possibleBoards, t_Color player, t_Color decideMoveFor)
{
	/**/int start_generateBoardsFromPoint = clock();
	int		i = 8;
	Board*	newBoard;
	t_vec2	nextMove;
	/**/bool b_isPointIn;
	/**/bool b_isMoveAuthorized;
	/**/bool b_IsInList;

	nextMove.x = point.x - 1;
	nextMove.y = point.y + 1;
	while (i != -1)
	{
		/**/int start_IsPointIn = clock();
		b_isPointIn = BoardTools::IsPointIn(nextMove);
		/**/time_IsPointIn += (clock() - start_IsPointIn) / double(CLOCKS_PER_SEC) * 1000;

		if (b_isPointIn && curBoard->map[nextMove.y][nextMove.x] == NONE)
		{
			/**/int start_isMoveAuthorized = clock();
			b_isMoveAuthorized = GameRules::isMoveAuthorized(*curBoard, player, nextMove);
			/**/time_isMoveAuthorized += (clock() - start_isMoveAuthorized) / double(CLOCKS_PER_SEC) * 1000;

			if (b_isMoveAuthorized)
			{
				/**/int start_newBoard = clock();
				newBoard = new Board(*curBoard, curBoard, nextMove, player);
				/**/time_newBoard += (clock() - start_newBoard) / double(CLOCKS_PER_SEC) * 1000;
				
				/**/int start_IsInList = clock();
				b_IsInList = BoardTools::IsInList(*newBoard, possibleBoards);
				/**/time_IsInList += (clock() - start_IsInList) / double(CLOCKS_PER_SEC) * 1000;

				if (b_IsInList == false)
				{			
					/**/int start_doCapture = clock();
					GameRules::doCaptures(*newBoard, player, nextMove);
					/**/time_doCaptures += (clock() - start_doCapture) / double(CLOCKS_PER_SEC) * 1000;
					possibleBoards.push_back(newBoard);
					n_newBoard++;
				}
				else
					delete newBoard;
			}
		}
		nextMove.x += 1;
		if (i % 3 == 0)
		{
			nextMove.x -= 3;
			nextMove.y -= 1;
		}
		i--;
	}
	/**/time_generateBoardsFromPoint += (clock() - start_generateBoardsFromPoint) / double(CLOCKS_PER_SEC) * 1000;
}
