#include "stdafx.h"
#include "ZynAddSubFx.h"

ZynAddSubFx_Interface *CreateInstance()
{
	ZynAddSubFx_Interface *p = new CZynAddSubFx();

	return p;
}

void DestoryInstance(ZynAddSubFx_Interface *p)
{
	if (p != nullptr)
	{
		ZynAddSubFx_Interface *pZyn = p;
		
		delete pZyn;
	}

}

CZynAddSubFx::CZynAddSubFx()
{
	m_peffectMgr = new EffectMgr(1);

	m_iChannels = 1;

	smpsl = new float[SOUND_BUFFER_SIZE];
	smpsr = new float[SOUND_BUFFER_SIZE];
}


CZynAddSubFx::~CZynAddSubFx()
{
	if (m_peffectMgr != nullptr)
		delete m_peffectMgr;

	if (smpsl != nullptr)
		delete[] smpsl;

	if (smpsr != nullptr)
		delete[] smpsr;
}

void CZynAddSubFx::setpramater(int iChannels, int iSampleRate)
{
	m_iChannels = iChannels;
	
	SAMPLE_RATE = iSampleRate;

}
void CZynAddSubFx::setdryonly(bool value)
{
	m_peffectMgr->setdryonly(value);
}

void CZynAddSubFx::changeeffect(int iIndex)
{
	m_peffectMgr->changeeffect(iIndex);
}

bool CZynAddSubFx::put_pcm(char * pcm, int iSize)
{
	if (pcm == nullptr)
		return false;

	float *pSr = (float *)pcm;

	int iLoop = iSize / (sizeof(float) *m_iChannels * SOUND_BUFFER_SIZE);

	for (int i = 0; i < iLoop; i++)
	{
		float *pWrite = pSr;

		for (int j = 0; j < SOUND_BUFFER_SIZE; j++)
		{
			smpsl[j] = (float)*pSr;
			++pSr;
			smpsr[j] = (float)*pSr;
			++pSr;
		}

		bool br = m_peffectMgr->out(smpsl, smpsr);
		if (br == false)
		{
			return false;
		}

		int iIndex = 0;
		for (int j = 0; j < SOUND_BUFFER_SIZE; j++)
		{
			pWrite[iIndex] = smpsl[j];
			iIndex++;
			pWrite[iIndex] = smpsr[j];
			iIndex++;
		}
	}

	return true;
}
