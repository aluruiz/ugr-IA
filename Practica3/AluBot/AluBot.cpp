/*
 * ManuPCBot.cpp
 *
 *  Created on: 15 ene. 2018
 *      Author: manupc
 */

#include "AluBot.h"

#include <string>
#include <cstdlib>
#include <iostream>
#include <list>
#include <stack>
using namespace std;

const int mas_infinito = 100000, menos_infinito=-100000, gano=90000, pierdo=-90000;

AluBot::AluBot() {
	// Inicializar las variables necesarias para ejecutar la partida
	srand(time(0));

}

AluBot::~AluBot() {
	// Liberar los recursos reservados (memoria, ficheros, etc.)
}

void AluBot::initialize() {
	// Inicializar el bot antes de jugar una partida
	//primera_jugada = true;
}

string AluBot::getName() {
	return "AluBot"; // Sustituir por el nombre del bot
}

int AluBot::heuristica(const GameState &tablero, Player jugador){
	Player adversario = J2;
	int n, i, j;
	int pros=0, contras=0;
	int valor;
	if (jugador == J2){
		adversario = J1;
	}

	if(tablero.getSeedsAt(jugador, GRANERO) > 24){
		return gano;
	}
	else if (tablero.getSeedsAt(adversario, GRANERO)> 24){
		return pierdo;
	}

	for (i=1; i<=6; i++){
		Position posicion = (Position) i;
		valor = tablero.getSeedsAt(jugador, posicion) - i;
		Position adversa = (Position) (7-i);
		//Si soy jugador
		if(tablero.getCurrentPlayer()==jugador){
			//Si puedo robar
			if (tablero.getSeedsAt(jugador, posicion) == 0 and tablero.getSeedsAt(adversario, adversa)>0){
				for(j=1; j<=6; j++){
					Position p_j = (Position) j;
					n = tablero.getSeedsAt(jugador, p_j);
					if(n==(i-1)){
						pros += tablero.getSeedsAt(jugador, GRANERO) + tablero.getSeedsAt(adversario, adversa) + tablero.getSeedsAt(jugador, posicion);
					}
				}
			}
			//Si puedo conseguir un turno mas
			else if(valor == 1){
					pros += tablero.getSeedsAt(jugador, GRANERO);
				}
			else{
				pros += tablero.getSeedsAt(jugador, GRANERO);
			}
		}
		//Si no soy jugador
		else{
			//Si pueden robarme
			if(tablero.getSeedsAt(jugador, posicion) > 0 and tablero.getSeedsAt(adversario, adversa)==0){
				for(j=1; j<=6; j++){
					Position p_j = (Position) (7-j);
					n = tablero.getSeedsAt(adversario, p_j);
					if(n==((7-i)-1)){
						contras += tablero.getSeedsAt(jugador, GRANERO) + tablero.getSeedsAt(adversario, adversa) + tablero.getSeedsAt(jugador, posicion);
					}
				}
			}
		}
	}

	//return tablero.getSeedsAt(jugador, GRANERO);
	return (pros - contras);
}

int AluBot::Alfa_Beta(const GameState &tablero, int profundidad, int alfa, int beta, Player jugador, Move &movimiento){
	if (profundidad==0 or tablero.isFinalState()){
		return heuristica(tablero,jugador);
	}

	for (int i=1; i<=6 and alfa<beta; i++){
		if(tablero.getSeedsAt(tablero.getCurrentPlayer(), (Position) i)>0){
			GameState hijo = tablero.simulateMove((Move) i);
			Move anterior;
			int valor = Alfa_Beta(hijo, profundidad-1, alfa, beta, jugador, anterior);
			if(tablero.getCurrentPlayer()==jugador){
				if(valor>alfa){
					alfa = valor;
					movimiento = (Move) i;
				}
			}
			else{
				if(valor<beta){
					beta = valor;
				}
			}
		}
	}
	if(tablero.getCurrentPlayer()==jugador){
		return alfa;
	}
	else{
		return beta;
	}
}

Move AluBot::nextMove(const vector<Move> &adversary, const GameState &state) {
	Player yo= this->getPlayer();
	long timeout= this->getTimeOut();
	Move movimiento;
	/*if (primera_jugada and yo == J1){
		primera_jugada= false;
		return (Move) 4;
	}*/
	int valor = Alfa_Beta(state, 10, menos_infinito, mas_infinito, yo, movimiento);
	// Implementar aquí la selección de la acción a realizar

	// OJO: Recordatorio. NO USAR cin NI cout.
	// Para salidas por consola (debug) utilizar cerr. Ejemplo:
	// cerr<< "Lo que quiero mostrar"<<endl;

	// OJO: Recordatorio. El nombre del bot y de la clase deben coincidir.
	// En caso contrario, el bot no podrá participar en la competición.
	// Se deberá sustituir el nombre MiBot como nombre de la clase por otro
	// seleccionado por el alumno. Se deberá actualizar también el nombre
	// devuelto por el método getName() acordemente.

	return movimiento;
}
