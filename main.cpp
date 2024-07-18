#include <iostream>
#include <algorithm>
#include <vector>
#include <array>

using namespace std;

#define WIN 1000
#define DRAW 0
#define LOSS -1000

#define AI_MARKER 'O'
#define PLAYER_MARKER 'X'
#define EMPTY_SPACE '-'

#define START_DEPTH 0

// Print game state
void print_game_state(int state)
{
    if (WIN == state) { cout << "WIN" << endl; }
    else if (DRAW == state) { cout << "DRAW" << endl; }
    else if (LOSS == state) { cout << "LOSS" << endl; }
}

// All possible winning states
vector<vector<pair<int, int>>> winning_states
{
    // Every row
    { make_pair(0, 0), make_pair(0, 1), make_pair(0, 2) },
    { make_pair(1, 0), make_pair(1, 1), make_pair(1, 2) },
    { make_pair(2, 0), make_pair(2, 1), make_pair(2, 2) },

    // Every column
    { make_pair(0, 0), make_pair(1, 0), make_pair(2, 0) },
    { make_pair(0, 1), make_pair(1, 1), make_pair(2, 1) },
    { make_pair(0, 2), make_pair(1, 2), make_pair(2, 2) },

    // Every diagonal
    { make_pair(0, 0), make_pair(1, 1), make_pair(2, 2) },
    { make_pair(2, 0), make_pair(1, 1), make_pair(0, 2) }
};

// Print the current board state
void print_board(char board[3][3])
{
    cout << endl;
    cout << board[0][0] << " | " << board[0][1] << " | " << board[0][2] << endl;
    cout << "----------" << endl;
    cout << board[1][0] << " | " << board[1][1] << " | " << board[1][2] << endl;
    cout << "----------" << endl;
    cout << board[2][0] << " | " << board[2][1] << " | " << board[2][2] << endl << endl;
}

// Get all available legal moves (spaces that are not occupied)
vector<pair<int, int>> get_legal_moves(char board[3][3])
{
    vector<pair<int, int>> legal_moves;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] != AI_MARKER && board[i][j] != PLAYER_MARKER)
            {
                legal_moves.push_back(make_pair(i, j));
            }
        }
    }

    return legal_moves;
}

// Check if a position is occupied
bool position_occupied(char board[3][3], pair<int, int> pos)
{
    return board[pos.first][pos.second] != EMPTY_SPACE;
}

// Get all board positions occupied by the given marker
vector<pair<int, int>> get_occupied_positions(char board[3][3], char marker)
{
    vector<pair<int, int>> occupied_positions;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (marker == board[i][j])
            {
                occupied_positions.push_back(make_pair(i, j));
            }
        }
    }

    return occupied_positions;
}

// Check if the board is full
bool board_is_full(char board[3][3])
{
    return get_legal_moves(board).empty();
}

// Check if the game has been won
bool game_is_won(vector<pair<int, int>> occupied_positions)
{
    for (const auto& curr_win_state : winning_states)
    {
        bool game_won = true;
        for (const auto& pos : curr_win_state)
        {
            if (find(occupied_positions.begin(), occupied_positions.end(), pos) == occupied_positions.end())
            {
                game_won = false;
                break;
            }
        }
        if (game_won)
        {
            return true;
        }
    }
    return false;
}

char get_opponent_marker(char marker)
{
    return (marker == PLAYER_MARKER) ? AI_MARKER : PLAYER_MARKER;
}

// Check if someone has won or lost
int get_board_state(char board[3][3], char marker)
{
    char opponent_marker = get_opponent_marker(marker);

    if (game_is_won(get_occupied_positions(board, marker)))
    {
        return WIN;
    }

    if (game_is_won(get_occupied_positions(board, opponent_marker)))
    {
        return LOSS;
    }

    if (board_is_full(board))
    {
        return DRAW;
    }

    return DRAW;
}

// Apply the minimax game optimization algorithm
pair<int, pair<int, int>> minimax_optimization(char board[3][3], char marker, int depth, int alpha, int beta)
{
    // Initialize best move
    pair<int, int> best_move = make_pair(-1, -1);
    int best_score = (marker == AI_MARKER) ? LOSS : WIN;

    // If we hit a terminal state (leaf node), return the best score and move
    if (board_is_full(board) || DRAW != get_board_state(board, AI_MARKER))
    {
        best_score = get_board_state(board, AI_MARKER);
        return make_pair(best_score, best_move);
    }

    vector<pair<int, int>> legal_moves = get_legal_moves(board);

    for (const auto& curr_move : legal_moves)
    {
        board[curr_move.first][curr_move.second] = marker;

        // Maximizing player's turn
        if (marker == AI_MARKER)
        {
            int score = minimax_optimization(board, PLAYER_MARKER, depth + 1, alpha, beta).first;

            // Get the best scoring move
            if (best_score < score)
            {
                best_score = score - depth * 10;
                best_move = curr_move;

                // Check if this branch's best move is worse than the best
                // option of a previously searched branch. If it is, skip it
                alpha = max(alpha, best_score);
                board[curr_move.first][curr_move.second] = EMPTY_SPACE;
                if (beta <= alpha) 
                { 
                    break; 
                }
            }

        } // Minimizing opponent's turn
        else
        {
            int score = minimax_optimization(board, AI_MARKER, depth + 1, alpha, beta).first;

            if (best_score > score)
            {
                best_score = score + depth * 10;
                best_move = curr_move;

                // Check if this branch's best move is worse than the best
                // option of a previously searched branch. If it is, skip it
                beta = min(beta, best_score);
                board[curr_move.first][curr_move.second] = EMPTY_SPACE;
                if (beta <= alpha) 
                { 
                    break; 
                }
            }

        }

        board[curr_move.first][curr_move.second] = EMPTY_SPACE; // Undo move
    }

    return make_pair(best_score, best_move);
}

// Check if the game is finished
bool game_is_done(char board[3][3])
{
    return board_is_full(board) || DRAW != get_board_state(board, AI_MARKER);
}

int main()
{
    char board[3][3] = { {EMPTY_SPACE, EMPTY_SPACE, EMPTY_SPACE}, {EMPTY_SPACE, EMPTY_SPACE, EMPTY_SPACE}, {EMPTY_SPACE, EMPTY_SPACE, EMPTY_SPACE} };

    cout << "********************************\n\n\tTic Tac Toe AI\n\n********************************" << endl << endl;
    cout << "Player = X\t AI Computer = O" << endl << endl;

    print_board(board);

    while (!game_is_done(board))
    {
        int row, col;
        cout << "Row play (0, 1, 2): ";
        cin >> row;
        cout << "Col play (0, 1, 2): ";
        cin >> col;
        cout << endl << endl;

        if (position_occupied(board, make_pair(row, col)))
        {
            cout << "The position (" << row << ", " << col << ") is occupied. Try another one..." << endl;
            continue;
        }
        else
        {
            board[row][col] = PLAYER_MARKER;
        }

        if (!game_is_done(board))
        {
            pair<int, pair<int, int>> ai_move = minimax_optimization(board, AI_MARKER, START_DEPTH, LOSS, WIN);
            board[ai_move.second.first][ai_move.second.second] = AI_MARKER;
        }

        print_board(board);
    }

    cout << "********** GAME OVER **********" << endl << endl;

    int player_state = get_board_state(board, PLAYER_MARKER);

    cout << "PLAYER "; print_game_state(player_state);

    return 0;
}
