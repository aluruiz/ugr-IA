#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"


#include <iostream>
#include <queue>
#include <set>
#include <cmath>

void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}
    struct compara_pares{
        bool operator()(const pair<pair<int,int>, pair<int, int>> &a, const pair<pair<int,int>, pair<int, int>> &b){
          if(a.first.first < b.first.first)
                return true;
          else if (a.first.first == b.first.first and (a.first.second < b.first.second))
            return true;
          else if ((a.first.first == b.first.first) and (a.first.second == b.first.second) and (a.second.first < b.second.first))
            return true;
          else if ((a.first.first == b.first.first) and (a.first.second == b.first.second) and (a.second.first == b.second.first) and (a.second.second < b.second.second))
            return true;
          else
            return false;
        }
    };
//Nodo Anchura
    struct nodo{
        estado first;
        list<Action> second;
        bool operator<(const nodo &x) const {
            if(first.fila < x.first.fila)
                return true;
            else if(first.fila == x.first.fila and first.columna < x.first.columna)
                return true;
            else if(first.fila == x.first.fila and first.columna == x.first.columna and first.orientacion < x.first.orientacion)
                return true;
            else
                return false;
        }
    };

    bool esObstaculo(char ch){
        if(ch == 'T' or ch == 'K' or ch == 'S' or ch == '?'){
            return false;
        }
        else{
            return true;
        }
    }

void pinta_estado(estado v){
    cout << v.fila << ", " << v.columna << ", " << v.orientacion << endl;
}
//Algoritmo Anchura
bool ComportamientoJugador::pathFinding(const estado &origen, const estado &destino, list<Action> &plan) {
    list<nodo> abiertos;
    set<nodo> cerrados;
    nodo actual;
    actual.first=origen;
    abiertos.push_back(actual);

    while(!abiertos.empty() and abiertos.front().first.fila != destino.fila or abiertos.front().first.columna != destino.columna ){
            actual = abiertos.front();
            abiertos.pop_front();
            cerrados.insert(actual);
            //Accion girar a la derecha
            nodo siguiente = actual;
            siguiente.first.orientacion = (actual.first.orientacion +1)%4;
            siguiente.second.push_back(actTURN_R);
            if (cerrados.find(siguiente) == cerrados.end()){
                abiertos.push_back(siguiente);
                pinta_estado(siguiente.first);
                cerrados.insert(siguiente);
            }
            //Accion girar a la izquierda
            siguiente = actual;
            siguiente.first.orientacion = (actual.first.orientacion +3)%4;
            siguiente.second.push_back(actTURN_L);
            if (cerrados.find(siguiente) == cerrados.end()){
                abiertos.push_back(siguiente);
                pinta_estado(siguiente.first);
                cerrados.insert(siguiente);
            }
            //Avanzar
            siguiente = actual;
            switch(actual.first.orientacion){
            case 0:
                siguiente.first.fila --;
            break;
            case 1:
                siguiente.first.columna ++;
            break;
            case 2:
                siguiente.first.fila ++;
            break;
            case 3:
                siguiente.first.columna --;
            break;
            }
            if(!esObstaculo(mapaResultado[siguiente.first.fila][siguiente.first.columna])){
                siguiente.second.push_back(actFORWARD);
                if (cerrados.find(siguiente) == cerrados.end()){
                    abiertos.push_back(siguiente);
                    pinta_estado(siguiente.first);
                    cerrados.insert(siguiente);
                }
            }
    }

    if(!abiertos.empty()){
        cout << "He encontrado solucion" << endl;
        plan = abiertos.front().second;
        VisualizaPlan(origen, plan);
        return true;
    }
    else{
        return false;
    }

};

Action ComportamientoJugador::think(Sensores sensores) {
  if (sensores.mensajeF != -1 and !bien_situado){
		fil = sensores.mensajeF;
		col = sensores.mensajeC;
		bien_situado = true;
  }

    if (!bien_situado){
       if ((sensores.terreno[2]=='S' or sensores.terreno[2]=='T' or sensores.terreno[2]=='K') and sensores.superficie[2]!='a'){
           return actFORWARD;
       }
       else{
           return actTURN_R;
       }
    }

	// Actualizar el efecto de la ultima accion
	switch (ultimaAccion){
		case actTURN_R: brujula = (brujula+1)%4; break;
		case actTURN_L: brujula = (brujula+3)%4; break;
		case actFORWARD:
			switch (brujula){
				case 0: fil--; break;
				case 1: col++; break;
				case 2: fil++; break;
				case 3: col--; break;
			}
			cout << "fil: " << fil << "  col: " << col << " Or: " << brujula << endl;
	}
	cout << "Destino fil: " << sensores.destinoF << "  col: " << sensores.destinoC << endl;

	if (sensores.reset){
		hayPlan = false;
	}

	if(bien_situado){
        guardarVisitado(sensores);
	}

	// Determinar si ha cambiado el destino desde la ultima planificacion
	if (hayPlan and (sensores.destinoF != ultPosF or sensores.destinoC != ultPosC)){
		cout << "El destino ha cambiado\n";
		hayPlan = false;
	}

		estado origen, destino;
		origen.fila = fil;
		origen.columna = col;
		origen.orientacion = brujula;

		destino.fila = sensores.destinoF;
		destino.columna = sensores.destinoC;

		// Determinar si tengo que construir un plan
	if (!hayPlan or esObstaculo(sensores.terreno[2]) and plan.front()==actFORWARD){
        if (bien_situado){
            hayPlan = pathFinding(origen,destino,plan);
        }

		ultPosF = sensores.destinoF;
		ultPosC = sensores.destinoC;
	}


	// Ejecutar el plan
    Action sigAccion;
	if (hayPlan and plan.size()>0 and sensores.superficie[2] != 'a'){
		sigAccion = plan.front();
		plan.erase(plan.begin());
	}
	else {
		sigAccion = actIDLE;
	}

	ultimaAccion = sigAccion;
	return sigAccion;
}


void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}

void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}

void ComportamientoJugador::guardarVisitado(Sensores sensores){
	mapaResultado[fil][col] = sensores.terreno[0];

	if (brujula == 0){
		mapaResultado[fil-1][col-1] = sensores.terreno[1];
		mapaResultado[fil-1][col] = sensores.terreno[2];
		mapaResultado[fil-1][col+1] = sensores.terreno[3];

		mapaResultado[fil-2][col-2] = sensores.terreno[4];
		mapaResultado[fil-2][col-1] = sensores.terreno[5];
		mapaResultado[fil-2][col] = sensores.terreno[6];
		mapaResultado[fil-2][col+1] = sensores.terreno[7];
		mapaResultado[fil-2][col+2] = sensores.terreno[8];

		mapaResultado[fil-3][col-3] = sensores.terreno[9];
		mapaResultado[fil-3][col-2] = sensores.terreno[10];
		mapaResultado[fil-3][col-1] = sensores.terreno[11];
		mapaResultado[fil-3][col] = sensores.terreno[12];
		mapaResultado[fil-3][col+1] = sensores.terreno[13];
		mapaResultado[fil-3][col+2] = sensores.terreno[14];
		mapaResultado[fil-3][col+3] = sensores.terreno[15];
	}
	else
		if (brujula == 1){
			mapaResultado[fil-1][col+1] = sensores.terreno[1];
			mapaResultado[fil][col+1] = sensores.terreno[2];
			mapaResultado[fil+1][col+1] = sensores.terreno[3];

			mapaResultado[fil-2][col+2] = sensores.terreno[4];
			mapaResultado[fil-1][col+2] = sensores.terreno[5];
			mapaResultado[fil][col+2] = sensores.terreno[6];
			mapaResultado[fil+1][col+2] = sensores.terreno[7];
			mapaResultado[fil+2][col+2] = sensores.terreno[8];

			mapaResultado[fil-3][col+3] = sensores.terreno[9];
			mapaResultado[fil-2][col+3] = sensores.terreno[10];
			mapaResultado[fil-1][col+3] = sensores.terreno[11];
			mapaResultado[fil][col+3] = sensores.terreno[12];
			mapaResultado[fil+1][col+3] = sensores.terreno[13];
			mapaResultado[fil+2][col+3] = sensores.terreno[14];
			mapaResultado[fil+3][col+3] = sensores.terreno[15];
		}
		else
			if (brujula == 2){
				mapaResultado[fil+1][col+1] = sensores.terreno[1];
				mapaResultado[fil+1][col] = sensores.terreno[2];
				mapaResultado[fil+1][col-1] = sensores.terreno[3];

				mapaResultado[fil+2][col+2] = sensores.terreno[4];
				mapaResultado[fil+2][col+1] = sensores.terreno[5];
				mapaResultado[fil+2][col] = sensores.terreno[6];
				mapaResultado[fil+2][col-1] = sensores.terreno[7];
				mapaResultado[fil+2][col-2] = sensores.terreno[8];

				mapaResultado[fil+3][col+3] = sensores.terreno[9];
				mapaResultado[fil+3][col+2] = sensores.terreno[10];
				mapaResultado[fil+3][col+1] = sensores.terreno[11];
				mapaResultado[fil+3][col] = sensores.terreno[12];
				mapaResultado[fil+3][col-1] = sensores.terreno[13];
				mapaResultado[fil+3][col-2] = sensores.terreno[14];
				mapaResultado[fil+3][col-3] = sensores.terreno[15];
			}
			else
				if (brujula == 3){
					mapaResultado[fil+1][col-1] = sensores.terreno[1];
					mapaResultado[fil][col-1] = sensores.terreno[2];
					mapaResultado[fil-1][col-1] = sensores.terreno[3];

					mapaResultado[fil+2][col-2] = sensores.terreno[4];
					mapaResultado[fil+1][col-2] = sensores.terreno[5];
					mapaResultado[fil][col-2] = sensores.terreno[6];
					mapaResultado[fil-1][col-2] = sensores.terreno[7];
					mapaResultado[fil-2][col-2] = sensores.terreno[8];

					mapaResultado[fil+3][col-3] = sensores.terreno[9];
					mapaResultado[fil+2][col-3] = sensores.terreno[10];
					mapaResultado[fil+1][col-3] = sensores.terreno[11];
					mapaResultado[fil][col-3] = sensores.terreno[12];
					mapaResultado[fil-1][col-3] = sensores.terreno[13];
					mapaResultado[fil-2][col-3] = sensores.terreno[14];
					mapaResultado[fil-3][col-3] = sensores.terreno[15];
				}
}
