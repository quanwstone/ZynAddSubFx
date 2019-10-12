#pragma once
#include "Interface.h"
#include"EffectMgr.h"

int SAMPLE_RATE = 48000;

int SOUND_BUFFER_SIZE = 240;

class CZynAddSubFx :
	public ZynAddSubFx_Interface
{
public:
	CZynAddSubFx();
	~CZynAddSubFx();
public:
	virtual void setpramater(int iChannels, int iSampleRate);

	virtual void setdryonly(bool value);

	virtual void changeeffect(int iIndex);

	virtual bool put_pcm(char *pcm, int iSize);
public:
	EffectMgr *m_peffectMgr;

	int m_iChannels;
	
	float *smpsl;
	float *smpsr;

};

