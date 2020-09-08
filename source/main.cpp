//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：音频3A处理演示DEMO
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2020-5-18
//**************************************************************************//
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef WIN32
#include <shlwapi.h>
#pragma comment(lib, "shlwapi")
#endif

#include "SDAecAgcAnsProcessSdk.h"




static void SD_Sleep(DWORD dwMilliseconds)
{
#ifdef WIN32
	Sleep(dwMilliseconds);
#else
	// usleep(dwMilliseconds*1000);
	// POSIX has both a usleep() and a nanosleep(), but the former is deprecated,
	// so we use nanosleep() even though it has greater precision than necessary.
	struct timespec ts;
	ts.tv_sec = dwMilliseconds / 1000;
	ts.tv_nsec = (dwMilliseconds % 1000) * 1000000;
	int ret = nanosleep(&ts, NULL);
	if (ret != 0)
	{
		static int nCount = 0;
		if ((nCount % 5000) == 0)
		{
			SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "nanosleep() returning early!!!");
		}
		nCount++;

		usleep(dwMilliseconds * 1000);
	}
#endif
}


//3A处理输出回调
void Output3AProcessedDataFunc(short *data, int len, void *pObject)
{
	FILE *pfOutput = (FILE *)pObject;
	if (pfOutput)
	{
		fwrite(data, sizeof(short), len, pfOutput);
	}
}


int main(int argc, char **argv)
{
    FILE *fref = NULL;
    FILE *fmic = NULL;
    FILE *faec = NULL;
	short* sref = NULL;
	short* smic = NULL;
	void* h3A = NULL;

	//单次读入的参考信号、麦克信号样点数
    int framesamples = 0;

    int samplerate = 0;
    int delay = 0;
    int channels = 1;
    
    if (argc != 7) 
    {
        printf("usage: 3Aprocess.exe mic.pcm ref.pcm aec.pcm samplerate channels delay(ms)\n");
        return -1;
    }

	//支持的常规采样率
    samplerate = atoi(argv[4]);
    if (samplerate != 8000  && samplerate != 16000 && samplerate != 32000 && samplerate != 48000 && samplerate != 44100)
    {
        printf("samplerate %d unsupported(%s)\n", samplerate, argv[4]);
        return -1;
    }

	//支持的声道数
    channels = atoi(argv[5]);
    if ((channels != 1) && (channels != 2))
    {
        printf("channels %d unsupported(%s)\n", channels, argv[5]);
        return -1;
    }
    
	//初始延时估计值，若不能准确提供，可设置为默认值0
    delay = atoi(argv[6]);


	//相关文件读写打开
	fmic = fopen(argv[1], "rb");
	if (NULL == fmic)
	{
		printf("fopen %s failed\n", argv[1]);
		goto exitproc;
	}

	fref = fopen(argv[2], "rb");
	if (NULL == fref)
	{
		printf("fopen %s failed\n", argv[2]);
		goto exitproc;
	}

	faec = fopen(argv[3], "wb");
	if (NULL == faec)
	{
		printf("fopen %s failed\n", argv[3]);
		goto exitproc;
	}

	//单次读取的样点数, 假设为10ms
	framesamples = samplerate / 100;
	//分配临时缓存
	sref = (short*)malloc(sizeof(short) * framesamples * channels);
	smic = (short*)malloc(sizeof(short) * framesamples * channels);
	if ((sref == NULL) || (smic == NULL))
	{
		printf("malloc %d failed\n", sizeof(short) * framesamples * channels);
		goto exitproc;
	}


	//初始化SDK日志输出级别、目录
	SD3AProcess_Enviroment_Init("./log", AUDIO3ALOG_OUTPUT_LEVEL_INFO);


	//创建3A对象
	h3A = SD3AProcess_New();
    if (NULL == h3A)
    {
        printf("SD3AProcess_New failed!\n");
		goto exitproc;
    }

	//SD3AProcess_EnableDebugMode(h3A, "./debug");

	//启动3A对象
	BOOL bRet = SD3AProcess_Start(h3A, samplerate, channels, delay, TRUE, TRUE, TRUE, Output3AProcessedDataFunc, NULL, faec);
	if (bRet == FALSE)
	{
		goto exitproc;
	}


    while(1)
    {
		//读文件模拟获取参考信号
        if (fread(sref, sizeof(short), framesamples * channels, fref) != framesamples * channels)
        {
            printf("process all data, exiting...\n");
            goto exitproc;
        }

		//读文件模拟获取麦克信号
        if (fread(smic, sizeof(short), framesamples * channels, fmic) != framesamples * channels)
        {
            printf("process all data, exiting...\n");
            goto exitproc;
        }

		//送3A对象处理，处理后数据通过回调输出
		SD3AProcess_PutRefData(h3A, sref, framesamples * channels);
		SD3AProcess_PutMicData(h3A, smic, framesamples * channels);

		Sleep(10);
    }

exitproc:
    if (fref)
    {
        fclose(fref);
    }
    if (fmic)
    {
        fclose(fmic);
    }
	if (faec)
	{
		fclose(faec);
	}

	if (sref)
	{
		free(sref);
	}
	if (smic)
	{
		free(smic);
	}

	//资源回收
	SD3AProcess_Delete(&h3A);

	SD3AProcess_Enviroment_Free();

    return 0;
}



