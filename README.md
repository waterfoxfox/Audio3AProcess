# audio 3A (AGC\AEC\ANS\VAD) process with simple api and single lib


## Audio3AProcessSdk简介
极简的音频3A(AGC AEC ANS VAD)处理封装接口：

* 1、基于Webrtc AEC（非AECM、非AEC3）
* 2、支持8KHZ、16KHZ、32KHZ、44.1KHZ、48KHZ，支持单声道、双声道。
* 3、支持AEC自动延时估计。
* 4、仅6个API，仅一个DLL组成，占用空间小，无第三方依赖，集成简易。
* 5、C++开发，支持C、C++、C#，可移植Android等平台

Webrtc中关于延时自动估计的说明：<br>
WebRtc is very dependent on delay calculation, a poor estimate, even by as little as
40ms, may affect the echo cancellation results greatly. Still, with the delay agnostic feature, it may take some time (5-10s or more) for
the Aec module to learn the optimal delay, thus a good initial estimate is necessary for good EC quality in the beginning of a call.
自动延时估计需要一定的收敛时间，通过SD3AProcess_EnableDebugMode接口生成的Ref文件、Mic文件，使用cool edit确定二者延时作为初始值传入，
将有利于尽快收敛获得较好效果。<br><br>

#### 集成了采集和渲染的3A一体库：https://github.com/waterfoxfox/Audio3ACapRender

## Audio3AProcessSdk C API

### 
* 环境初始化，系统只需调用一次<br>
@param: outputPath：日志文件输出的目录，若目录不存在将自动创建<br>
@param: outputLevel：日志输出的级别，只有等于或者高于该级别的日志输出到文件<br>
@return: <br>
void  `SD3AProcess_Enviroment_Init`(const char * outputPath,  int outputLevel);

### 
* 环境反初始化，系统只需调用一次<br>
@return:<br>
void  `SD3AProcess_Enviroment_Free`();

### 
* 创建SD3AProcess<br>
@return: 返回模块指针，为NULL则失败<br>
void*  `SD3AProcess_New`();

### 
* 销毁SD3AProcess，使用者应该做好与其他API之间的互斥保护<br>
@param pp3AProcess: 模块指针<br>
@return: <br>
void  `SD3AProcess_Delete`(void** pp3AProcess);

### 
* 开始启动SD3AProcess，仅支持16bit short音频3A处理<br>
@param p3AProcess: 模块指针<br>
@param nSampleRate: 输入待处理数据采样率<br>
@param nChannelNum: 输入待处理数据声道数<br>
@param nAecDelayInitMs: MIC信号与REF扬声器输出信号之间的延时差，将作为内部AEC自动延时估计的初始值。建议通过SD3AProcess_EnableDebugMode接口保存MIC、REF文件后观测得到准确的延时估计初始值有利于自动延时估计尽快收敛<br>
@param bEnableAec: 是否使能AEC<br>
@param bEnableAgc: 是否使能AGC<br>
@param bEnableAns: 是否使能ANS<br>
@param pfOutput3ACallback: 经过3A处理后的音频数据输出回调接口<br>
@param pfVadStatusCallback: VAD检测的输出回调接口，设置为NULL时不开启VAD检测<br>
@param pObject: 上述输出回调接口的透传指针，将通过回调函数形参方式透传外层<br>
@return: TURE成功，FALSE失败<br>
BOOL  `SD3AProcess_Start`(void* p3AProcess, int nSampleRate, int nChannelNum, int nAecDelayInitMs, BOOL bEnableAec, BOOL bEnableAgc, BOOL bEnableAns, Output3AProcessedData pfOutput3ACallback, OutputVadProcessStatus pfVadStatusCallback, void* pObject);

### 
* 停止SD3AProcess<br>
@param p3AProcess: 模块指针<br>
@return: <br>
void  `SD3AProcess_Stop`(void* p3AProcess);


### 
* 存入REF扬声器数据作为AEC处理参考信号<br>
@param p3AProcess: 模块指针<br>
@param psRefData: REF扬声器数据<br>
@param nRefCount: REF扬声器数据大小（非字节数，而是psRefData 中 short数据的数目）<br>
@return: <br>
void  `SD3AProcess_PutRefData`(void* p3AProcess, const short *psRefData, int nRefCount);

### 
* 存入待3A处理的麦克风采集信号<br>
@param p3AProcess: 模块指针<br>
@param psMicData: MIC数据<br>
@param nMicCount: MIC数据大小（非字节数，而是psRefData 中 short数据的数目）<br>
@return: <br>
void  `SD3AProcess_PutMicData`(void* p3AProcess, const short *psMicData, int nMicCount);

### 
* 启用AEC调试模式，此时将生成AEC处理前的Ref信号和Mic信号到指定的路径，便于观察二者延时差<br>
@param p3AProcess: 模块指针<br>
@param pcTempFileSaveDir: 调试文件存放路径<br>
@return: TRUE-使能成功， FALSE-使能失败<br>
BOOL  `SD3AProcess_EnableDebugMode`(void* p3AProcess, const char *pcTempFileSaveDir);

### 本库仅做演示用途，若需要商业用途与技术支持请联系 www.mediapro.cc
