//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	          直接音频播放控制函数部分
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2009-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _DIRECT_FFMPEG_H
#define _DIRECT_FFMPEG_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

#include <ffmpeg/libavutil/avstring.h>
#include <ffmpeg/libavformat/avformat.h>
#include <ffmpeg/libswscale/swscale.h>
#include <ffmpeg/libavcodec/opt.h>


// 此结构体兼容以前的音频结构体，请勿对结构体做任何修改!!!
typedef struct _AUDIO_INFO
{
	int Type;
	DWORD TotalTime;
	DWORD Bitrate;
	DWORD Samplerate;
}AUDIO_INFO;
typedef AUDIO_INFO *PAUDIO_INFO;

// 此结构体兼容以前的视频结构体，请勿对结构体做任何修改!!!
typedef struct _VIDEO_INFO
{
	int Type;
	int MovieWidth;
	int MovieHeight;
	int FPS;
	DWORD TotalTime;
	DWORD VideoBitrate;
	DWORD AudioBitrate;
}VIDEO_INFO;
typedef VIDEO_INFO *PVIDEO_INFO;

typedef struct _MEDIA_INFO
{
	int MediaType;
	char bHasAudio;
	char bHasVideo;
	char bAllowSeek;
	DWORD TotalTime;
	
	int AudioCodec;
	int AudioBitrate;
	int AudioSamplerate;
	int AudioChannels;

	int VideoCodec;
	int VideoWidth;
	int VideoHeight;
	int VideoFps;
	int VideoBitrate;
}MEDIA_INFO;
typedef MEDIA_INFO *PMEDIA_INFO;


typedef struct _MEDIA_TASK
{
	DWORD Version;			// 版本
	HWND hWnd;				// 视频消息发送窗体
	int Record;				// 视频任务 0-播放 1-录制
	int MediaType;			// 视频媒体类型
	
	char *FileName;			// 视频文件名称
	BYTE *DataBuf;			// 视频数据缓冲区
	int BufSize;			// 视频缓冲区大小
	DWORD PlayOffset;		// 视频播放起始偏移
	DWORD PlayLength;		// 视频播放数据长度
	HANDLE hCallback;		// 回调函数句柄
	
	RECT ShowRect;			// 视频显示矩形
	int Volume;				// 音频录放音量
	int Tempo;				// 音频播放速度
	int Eq;					// 音频播放EQ模式
	int Duration;			// 录制时间
	int Position;			// 播放时间
	int Degree;				// 视频显示角度

	DWORD ABitrate;			// 音频录制波特率
	DWORD ASamplerate;		// 音频录制采样率
	DWORD AChannels;		// 音频录制声道数
	DWORD VBitrate;			// 视频录制波特率
	DWORD Device;			// 录放设备定义：MIC/LINE-IN
	
	int ExterdType;			// 附加数据类型
	int ExtendSize;			// 附加数据大小
	void *Extend;			// 附加数据
}MEDIA_TASK;
typedef MEDIA_TASK *PMEDIA_TASK;


typedef struct _FFMPEG_FILE
{
	int PlayOffset;			// 播放数据偏移
	int PlayLength;			// 播放数据长度
	int vOffset;			// 虚拟文件/缓冲区偏移
	
	FILE *File;				// 媒体数据文件句柄
	
	BYTE *uBuf;				// 用户缓冲区指针
	int uSize;				// 用户缓冲区大小
	int uValid;				// 用户缓冲区中有效数据
	int uOffset;			// 用户缓冲区数据虚拟偏移
	HANDLE uCb;				// 回调函数句柄

#if defined(CONFIG_MMU_ENABLE)
	int Asid;				// 用于读取用户空间数据使用
#endif	
}FFMPEG_VFILE;
typedef FFMPEG_VFILE *PFFMPEG_VFILE;

typedef struct _FFMPEG_CTRL
{
	LIST Link;
	int Code;
	DWORD *Param;
	int Size;
	int Return;
	HANDLE hSema;
	HANDLE hMutex;
	DWORD ParamData;
}FFMPEG_CTRL;
typedef FFMPEG_CTRL *PFFMPEG_CTRL;


typedef struct _FFMPEG_OBJECT
{
	KERNEL_OBJECT Header;	// 内核对象信息
	LIST Link;
	char bGetInfo;
	char bRecPause;
	char bClose;
	char bTerminate;
	int *bReady;
	HANDLE hMedia;		// 播放库句柄
	HANDLE hDac;		// DAC设备句柄
	MEDIA_TASK Task;
	FFMPEG_CTRL Ctrl;		// 控制信息
	FFMPEG_VFILE File;		// 数据缓冲区管理结构体	
	MEDIA_INFO Info;	// 数据信息结构体
	
	// 录音相关设置
	DWORD CurTime;
	
	// FFMPEG相关
	int paused;
	int last_paused;
	int seek_req;
	int seek_flags;
	int64_t seek_pos;
	AVFormatContext *ic;
	
	int audio_stream;
	AVStream *audio_st;
	
	int video_stream;
	AVStream *video_st;
}FFMPEG_OBJECT;
typedef FFMPEG_OBJECT *PFFMPEG_OBJECT;

// 设置播放文件类型
#define FFMPEG_PLAY_TYPE_UNKNOWN		0
#define FFMPEG_PLAY_TYPE_MIDI			1
#define FFMPEG_PLAY_TYPE_MP3			2
#define FFMPEG_PLAY_TYPE_AMR			3
#define FFMPEG_PLAY_TYPE_WAVE			4
#define FFMPEG_PLAY_TYPE_WMA			5
#define FFMPEG_PLAY_TYPE_PCM			6
#define FFMPEG_PLAY_TYPE_MELODY			7
#define FFMPEG_PLAY_TYPE_MP3_I			8
#define FFMPEG_PLAY_TYPE_MP3_II			9
#define FFMPEG_PLAY_TYPE_AAC			10
#define FFMPEG_PLAY_TYPE_NES			11
#define FFMPEG_PLAY_TYPE_BT				12
#define FFMPEG_PLAY_TYPE_SNES			13
#define FFMPEG_PLAY_TYPE_LPC_WMA		14
#define FFMPEG_PLAY_TYPE_FLAC_NATIVE	15
#define FFMPEG_PLAY_TYPE_FLAC_OGG		16

// 设置录音文件类型
#define FFMPEG_REC_TYPE_WAVE			0
#define FFMPEG_REC_TYPE_PCM    			101
#define FFMPEG_REC_TYPE_AMR				102
#define FFMPEG_REC_TYPE_MP3				103
#define FFMPEG_REC_TYPE_UNKOWN			255

// 播放控制码
#define FFMPEG_CTRL_PLAY			0x0001		// 播放
#define FFMPEG_CTRL_PAUSE			0x0002		// 暂停
#define FFMPEG_CTRL_RESUME			0x0003		// 继续
#define FFMPEG_CTRL_STOP			0x0004		// 停止
#define FFMPEG_CTRL_CLOSE			0x0005		// 关闭
#define FFMPEG_CTRL_FORWARD			0x0006		// 快进
#define FFMPEG_CTRL_BACKWARD		0x0007		// 快退
#define FFMPEG_CTRL_SEEK			0x0008		// 定位
#define FFMPEG_CTRL_SETAB			0x0009		// 设置AB点
#define FFMPEG_CTRL_SETEQ			0x000A		// 设置均衡
#define FFMPEG_CTRL_DURATION		0x000B		// 返回时间
#define FFMPEG_CTRL_VOLUME			0x000C		// 音量控制
#define FFMPEG_CTRL_TEMPO			0x000D		// 设置语音速度

// 状态获取控制码
#define FFMPEG_GET_STATUS			0x0000		// 获取播放状态(暂停、快进等)
#define FFMPEG_GET_ENFLAG			0x0001		// 获取播放控制允许标志
#define FFMPEG_GET_TIME				0x0011		// 获取当前播放时间
#define FFMPEG_GET_ALLTIME			0x0012		// 获取总的播放时间
#define FFMPEG_GET_ABPLAY			0x0013		// 获取AB播放状态
#define FFMPEG_GET_ATIME			0x0014		// 获取A点时间
#define FFMPEG_GET_BTIME			0x0015		// 获取B点时间
#define FFMPEG_GET_REPEADAB			0x0016		// 获取重复播放状态
#define FFMPEG_GET_EQ				0x0017		// 获取均衡状态
#define FFMPEG_GET_SPEED			0x0018		// 获取速度状态
#define FFMPEG_GET_VOLUME			0x0019		// 获取音量
#define FFMPEG_GET_OFFSET			0x001a		// 获取数据读取偏移
#define FFMPEG_GET_VOLENHANCE		0x001b		// 获取音量增强值

// 播放状态码
#define FFMPEG_ST_RECORD			0x1000		// 正在播放状态
#define FFMPEG_ST_READY				0x0000		// 播放器处于就绪态
#define FFMPEG_ST_PLAY				0x0001		// 正在播放状态
#define FFMPEG_ST_PAUSE				0x0002		// 暂停状态
#define FFMPEG_ST_STOP				0x0003		// 关闭状态
#define FFMPEG_ST_FORWARD			0x0004		// 快进状态
#define FFMPEG_ST_BACKWARD			0x0005		// 快退状态

#define FFMPEG_LOCK()				//	MediaSrvLock()
#define FFMPEG_UNLOCK()				//  MediaSrvUnlock()

// FfmpegCtl.c
int FfmpegCtlInit(void);
int FfmpegCtlVolumeSet(PFFMPEG_OBJECT obj, int volume);
int FfmpegCtlPause(PFFMPEG_OBJECT obj);
int FfmpegCtlResume(PFFMPEG_OBJECT obj);
int FfmpegCtlSeek(PFFMPEG_OBJECT obj, DWORD milliSecond);
int FfmpegCtlGetDuration(PFFMPEG_OBJECT obj, DWORD *duration);
int FfmpegCtlIsPlaying(PFFMPEG_OBJECT obj);
int FfmpegCtlPlaySpeed(PFFMPEG_OBJECT obj, WORD speed);
int FfmpegCtlForward(PFFMPEG_OBJECT obj);
int FfmpegCtlBackward(PFFMPEG_OBJECT obj);

// FfmpegCallback.c
void FfmpegCbSetCallback(void);
void FfmpegCbInit(void);
int FfmpegCbFileRead(PFFMPEG_VFILE vfile, BYTE *buffer, int count);
int FfmpegCbFileWrite(PFFMPEG_VFILE vfile, void *buffer, int count);
int FfmpegCbFileWriteFlush(PFFMPEG_VFILE vfile);
int FfmpegCbFileSeek(PFFMPEG_VFILE vfile, int offset, int origin);
int FfmpegCbFileTell(PFFMPEG_VFILE vfile);

// FfmpegThread.c
int FfmpegThreadInit(void);
void FfmpegThreadClose(PFFMPEG_OBJECT obj, int code);
void FfmpegThreadOpen(PFFMPEG_OBJECT obj);
int FfmpegThreadRun(PFFMPEG_OBJECT obj);

// FfmpegSrv.c
void FfmpegSrvInit(void);
void FfmpegSrvLock(void);
void FfmpegSrvUnlock(void);
int FfmpegSrvPowerOff(int mode);
int FfmpegSrvMode(void);
int FfmpegSrvInfo(void *media, void *info, int type);
int FfmpegSrvDestroyNotify(PMEDIA_TASK task);
void FfmpegSrvProcMsg(PFFMPEG_OBJECT obj);

void FfmpegShow(DWORD *data, RECT *dst_r, int dst_deg);
void FfmpegShowOpen(void);
void FfmpegShowClose(void);

HANDLE DacOpen(void);
int DacClose(HANDLE hdac);
int DacWrite(HANDLE hdac, short *src, int len);
int DacSetSamplerate(HANDLE hdac, int samprate, int chs);
int DacSetVolume(HANDLE hdac, int volume);
int DacGetStatus(HANDLE hdac, int pause);
int DacSetStatus(HANDLE hdac, int pause);
int DacGetVolume(HANDLE hdac);

HANDLE AdcOpen(DWORD samprate, DWORD chs);
int AdcClose(HANDLE hdac);
int AdcRead(HANDLE hdac, short *src, int len);
int AdcSetVolume(HANDLE hdac, int volume);
int AdcGetStatus(HANDLE hdac, int pause);
int AdcSetStatus(HANDLE hdac, int pause);
int AdcGetVolume(HANDLE hdac);

#endif	// _DIRECT_FFMPEG_H

