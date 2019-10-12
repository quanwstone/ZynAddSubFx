/*
ZynAddSubFX - a software synthesizer

EffectMgr.C - Effect manager, an interface betwen the program and effects
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

#include <stdlib.h>
#include <stdio.h>
#include "EffectMgr.h"


#include "Reverb.h"
#include "Echo.h"
#include "Chorus.h"
#include "Phaser.h"
#include "Alienwah.h"
#include "Distorsion.h"
#include "EQ.h"
#include "DynamicFilter.h"

#define min(a,b)    (((a) < (b)) ? (a) : (b))

EffectMgr::EffectMgr(int insertion_){

	m_dryCoeff = 0.8;		//干音系数
	m_wetCoeff = 0.4;		//湿音系数

	//setpresettype("Peffect");
	efx=NULL;
	nefx=0;
	insertion=insertion_;
	//mutex=mutex_;
	efxoutl=new REALTYPE[SOUND_BUFFER_SIZE];
	efxoutr=new REALTYPE[SOUND_BUFFER_SIZE];

	//denormalkillbuf=new REALTYPE [SOUND_BUFFER_SIZE];    
	//for (int i=0;i<SOUND_BUFFER_SIZE;i++) denormalkillbuf[i]=(RND-0.5)*1e-16;

	for (int i=0;i<SOUND_BUFFER_SIZE;i++){
		efxoutl[i]=0.0;
		efxoutr[i]=0.0;
	};
	filterpars=NULL;
	dryonly=false;
	defaults();

	for (int i=0 ;i< DEF_AUDIO_VOL_STEP_CNT ;i++)
	{
		m_fSetpValue[i] = 0.4 + i*0.1;
	}
	m_fVolPercent[DEF_AUDIO_LEFT_CHANNEL] = 1.0;
	m_fVolPercent[DEF_AUDIO_RIGHT_CHANNEL] = 1.0;
};


EffectMgr::~EffectMgr()
{
	if (efx!=NULL) delete (efx);
	delete (efxoutl);
	delete (efxoutr);
};

void EffectMgr::defaults(){
	changeeffect(0);
	setdryonly(false);
};

/*
* Change the effect
*/
void EffectMgr::changeeffect(int nefx_){
	cleanup();
	if (nefx==nefx_) return;
	nefx=nefx_;
	for (int i=0;i<SOUND_BUFFER_SIZE;i++){
		efxoutl[i]=0.0;
		efxoutr[i]=0.0;
	};

	if (efx!=NULL) delete (efx);
	switch (nefx){
	case 1:efx=new Reverb(insertion,efxoutl,efxoutr);break;
	case 2:efx=new Echo(insertion,efxoutl,efxoutr);break;
	case 3:efx=new Chorus(insertion,efxoutl,efxoutr);break;
	case 4:efx=new Phaser(insertion,efxoutl,efxoutr);break;
	case 5:efx=new Alienwah(insertion,efxoutl,efxoutr);break;
	case 6:efx=new Distorsion(insertion,efxoutl,efxoutr);break;
	case 7:efx=new EQ(insertion,efxoutl,efxoutr);break;
	//case 8:efx=new DynamicFilter(insertion,efxoutl,efxoutr);break;
		//put more effect here
	default:efx=NULL;break;//no effect (thru)
	};

	if (efx!=NULL) filterpars=efx->filterpars;
};

/*
* Obtain the effect number
*/
int EffectMgr::geteffect(){
	return (nefx);
};

/*
* Cleanup the current effect
*/
void EffectMgr::cleanup(){
	if (efx!=NULL) efx->cleanup();
};


/*
* Get the preset of the current effect
*/

unsigned char EffectMgr::getpreset(){
	if (efx!=NULL) return(efx->Ppreset);
	else return(0);
};

/*
* Change the preset of the current effect //preset:初始状态
*/
void EffectMgr::changepreset_nolock(unsigned char npreset){
	if (efx!=NULL) efx->setpreset(npreset);
};

/*
* Change the preset of the current effect(with thread locking)
*/
void EffectMgr::changepreset(unsigned char npreset){
	// pthread_mutex_lock(mutex);
	changepreset_nolock(npreset);
	// pthread_mutex_unlock(mutex);
};


/*
* Change a parameter of the current effect 
*/
void EffectMgr::seteffectpar_nolock(int npar,unsigned char value){
	if (efx==NULL) return;
	efx->changepar(npar,value);
};

/*
* Change a parameter of the current effect (with thread locking)
*/
void EffectMgr::seteffectpar(int npar,unsigned char value){
	//pthread_mutex_lock(mutex);
	seteffectpar_nolock(npar,value);
	//pthread_mutex_unlock(mutex);
};

/*
* Get a parameter of the current effect
*/
unsigned char EffectMgr::geteffectpar(int npar){
	if (efx==NULL) return(0);
	return(efx->getpar(npar));
};

bool EffectMgr::out(REALTYPE *smpsl,REALTYPE *smpsr)
{
	//混响处理中：v1是干音所乘的系数，v2是湿音所所乘的系数
	REALTYPE v1,v2;
	//此时针对的是干音、湿音系数由外界设定，故上面关于非线性的代码也是不需要的
	v1 = m_dryCoeff;
	v2 = m_wetCoeff;

	if (efx==NULL)
	{
		return false;
	};
	efx->out(smpsl,smpsr);

	//this is need only for the EQ effect
	//当使用EQ均衡器时，直接赋值，无需执行后面的混音代码了
	int i = 0;
	if (nefx==7)	
	{
		for (i=0; i<SOUND_BUFFER_SIZE; i++)
		{
			smpsl[i]=efxoutl[i];
			smpsr[i]=efxoutr[i];
		};
		return true;
	};

	//正常插入音效的情况，根据干音、湿音所占的比例进行计算得到结果
	for (i=0;i<SOUND_BUFFER_SIZE;i++)
	{
		smpsl[i]=smpsl[i]*v1+efxoutl[i]*v2;
		smpsr[i]=smpsr[i]*v1+efxoutr[i]*v2;
	};
	return true;
};

REALTYPE EffectMgr::MixAudio(REALTYPE aSampleIn,REALTYPE aSampleOut,float* aVolPercent)
{
	REALTYPE MixValue = 0;

	MixValue = aSampleIn + aSampleOut;

	MixValue = MixValue*(*aVolPercent);
	if(MixValue >= DEF_AUDIO_MAX_VOLUME)
	{
		for(int j=DEF_AUDIO_VOL_STEP_CNT-1 ;j>=0 ;j--)
		{
			if(MixValue*m_fSetpValue[j] < DEF_AUDIO_MAX_VOLUME)
			{
				(*aVolPercent) = m_fSetpValue[j];
				break;

			}
		}
		MixValue = DEF_AUDIO_MAX_VOLUME;
	}
	else if(MixValue <= DEF_AUDIO_MIN_VOLUME)
	{
		for(int j=DEF_AUDIO_VOL_STEP_CNT-1 ;j>=0 ;j--)
		{
			if(MixValue*m_fSetpValue[j] > DEF_AUDIO_MIN_VOLUME)
			{
				(*aVolPercent) = m_fSetpValue[j];
				break;
			}
		}
		MixValue = DEF_AUDIO_MIN_VOLUME;
	}

	if((*aVolPercent) < 1.0)
	{
		(*aVolPercent) = (*aVolPercent)+(1.0 - (*aVolPercent))/16;

		(*aVolPercent) = min(1.0,(*aVolPercent));
	}

	return MixValue;
}

/*
* Get the effect volume for the system effect
*/
REALTYPE EffectMgr::sysefxgetvolume()
{
	if (efx==NULL) 
		return (1.0);
	else
		return(efx->outvolume);    
};


/*
* Get the EQ response
*/
REALTYPE EffectMgr::getEQfreqresponse(REALTYPE freq)
{
	if (nefx==7) 
		return(efx->getfreqresponse(freq));
	else 
		return(0.0);
};

void EffectMgr::setdryonly(bool value)
{
	dryonly=value;
};

//设置干音、湿音系统，可单独设置
void EffectMgr::SetDryWetCoeff(float afDry, float afWet)
{
	m_dryCoeff = afDry;		//干音系数
	m_wetCoeff = afWet;		//湿音系数
}

