#include "Timer.h"

CTimer::CTimer()
{
	activo=false;
	tiempoInicial=0;
}

CTimer::~CTimer()
{
}

void CTimer::iniciaTimer()
{
	tiempoInicial=GetTickCount();
	activo=true;
}

void CTimer::detieneTimer()
{
	tiempoInicial=0;
	activo=false;
}

int CTimer::TiempoActual()
{
	int tiempo;

	tiempo=GetTickCount()-tiempoInicial;

	return tiempo;
}