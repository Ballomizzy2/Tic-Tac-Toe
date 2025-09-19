#include "TicTacToe.h"
#include "Square.h"
#include "Bit.h"
#include "Player.h"
#include <array>
#include <string>
// -----------------------------------------------------------------------------
// TicTacToe.cpp
// -----------------------------------------------------------------------------
// This file is intentionally *full of comments* and gentle TODOs that guide you
// through wiring up a complete Tic‑Tac‑Toe implementation using the game engine’s
// Bit / BitHolder grid system.
//
// Rules recap:
//  - Two players place X / O on a 3x3 grid.
//  - Players take turns; you can only place into an empty square.
//  - First player to get three-in-a-row (row, column, or diagonal) wins.
//  - If all 9 squares are filled and nobody wins, it’s a draw.
//
// Notes about the provided engine types you'll use here:
//  - Bit              : a visual piece (sprite) that belongs to a Player
//  - BitHolder        : a square on the board that can hold at most one Bit
//  - Player           : the engine’s player object (you can ask who owns a Bit)
//  - Game options     : let the mouse know the grid is 3x3 (rowX, rowY)
//  - Helpers you’ll see used: setNumberOfPlayers, getPlayerAt, startGame, etc.
//
// I’ve already fully implemented PieceForPlayer() for you. Please leave that as‑is.
// The rest of the routines are written as “comment-first” TODOs for you to complete.
// -----------------------------------------------------------------------------

const int AI_PLAYER   = 1;      // index of the AI player (O)
const int HUMAN_PLAYER= 0;      // index of the human player (X)

TicTacToe::TicTacToe()
{
}

TicTacToe::~TicTacToe()
{
}
// -----------------------------------------------------------------------------
// DO NOT CHANGE: Provided by the assignment
Bit* TicTacToe::PieceForPlayer(const int playerNumber)
{
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == 0 ? "x.png" : "o.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}
// -----------------------------------------------------------------------------

void TicTacToe::setUpBoard()
{
    // 2 players, 3x3 grid
    setNumberOfPlayers(2);
    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;

    // Layout: tweak as desired
    const ImVec2 origin(60.0f, 60.0f);
    const float  spacing = 128.0f; // distance between cell centers

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            ImVec2 pos(origin.x + x * spacing, origin.y + y * spacing);
            _grid[y][x].initHolder(pos, "square.png", x, y); // Square -> BitHolder with color selection
            _grid[y][x].destroyBit(); // ensure clean board
        }
    }

    startGame();
}

bool TicTacToe::actionForEmptyHolder(BitHolder *holder)
{
    // 1) Guard
    if (!holder) return false;

    // 2) Must be empty
    if (!holder->empty()) return false;

    // 3) Place current player's piece
    const int current = getCurrentPlayer()->playerNumber(); // zero-based
    Bit *piece = PieceForPlayer(current);
    piece->setPosition(holder->getPosition());              // center on square
    holder->setBit(piece);

    return true; // engine will advance turns/check end-of-turn elsewhere
}

bool TicTacToe::canBitMoveFrom(Bit* /*bit*/, BitHolder* /*src*/)
{
    // No moving in tic-tac-toe
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit* /*bit*/, BitHolder* /*src*/, BitHolder* /*dst*/)
{
    // No moving in tic-tac-toe
    return false;
}

void TicTacToe::stopGame()
{
    // Free all bits, leave holders intact
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            _grid[y][x].destroyBit();
}

Player* TicTacToe::ownerAt(int index) const
{
    const int y = index / 3;
    const int x = index % 3;
    if (x < 0 || x >= 3 || y < 0 || y >= 3) return nullptr;

    Bit* b = _grid[y][x].bit();
    return b ? b->getOwner() : nullptr;
}

Player* TicTacToe::checkForWinner()
{
    static const int lines[8][3] = {
        {0,1,2}, {3,4,5}, {6,7,8}, // rows
        {0,3,6}, {1,4,7}, {2,5,8}, // cols
        {0,4,8}, {2,4,6}           // diags
    };

    for (const auto& L : lines)
    {
        Player* a = ownerAt(L[0]);
        if (!a) continue;
        Player* b = ownerAt(L[1]);
        Player* c = ownerAt(L[2]);
        if (a && b && c && a == b && b == c)
            return a;
    }
    return nullptr;
}

bool TicTacToe::checkForDraw()
{
    // Draw iff no empties AND no winner
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            if (_grid[y][x].empty())
                return false;
    return checkForWinner() == nullptr;
}

std::string TicTacToe::initialStateString()
{
    return "000000000";
}

std::string TicTacToe::stateString() const
{
    // Left-to-right, top-to-bottom
    std::string s;
    s.reserve(9);
    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            Bit* b = _grid[y][x].bit();
            if (!b) { s.push_back('0'); continue; }

            // Player numbers are zero-based; encode as '1' or '2'
            int pn = b->getOwner() ? b->getOwner()->playerNumber() : 0; // 0 or 1
            s.push_back(char('1' + pn)); // 0->'1', 1->'2'
        }
    }
    return s;
}

void TicTacToe::setStateString(const std::string &s)
{
    if (s.size() != 9) return; // defensive

    // Clear current
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            _grid[y][x].destroyBit();

    // Recreate from snapshot
    for (int idx = 0; idx < 9; ++idx)
    {
        int y = idx / 3;
        int x = idx % 3;
        int v = s[idx] - '0'; // 0 empty, 1 player0, 2 player1
        if (v == 0) continue;

        int playerIndex = v - 1; // back to zero-based
        Bit* piece = PieceForPlayer(playerIndex);
        piece->setPosition(_grid[y][x].getPosition());
        _grid[y][x].setBit(piece);
    }
}

void TicTacToe::updateAI()
{
    // Implement in next assignment
}
