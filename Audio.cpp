#include "Audio.h"

FMOD_SYSTEM      *systemFmod;
FMOD_RESULT       result;

CAudio::CAudio()
{
	
}

CAudio::~CAudio()
{
}

int CAudio::iniciaAudio()
{
	unsigned int      version;

	unsigned int ms = 0;
	unsigned int lenms = 0;
	int          playing = 0;
	int          paused = 0;
	int          channelsplaying = 0;

	//Se inicia el sistema de sonido de FMOD
    result = FMOD_System_Create(&systemFmod);
    ERRCHECK(result);

    result = FMOD_System_GetVersion(systemFmod, &version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        return 0;
    }

    result = FMOD_System_Init(systemFmod, 32, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);
    
}

void CAudio::finalizaAudio()
{
	result = FMOD_System_Close(systemFmod);
    ERRCHECK(result);
   	result = FMOD_System_Release(systemFmod);
	ERRCHECK(result);
}

void CAudio::actualizaSistemaAudio()
{
	FMOD_System_Update(systemFmod);
}

void CSonido::cargaSonido(char *FileName, int loop)
{

	sound=0;
	channel=0;

	result = FMOD_System_CreateSound(systemFmod, FileName, FMOD_HARDWARE, 0, &sound);
    ERRCHECK(result);

	if(loop == 1)
	{
		result = FMOD_Sound_SetMode(sound, FMOD_LOOP_NORMAL); //se repite constantemente durante la reproducción
		ERRCHECK(result);										
	}

    //Se asocian los sonidos a los canales	
    result = FMOD_System_PlaySound(systemFmod, FMOD_CHANNEL_FREE, sound, 0, &channel);
    ERRCHECK(result);
    result = FMOD_Channel_SetPaused(channel, TRUE);
    ERRCHECK(result);
  
}

void CSonido::reproduceSonido()
{
	result = FMOD_System_PlaySound(systemFmod, FMOD_CHANNEL_FREE, sound, 0, &channel);
    ERRCHECK(result);
}

void CSonido::pausaSonido(bool estado)
{
	FMOD_Channel_SetPaused(channel, estado);
}

void CSonido::volumenSonido(float valor)
{
	FMOD_Channel_SetVolume(channel, valor);
}

void CSonido::liberaSonido()
{
	result = FMOD_Sound_Release(sound);
    ERRCHECK(result);
	
}

void ERRCHECK(FMOD_RESULT result)
{
    if(result != FMOD_OK)
        exit(-1);
}