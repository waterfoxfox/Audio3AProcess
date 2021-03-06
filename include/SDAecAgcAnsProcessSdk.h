﻿//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：音频3A处理对外DLL封装接口
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2020-5-18
//**************************************************************************//

#ifndef _SD_AUDIO_3A_PROCESS_SDK_H_
#define _SD_AUDIO_3A_PROCESS_SDK_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#ifndef BUILDING_DLL
#define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
#define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */
#else
#define DLLIMPORT
#endif

#ifdef __APPLE__
#ifndef OBJC_BOOL_DEFINED
typedef int BOOL;
#endif 
#else
#ifndef BOOL
typedef int BOOL;
#endif
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

//日志输出的级别
typedef enum AUDIO3A_LOG_OUTPUT_LEVEL
{
	AUDIO3ALOG_OUTPUT_LEVEL_DEBUG = 1,
	AUDIO3ALOG_OUTPUT_LEVEL_INFO,
	AUDIO3ALOG_OUTPUT_LEVEL_WARNING,
	AUDIO3ALOG_OUTPUT_LEVEL_ERROR,
	AUDIO3ALOG_OUTPUT_LEVEL_ALARM,
	AUDIO3ALOG_OUTPUT_LEVEL_FATAL,
	AUDIO3ALOG_OUTPUT_LEVEL_NONE
} AUDIO3A_LOG_OUTPUT_LEVEL;

//3A处理后的数据输出回调
typedef void (*Output3AProcessedData)(short *data, int len, void *pObject);
//若开启了VAD，通过本回调输出当前VAD状态
typedef void (*OutputVadProcessStatus)(BOOL bInVoiceStatus, void *pObject);

//////////////////////////////////////////////////////////////////////////
// 音频3A处理封装接口

/***
* 环境初始化，系统只需调用一次，主要用于日志模块的初始化
* @param: outputPath表示日志存放路径，支持相对路径和绝对路径，若目录不存在将自动创建
* @param: outputLevel表示日志输出的级别，只有等于或者高于该级别的日志输出到文件，取值范围参考3A_LOG_OUTPUT_LEVEL
* @return: 
*/
DLLIMPORT void  SD3AProcess_Enviroment_Init(const char* outputPath, int outputLevel);

DLLIMPORT void  SD3AProcess_Enviroment_Free();



/***
* 创建SD3AProcess对象
* @return: 返回模块指针，为NULL则失败
*/
DLLIMPORT void*  SD3AProcess_New();


/***
* 销毁SD3AProcess，使用者应该做好与其他API之间的互斥保护
* @param pp3AProcess: 模块指针指针
* @return:
*/
DLLIMPORT void  SD3AProcess_Delete(void** pp3AProcess);



/***
* 开始启动SD3AProcess，仅支持16bit short音频3A处理
* @param p3AProcess: 模块指针
* @param nSampleRate: 输入待处理数据采样率
* @param nChannelNum: 输入待处理数据声道数
* @param nAecDelayInitMs: MIC信号与REF扬声器输出信号之间的延时差，将作为内部AEC自动延时估计的初始值。
*                         建议通过SD3AProcess_EnableDebugMode接口保存MIC、REF文件后观测得到
*                         准确的延时估计初始值有利于自动延时估计尽快收敛
* @param bEnableAec: 是否使能AEC
* @param bEnableAgc: 是否使能AGC
* @param bEnableAns: 是否使能ANS
* @param pfOutput3ACallback: 经过3A处理后的音频数据输出回调接口
* @param pfVadStatusCallback: VAD检测的输出回调接口，设置为NULL时不开启VAD检测
* @param pObject: 上述输出回调接口的透传指针，将通过回调函数形参方式透传外层
* @return: TURE成功，FALSE失败
*/
DLLIMPORT BOOL  SD3AProcess_Start(void* p3AProcess, int nSampleRate, int nChannelNum, int nAecDelayInitMs, BOOL bEnableAec, BOOL bEnableAgc, BOOL bEnableAns,
									Output3AProcessedData pfOutput3ACallback, OutputVadProcessStatus pfVadStatusCallback, void* pObject);



/***
* 停止SD3AProcess
* @param p3AProcess: 模块指针
* @return:
*/
DLLIMPORT void  SD3AProcess_Stop(void* p3AProcess);


/***
* 存入REF扬声器数据作为AEC处理参考信号
* @param p3AProcess: 模块指针
* @param psRefData: REF扬声器数据
* @param nRefCount: REF扬声器数据大小（非字节数，而是psRefData 中 short数据的数目）
* @return: TURE成功，FALSE失败
*/
DLLIMPORT BOOL  SD3AProcess_PutRefData(void* p3AProcess, const short *psRefData, int nRefCount);


/***
* 存入待3A处理的麦克风采集信号
* @param p3AProcess: 模块指针
* @param psMicData: MIC数据
* @param nMicCount: MIC数据大小（非字节数，而是psRefData 中 short数据的数目）
* @return: TURE成功，FALSE失败
*/
DLLIMPORT BOOL  SD3AProcess_PutMicData(void* p3AProcess, const short *psMicData, int nMicCount);



/***
* 启用AEC调试模式，此时将生成AEC处理前的Ref信号和Mic信号到指定的路径，便于观察二者延时差
* @param p3AProcess: 模块指针
* @param pcTempFileSaveDir: 调试文件存放路径
* @return: TRUE-使能成功， FALSE-使能失败
*/
DLLIMPORT BOOL  SD3AProcess_EnableDebugMode(void* p3AProcess, const char *pcTempFileSaveDir);




#ifdef __cplusplus
}
#endif

#endif // _SD_AUDIO_3A_PROCESS_SDK_H_
