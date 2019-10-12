#include<stdio.h>
#include<Windows.h>
#include"../ZynAddSubFX/Interface.h"

#pragma comment(lib,"ZynAddSubFX.lib")

int main(int arvc, char *argv[])
{
	FILE *pPcm = nullptr;
	FILE *pEff = nullptr;

	errno_t er = fopen_s(&pPcm, "../Pcm/6.pcm", "rb+");
	if (er != 0)
	{
		printf("fopen_s failed.\n");
	}
	er = fopen_s(&pEff, "e.pcm", "wb+");
	if (er != 0)
	{
		printf("fopen_s failed.\n");
	}

	DWORD dwSamplesPerSec = 44100;
	WORD wChannels = 2;
	WORD wBitsPerSample = 32;

	ZynAddSubFx_Interface *p = CreateInstance();
	if (p == nullptr)
	{
		printf("");
	}

	p->setdryonly(false);

	p->setpramater(wChannels, dwSamplesPerSec);

	p->changeeffect(1);
	
	int iLen = 220 * 2 * 2 * 20;

	char *buf = new char[iLen];
	memset(buf, 0, iLen);

	int iR = fread(buf, 1, iLen, pPcm);

	float smpsl[240] = { 0 };
	float smpsr[240] = { 0 };

	while (iR)
	{
		p->put_pcm(buf, iR);

		fwrite(buf, 1, iR, pEff);

		iR = fread(buf, 1, iLen, pPcm);
	}

	fclose(pPcm);
	fclose(pEff);

	DestoryInstance(p);

	return 0;
}


