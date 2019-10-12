/*
ZynAddSubFX - a software synthesizer

EffectMgr.h - Effect manager, an interface betwen the program and effects
Copyright (C) 2002-2005 Nasca Octavian Paul
Author: Nasca Octavian Paul

This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License 
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License (version 2) for more details.

You should have received a copy of the GNU General Public License (version 2)
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#ifndef EFFECTMGR_H
#define EFFECTMGR_H
#include "Effect.h"

#define DEF_AUDIO_VOL_STEP_CNT	6
#define DEF_AUDIO_LEFT_CHANNEL	0
#define DEF_AUDIO_RIGHT_CHANNEL	1
#define DEF_AUDIO_MAX_VOLUME	32767
#define DEF_AUDIO_MIN_VOLUME	(-32767)

class EffectMgr
{
public:
	//EffectMgr(int insertion_,pthread_mutex_t *mutex_);
	EffectMgr(int insertion_);
	~EffectMgr();
	/*
	void add2XML(XMLwrapper *xml);
	void getfromXML(XMLwrapper *xml);
	*/
	void defaults();

	//不使用特效
	void setdryonly(bool value);

	bool getdryonly() {return dryonly;	}

	//get the output(to speakers) volume of the systemeffect
	REALTYPE sysefxgetvolume();

	void cleanup();//cleanup the effect

	//更换效果/*1:Reverb 2:Echo 3:Chorus 4:Phaser 5:Alienwah:6:Distorsion 7:EQ */
	void changeeffect(int nefx_);

	//得到效果
	int geteffect();
	void changepreset(unsigned char npreset);
	void changepreset_nolock(unsigned char npreset);
	unsigned char getpreset();
	
	//设置效果参数
	void seteffectpar(int npar,unsigned char value);
	void seteffectpar_nolock(int npar,unsigned char value);//sets the effect par without thread lock
	
	//得到效果参数
	unsigned char geteffectpar(int npar);
	int insertion;//1 if the effect is connected as insertion effect
	
	REALTYPE *efxoutl,*efxoutr;

	//used by UI
	REALTYPE getEQfreqresponse(REALTYPE freq);

	FilterParams *filterpars;
	//REALTYPE *denormalkillbuf;

	//混音函数,对超出极限的音量及下16个点音量进行衰减
	REALTYPE MixAudio(REALTYPE aSampleIn,REALTYPE aSampleOut,float* aVolPercent);

public:
	//设置干音、湿音系数，可单独设置
	void SetDryWetCoeff(float afDry, float afWet);

	//Apply the effect，返回FALSE时无法
	bool out(REALTYPE *smpsl,REALTYPE *smpsr);

private:
	int nefx;
	Effect *efx;
	//	pthread_mutex_t *mutex;
	bool dryonly;

	float m_fSetpValue[DEF_AUDIO_VOL_STEP_CNT];	//声音下降百分比表
	float m_fVolPercent[2];	//左右声道当前下降值

private:
	float m_dryCoeff;		//干音系数
	float m_wetCoeff;		//湿音系数
};
#endif



