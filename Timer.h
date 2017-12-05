#ifndef _TIMER_H
#define _TIMER_H

#include "Main.h"

class CTimer
{
	public:
		CTimer();
		~CTimer();
		void iniciaTimer();
		void detieneTimer();
		int TiempoActual(); //En milisegundos
		
	private:
		bool activo;
		int tiempoInicial;

};

#endif 