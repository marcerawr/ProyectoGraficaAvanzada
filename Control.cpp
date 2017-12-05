#include "Control.h"

//Variables para el joystick
JOYINFOEX joyInfo1;
JOYCAPS jcCaps;
MMRESULT err;
MMRESULT err2;

CControl::CControl()
{
}

CControl::~CControl()
{
}

void CControl::inicializaControl()
{
	tipoControl=1;
}

void CControl::actualizaControl()
{
	if(tipoControl == 1)
		controlTeclado();
	else if(tipoControl == 2)
		controlJoystick();
}

void CControl::controlTeclado()
{
	//Al presionar cualquiera de las teclas de dirección se iniciará la animación de caminar
	//Al soltar la tecla presionada se detiene la animación.
	
	//UP
	if(GetKeyState(VK_UP) & 0x80) //Si está presionada la tecla UP
	{
		estadoTecla[0]=1;
	}

	if(!(GetKeyState(VK_UP) & 0x80)) //Si no está presionada la tecla UP
	{
		estadoTecla[0]=0;
	}

	//DOWN
	if(GetKeyState(VK_DOWN) & 0x80) //Si está presionada la tecla DOWN
	{
		estadoTecla[1]=1;
	}

	if(!(GetKeyState(VK_DOWN) & 0x80)) //Si no está presionada la tecla DOWN
	{
		estadoTecla[1]=0;
	}

	//LEFT
	if(GetKeyState(VK_LEFT) & 0x80) //Si está presionada la tecla LEFT
	{
		estadoTecla[2]=1;
	}

	if(!(GetKeyState(VK_LEFT) & 0x80)) //Si no está presionada la tecla LEFT
	{
		estadoTecla[2]=0;
	}

	//RIGHT
	if(GetKeyState(VK_RIGHT) & 0x80) //Si está presionada la tecla RIGHT
	{
		estadoTecla[3]=1;
	}

	if(!(GetKeyState(VK_RIGHT) & 0x80)) //Si no está presionada la tecla RIGHT
	{
		estadoTecla[3]=0;
	}

	//Z
	if(GetKeyState('Z') & 0x80)
	{
		estadoTecla[4]=1;
	}

	if(!(GetKeyState('Z') & 0x80))
	{
		estadoTecla[4]=0;
	}

	//X
	if(GetKeyState('X') & 0x80)
	{
		estadoTecla[5]=1;
	}

	if(!(GetKeyState('X') & 0x80))
	{
		estadoTecla[5]=0;
	}

	//Enter
	if(GetKeyState(VK_RETURN) & 0x80) //Si está presionada la tecla UP
	{
		estadoTecla[6]=1;
	}

	if(!(GetKeyState(VK_RETURN) & 0x80)) //Si no está presionada la tecla UP
	{
		estadoTecla[6]=0;
	}
}

void CControl::controlJoystick()
{
	ZeroMemory(&joyInfo1, sizeof(JOYINFOEX));
	joyInfo1.dwSize=sizeof(JOYINFOEX);
	joyInfo1.dwFlags=JOY_RETURNALL;

	err=joyGetPosEx(JOYSTICKID1, &joyInfo1);
	if(err!=JOYERR_NOERROR){
		switch(err){
		case MMSYSERR_NODRIVER:
			//no driver installed
			break;
		case MMSYSERR_INVALPARAM:
			//invalid parameter passed
			break;
		case MMSYSERR_BADDEVICEID:
			//joystick ID is invalid
			break;
		case JOYERR_UNPLUGGED:
			//joystick is not pluged in
			break;
		}
	}

	//int p=joyInfo1.dwPOV/100;
	int r=joyInfo1.dwRpos >> 12;
	int x=joyInfo1.dwXpos >> 12;
	int y=joyInfo1.dwYpos >> 12;
	int z=joyInfo1.dwZpos >> 12;

	if(y == 0) //arriba
		estadoTecla[0]=1;
	else
		estadoTecla[0]=0;
	
	if(y == 15) //abajo
		estadoTecla[1]=1;
	else
		estadoTecla[1]=0;

	if(x == 0) //izq
		estadoTecla[2]=1;
	else
		estadoTecla[2]=0;

	if(x == 15) //der
		estadoTecla[3]=1;
	else
		estadoTecla[3]=0;

	if(joyInfo1.dwButtons & JOY_BUTTON3) //Saltando
		estadoTecla[4]=1;
	else
		estadoTecla[4]=0;

	if(joyInfo1.dwButtons & JOY_BUTTON2) //Golpeando
		estadoTecla[5]=1;
	else
		estadoTecla[5]=0;
	
	
}

int CControl::obtieneEstadoTecla(int codigo)
{
	int v;

	v=estadoTecla[codigo];

	return v;
}