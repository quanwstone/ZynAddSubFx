#pragma once
class ZynAddSubFx_Interface
{
public: 

	//设置参数
	virtual void setpramater(int iChannels,int iSampleRate) = 0;

	//设置启用特效.
	virtual void setdryonly(bool value) = 0;

	//设置特效类型
	virtual void changeeffect(int iIndex) = 0;

	//传递数据
	virtual bool put_pcm(char *pcm, int iSize) = 0;

};
extern "C" __declspec(dllexport) ZynAddSubFx_Interface *CreateInstance();

extern "C" __declspec(dllexport) void DestoryInstance(ZynAddSubFx_Interface *p);
