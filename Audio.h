#ifndef _AUDIO_H
#define _AUDIO_H

#include "Main.h"

#pragma comment(lib, "fmod/fmodex_vc.lib" )

void ERRCHECK(FMOD_RESULT result);

class CAudio
{
	public:
		CAudio();
		~CAudio();
		int iniciaAudio();
		void actualizaSistemaAudio();
		void finalizaAudio();
		
	private:

};

class CSonido
{
	public:
		CSonido() {}
		~CSonido() {}
		void cargaSonido(char *FileName, int loop);
		void reproduceSonido();
		void pausaSonido(bool estado);
		void volumenSonido(float valor); //entre 0 y 1
		void liberaSonido();

	private:
		FMOD_SOUND   *sound;
		FMOD_CHANNEL *channel;
};

#endif 