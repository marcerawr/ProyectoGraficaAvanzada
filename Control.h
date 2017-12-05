#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "main.h"

class CControl
{
	public:
		CControl();
		~CControl();
		void inicializaControl();
		void actualizaControl();
		void controlTeclado();
		void controlJoystick();
		int obtieneEstadoTecla(int codigo);

	private:
		int tipoControl; //1: teclado, 2: joystick
		//0: up, 1: down, 2: left, 3: right, 4: z, 5: x, 6: enter
		int estadoTecla[7]; //0: si no esta presionada, 1: si está presionada
		int estadoAntTecla[7];
};

#endif