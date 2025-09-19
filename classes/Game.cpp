#include "Game.h"
#include "Bit.h"
#include "BitHolder.h"
#include "Turn.h"
#include "../Application.h"

Game::Game()
{
	_gameOptions.AIPlayer = false;
	_gameOptions.AIPlaying = false;
	_gameOptions.currentTurnNo = 0;
	_gameOptions.gameNumber = -1;
	_gameOptions.numberOfPlayers = 0;
	_gameOptions.rowX = 0;
	_gameOptions.rowY = 0;
	_gameOptions.score = 0;
	_gameOptions.AIDepthSearches = 0;
	_gameOptions.AIvsAI = false;
	
	_score = 0;
	_table = nullptr;
	_winner = nullptr;
	_lastMove = "";
	_gameNumber = -1;
}


Game::~Game()
{
	for (auto & _turn : _turns) {
		delete _turn;
	}
	_turns.clear();
	for (auto & _player : _players) {
		delete _player;
	}
	_players.clear();

	_score = 0;
	_table = nullptr;
	_winner = nullptr;
	_gameOptions.currentTurnNo = 0;
	_lastMove = "";
}

void Game::setNumberOfPlayers(unsigned int n)
{
	_players.clear();
	for (unsigned int i = 1; i <= n; i++)
	{
		Player *player = Player::initWithGame(this);
//		player->setName( std::format( "Player-{}", i ) );
		player->setName( "Player" );
		player->setPlayerNumber(i-1);			// player numbers are zero-based
		_players.push_back(player);
	}
	_winner = nullptr;
	_gameNumber = 0;
	_gameOptions.numberOfPlayers = n;
	Turn *turn = Turn::initStartOfGame(this);
	_turns.clear();
	_turns.push_back(turn);
}

void Game::setAIPlayer(unsigned int playerNumber)
{
	_players.at(playerNumber)->setAIPlayer(true);
	_gameOptions.AIPlayer = playerNumber;
	_gameOptions.AIPlayer = true;
}

void Game::startGame()
{
	std::string startState = stateString();
	Turn *turn = _turns.at(0);
	turn->_boardState = startState;
	turn->_gameNumber = _gameNumber;
	_gameOptions.currentTurnNo = 0;
}

void Game::endTurn()
{
	_gameOptions.currentTurnNo++;
	std::string startState = stateString();
	Turn *turn = new Turn;
	turn->_boardState = stateString();
	turn->_date = (int)_gameOptions.currentTurnNo;
	turn->_score = _score;
	turn->_gameNumber = _gameNumber;
	_turns.push_back(turn);
	ClassGame::EndOfTurn();
}

void Game::scanForMouse()
{
    //if (gameHasAI() && getCurrentPlayer()->isAIPlayer()) 
    //{
    //    updateAI();
    //    return;
    //}

    ImVec2 mousePos = ImGui::GetMousePos();
    mousePos.x -= ImGui::GetWindowPos().x;
    mousePos.y -= ImGui::GetWindowPos().y;

    for (int y=0; y<_gameOptions.rowY; y++) {
        for (int x=0; x<_gameOptions.rowX; x++) {
			BitHolder &holder = getHolderAt(x, y);
            if (holder.isMouseOver(mousePos)) {
                if (ImGui::IsMouseClicked(0)) {
                    if (actionForEmptyHolder(&holder)) {
                        endTurn();
                    }
                } else {
                    holder.setHighlighted(true);
                }
            } else {
                holder.setHighlighted(false);
            }
        }
    }    
}

//
// draw the board and then the pieces
// this will also go somewhere else when the heirarchy is set up
//
void Game::drawFrame()
{
    scanForMouse();

    for (int y=0; y<_gameOptions.rowY; y++) {
        for (int x=0; x<_gameOptions.rowX; x++) {
			BitHolder &holder = getHolderAt(x, y);
            holder.paintSprite();
            if (holder.bit()) {
                holder.bit()->paintSprite();
            }
        }
    }
}

void Game::bitMovedFromTo(Bit *bit, BitHolder *src, BitHolder *dst)
{
	endTurn();
}

Bit* Game::bitToPlaceInHolder(BitHolder *holder)
{
	return nullptr;
}

bool Game::actionForEmptyHolder(BitHolder *holder)
{
	return false;
}

bool Game::clickedBit(Bit *bit)
{
	return true;
}

bool Game::animateAndPlaceBitFromTo(Bit *bit, BitHolder*src, BitHolder*dst)
{
	return false;
}

bool Game::gameHasAI()
{
    return false;
}

void Game::updateAI()
{
}

