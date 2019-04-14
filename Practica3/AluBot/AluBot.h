/*
 * ManuPCBot.h
 *
 *  Created on: 15 ene. 2018
 *      Author: manupc
 */

#include "Bot.h"

#ifndef MANUPCBOT_H_
#define MANUPCBOT_H_

class AluBot:Bot {
public:
	AluBot();
	~AluBot();
//bool primera_jugada; 

	void initialize();
	string getName();
	Move nextMove(const vector<Move> &adversary, const GameState &state);
	int heuristica(const GameState &tablero, Player jugador);
	int Alfa_Beta(const GameState &tablero, int profundidad, int alfa, int beta, Player jugador, Move &movimiento);
};

#endif /* MANUPCBOT_H_ */
