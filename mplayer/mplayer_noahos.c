#include "sys/time.h"
#include "mplayer_noahos.h"

#define USE_IPU 0

/* Following is JZ4755 define .*/
#define OSC_CLOCK       24000000
//Register OP
#define INREG8(x)           ( (unsigned char)(*(volatile unsigned char * const)(x)) )
#define OUTREG8(x, y)       *(volatile unsigned char * const)(x) = (y)
#define SETREG8(x, y)       OUTREG8(x, INREG8(x)|(y))
#define CLRREG8(x, y)       OUTREG8(x, INREG8(x)&~(y))

#define INREG16(x)           ( (unsigned short)(*(volatile unsigned short * const)(x)) )
#define OUTREG16(x, y)       *(volatile unsigned short * const)(x) = (y)
#define SETREG16(x, y)      OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)      OUTREG16(x, INREG16(x)&~(y))

#define INREG32(x)           ( (unsigned int)(*(volatile unsigned int* const)(x)) )
#define OUTREG32(x, y)       *(volatile unsigned int * const)(x) = (y)
#define SETREG32(x, y)      OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32(x, INREG32(x)&~(y))

//CPM
#define CPM_BASE_U_VIRTUAL		( 0xB0000000 ) 
#define CPM_CLKGR_OFFSET		( 0x20 )	// RW, 32, 0x00000000, Clock Gate Register
#define CLKGR_STOP_TCU			( 1 << 1 )
#define A_CPM_CLKGR			( CPM_BASE_U_VIRTUAL + CPM_CLKGR_OFFSET )

//TCU, OST
#define TCU_BASE_U_VIRTUAL	( 0xB0002000 )
#define OST_BASE_U_VIRTUAL	( 0xB0002000 )
#define TCU_TESR_OFFSET			( 0x14 )	//  W,  16, 0x??
#define TCU_TECR_OFFSET			( 0x18 )	//  W,  16, 0x??
#define TCU_TMSR_OFFSET			( 0x34 )	//  W, 32, 0x????????
#define	OST_DR_OFFSET			  ( 0xe0 )	// RW, 32, 0x????
#define	OST_CNT_OFFSET			( 0xe8 )	// RW, 32, 0x????
#define	OST_CSR_OFFSET			( 0xeC )	// RW, 16, 0x0000
#define	TCU_OSTCL		        ( 1 << 15 )
#define	TCU_OSTEN		        ( 1 << 15 )
#define	TCU_OSTMST		      ( 1 << 15 )
#define	TCU_CLK_EXTAL				( 0x1 << 2)
#define	TCU_CLK_PRESCALE_CLK4			( 0x1 << 3)
#define A_TCU_TESR				( TCU_BASE_U_VIRTUAL + TCU_TESR_OFFSET )
#define A_TCU_TECR				( TCU_BASE_U_VIRTUAL + TCU_TECR_OFFSET )
#define A_TCU_TMSR				( TCU_BASE_U_VIRTUAL + TCU_TMSR_OFFSET )
#define A_OST_DR				( OST_BASE_U_VIRTUAL + OST_DR_OFFSET )
#define A_OST_CNT				( OST_BASE_U_VIRTUAL + OST_CNT_OFFSET )
#define A_OST_CSR				( OST_BASE_U_VIRTUAL + OST_CSR_OFFSET )

#define MSGSIZE_MAX 2048
Jz47_AV_Decoder *jz47_av_decp;
extern void noah_kprintf(const char *format, ... );
extern void ipu_init_lcd_size(int x,int y,int w, int h);
//=================== NOAH OS and timer =========================
void noah_os_init (struct JZ47_AV_Decoder *priv)
{
	jz47_av_decp = priv;
	noah_kprintf ("++ buf = 0x%08x, size = 0x%08x steam = 0x%08x, plugstream = 0x%08x+++\n", 
	         jz47_av_decp->malloc_buf, jz47_av_decp->malloc_size,  jz47_av_decp->stream,  jz47_av_decp->plugstream);
	mplayer_memory_set(jz47_av_decp->malloc_buf, jz47_av_decp->malloc_size);
	ipu_init_lcd_size(jz47_av_decp->UsrLcdPosX,jz47_av_decp->UsrLcdPosY,jz47_av_decp->UsrLcdWidth,jz47_av_decp->UsrLcdHeight);
}

#define read_c0_status()                  \
__extension__ ({								          \
  int  __d;										            \
  __asm__ __volatile ("mfc0 %0, $12, 0"		\
	   :"=d"(__d));                         \
  __d; 											              \
})

#define write_c0_status(d)            \
  __asm__ __volatile ("mtc0 %0, $12, 0"::"d"(d));
//----------------------------------------------------------------------------

void cli(void)
{
	register unsigned int t;

	t = read_c0_status();
	t &= ~1;
	write_c0_status(t);
}

//----------------------------------------------------------------------------

void sti(void)
{
	register unsigned int t;

	t = read_c0_status();
	t |= 1;
	t &= ~2;
	write_c0_status(t);
}

#define CP0_wGet_PRId()								    \
__extension__ ({								          \
  int  __d;										            \
  __asm__ __volatile ("mfc0 %0, $15, 0"		\
	   :"=d"(__d));                         \
  __d; 											              \
})

unsigned int os_GetProcessorID(void)
{
	unsigned int procid = 4755;
	unsigned int id = CP0_wGet_PRId();

	unsigned int pre_check_id = (unsigned int)(*((volatile unsigned int *)0xB00F0000));

	if (pre_check_id & 0x80000000)
		return (4725);

	switch (id)
	{
		case 0x0AD0024F:
			procid = 4740;
			break;

		case 0x1ED0024F:
			procid = 4750;
			break;

		case 0x2ED0024F:
			procid = 4755;
			break;
	}

	return (procid);
}

#define CACHE_LINE_SIZE		32
#define Hit_Invalidate_D	0x11

#define SYNC_WB() __asm__ __volatile__ ("sync")
#define cache_op(op,addr)						\
	__asm__ __volatile__(						\
	"	.set	noreorder		\n"			\
	"	.set	mips32\n\t		\n"			\
	"	cache	%0, %1			\n"			\
	"	.set	mips0			\n"			\
	"	.set	reorder"					\
	:										\
	: "i" (op), "m" (*(unsigned char *)(addr)))

void __dcache_hitinv(unsigned int addr,unsigned int size)
{
	unsigned long dc_lsize = CACHE_LINE_SIZE;
	unsigned long end, a;

	a = addr & ~(dc_lsize - 1);
	end = (addr + size - 1) & ~(dc_lsize - 1);
	while (1) {
		cache_op(Hit_Invalidate_D, a);
		if (a == end)
			break;
		a += dc_lsize;
	}
	SYNC_WB();
}       

int Init_PerformanceCounter(void)
{
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_TCU);
	OUTREG16(A_TCU_TECR, TCU_OSTCL);

	OUTREG16(A_OST_CSR, TCU_CLK_EXTAL | TCU_CLK_PRESCALE_CLK4);
	OUTREG32(A_OST_DR, 0xFFFFFFFF);
	OUTREG32(A_OST_CNT, 0);

	SETREG32(A_TCU_TESR, TCU_OSTEN);

	return ((OSC_CLOCK / 4 ) / 1000000);
}

unsigned int Get_PerformanceCounter(void)
{
     return (INREG32(A_OST_CNT));
}

void Stop_PerformanceCounter(void)
{
	noah_kprintf("WARMING: Performance Counter will STOP!!!\n");

	OUTREG32(A_TCU_TMSR, TCU_OSTMST);
	OUTREG16(A_TCU_TECR, TCU_OSTCL);
}

void JZ_StopTimer(void)
{
	Stop_PerformanceCounter();
}

#ifdef NOAH_OS
static unsigned int prev_time = 0, cur_time = 0;
static unsigned long long time_of_day = 0;
int gettimeofday(struct timeval now_tv, struct timezone zone)
{
	unsigned cur_time = Get_PerformanceCounter() / (OSC_CLOCK / 4 / 1000000);
	if (cur_time < prev_time)
	{
		time_of_day = time_of_day + 0x100000000LL;
		time_of_day = time_of_day + (unsigned long long) cur_time - (unsigned long long)prev_time;
	}
	else
		time_of_day = time_of_day + (unsigned long long) cur_time - (unsigned long long)prev_time;
	
	prev_time = cur_time;
	
	 now_tv.tv_sec  = (int)(time_of_day / 1000000);
   now_tv.tv_usec = (int)(time_of_day % 1000000);
   return 0;
}
#endif

void BUFF_TimeDly (unsigned int usec)
{
	//kprintf("mplayer time delay = %d\n",usec / 1000);
	jz47_av_decp->os_msleep (usec / 1000);
}

// ========== IPU and display ===========

#if USE_IPU
void ipu_image_start () 
{
	return;
}

#endif

void mplayer_showmode ()
{
	return;
}

void jz47_auto_select ()
{
	return;
}

void Flush_OSDScrean ()
{
	return;
}

// ================ LCD ===================
/*
*/
int lcd_get_width ()
{
  return jz47_av_decp->lcd_width;
}

int lcd_get_height ()
{
  return jz47_av_decp->lcd_height;
}

int lcd_get_line_length ()
{
  return jz47_av_decp->lcd_line_length;
}

void* lcd_get_frame ()
{
  return (void *)jz47_av_decp->lcd_frame_buffer;
}

int get_display_is_direct ()
{
	return 0;
}

// ======== decoder  =================/
#if USE_IPU
void drop_image ()
{
	return;
}
#endif
void drop_image_seekmode ()
{
	return;
}

void set_release_cycle ()
{
	return;
}

int fill_audio_out_buffers_foraudio ()
{
	return 1;
}

int GetAudioMemMode ()
{
	return 0;
}

// ================ NOAH OS I/O stream =================

#define SEEK_SET               0
#define SEEK_CUR               1
#define SEEK_END               2

void* jzfs_Open ()
{
	return jz47_av_decp->stream;
}

void jzfs_Close ()
{
	return;
}

long jzfs_Tell (void *stream)
{
	return jz47_av_decp->os_ftell (stream);
}

long jzfs_Read (void *ptr, long size, long nmemb, void *stream)
{
	long len;
	len = jz47_av_decp->os_fread (ptr, size, nmemb, stream);
//	kprintf("jzfs_Read: len = %d, size = %d\n",len,size * nmemb);
	return len;
}

long jzfs_Write (void *ptr, long size, long nmemb, void *stream)
{
	return jz47_av_decp->os_fwrite (ptr, size, nmemb, stream);
}

long jzfs_Seek (void *stream, long offset, int whence)
{
	return jz47_av_decp->os_fseek (stream, offset, whence);
}

unsigned int FS_FReadEx(void *ptr, long size, long nmemb, void *stream)
{
	unsigned long len;
	len = jz47_av_decp->os_fread (ptr, size, nmemb, stream);
//	kprintf("jzfs_Read: len = %d, size = %d\n",len,size * nmemb);
	return len;
}

/* following is BUFF functions.  */
void GM_Dealloc ()
{
	return;
}

void Module_alignAlloc ()
{
	return;
}

void BUFF_Init ()
{
	return;
}

void BUFF_SetBufSize(int d,int perbuffsize,int buffmode)
{
	return;
}

int BUFF_GetMode()
{
	return 0;
}

void* BUFF_Open (char *p)
{
	return jz47_av_decp->stream;
}

void BUFF_Close (void *stream)
{
	return;
}

long BUFF_Seek (void *stream, long offset, int whence)
{
	return jz47_av_decp->os_fseek (stream, offset, whence);
}

long BUFF_DirectRead (void *stream, void *ptr, long size)
{
	long len;
	len = jz47_av_decp->os_fread (ptr, 1, size, stream);
//	kprintf("BUFF_DirectRead: len = %d,size = %d\n",len,size);
	
	return len;
}

long BUFF_GetFileSize (void *stream)
{
	long len, cur;
	cur = jz47_av_decp->os_ftell (stream);
	jz47_av_decp->os_fseek (stream, 0, SEEK_END);
	len = jz47_av_decp->os_ftell (stream);
	jz47_av_decp->os_fseek (stream, cur, SEEK_SET);
	
	return len;
}

void BUFF_ConsumeBuff ()
{
	return;
}

long BUFF_PauseBuff ()
{
	return jz47_av_decp->os_ftell (jz47_av_decp->stream);
}

// ================ NOAH OS audio driver functions =================

//#define SAVE_AUDIO_DATA

#ifdef SAVE_AUDIO_DATA
#include "config.h"
#include "libaf/af_format.h"
#include "libavutil/bswap.h"

#define WAV_ID_RIFF 0x46464952 /* "RIFF" */
#define WAV_ID_WAVE 0x45564157 /* "WAVE" */
#define WAV_ID_FMT  0x20746d66 /* "fmt " */
#define WAV_ID_DATA 0x61746164 /* "data" */
#define WAV_ID_PCM  0x0001

struct WaveHeader
{
	uint32_t riff;
	uint32_t file_length;
	uint32_t wave;
	uint32_t fmt;
	uint32_t fmt_length;
	uint16_t fmt_tag;
	uint16_t channels;
	uint32_t sample_rate;
	uint32_t bytes_per_second;
	uint16_t block_align;
	uint16_t bits;
	uint32_t data;
	uint32_t data_length;
};

/* init with default values */
static struct WaveHeader wavhdr;
static void *dump_fp;
static int write_wave_header(int rate,int channels,int format,int flags) {
	int bits=8;
	
	switch(format){
	case AF_FORMAT_S8:
	    format=AF_FORMAT_U8;
	case AF_FORMAT_U8:
	    break;
	default:
	    format=AF_FORMAT_S16_LE;
	    bits=16;
	    break;
	}

	wavhdr.riff = le2me_32(WAV_ID_RIFF);
	wavhdr.wave = le2me_32(WAV_ID_WAVE);
	wavhdr.fmt = le2me_32(WAV_ID_FMT);
	wavhdr.fmt_length = le2me_32(16);
	wavhdr.fmt_tag = le2me_16(WAV_ID_PCM);
	wavhdr.channels = le2me_16(channels);
	wavhdr.sample_rate = le2me_32(rate);
	wavhdr.bytes_per_second = le2me_32(channels*rate*(bits/8));
	wavhdr.bits = le2me_16(bits);
	wavhdr.block_align = le2me_16(channels * (bits / 8));
	
	wavhdr.data = le2me_32(WAV_ID_DATA);
	wavhdr.data_length=le2me_32(0x7ffff000);
	wavhdr.file_length = wavhdr.data_length + sizeof(wavhdr) - 8;

	dump_fp = kfopen("d:\\mpaudiodump.wav", "w+b");
	if(dump_fp) {
			kfwrite(&wavhdr, 1, sizeof(wavhdr), dump_fp);
			wavhdr.file_length=wavhdr.data_length=0;
		return 1;
	}
	else
	  kprintf ("==== Open audio dump file error =====\n");
	return 0;
}

static void write_wave_end () {
	if (! dump_fp)
		return;
		
  kfseek(dump_fp, 0, SEEK_SET);/* Write wave header */
	
	wavhdr.file_length = wavhdr.data_length + sizeof(wavhdr) - 8;
	wavhdr.file_length = le2me_32(wavhdr.file_length);
	wavhdr.data_length = le2me_32(wavhdr.data_length);
	kfwrite(&wavhdr, 1, sizeof(wavhdr), dump_fp);
	
	kfclose(dump_fp);
	kprintf ("++++++++ close the audio dump file ++++++++++\n");
}

static void write_wave_data (unsigned char *data, int len) {
	if (! dump_fp)
		return;
		
	kfwrite(data, 1, len, dump_fp);
  wavhdr.data_length += len;
}
#endif

int noahos_audio_init(int rate,int channels,int format,int flags)
{
#ifdef SAVE_AUDIO_DATA
  write_wave_header(rate, channels, format, flags);
#endif	
	
  return jz47_av_decp->os_audio_init (rate, channels, format, flags);
}

void noahos_audio_uninit(int immed)
{
#ifdef SAVE_AUDIO_DATA
  write_wave_end();
#endif		
  jz47_av_decp->os_audio_uninit (immed);
}

void noahos_audio_reset(void)
{
	jz47_av_decp->os_audio_reset ();
}

void noahos_audio_pause(void)
{
	jz47_av_decp->os_audio_pause ();
}

void noahos_audio_resume(void)
{
	jz47_av_decp->os_audio_resume ();
}

int noahos_audio_get_space(void)
{
	return jz47_av_decp->os_audio_get_space ();
}

int noahos_audio_play(void* data,int len,int flags)
{
#ifdef SAVE_AUDIO_DATA
  write_wave_data(data, len);
#endif

  len = jz47_av_decp->os_audio_play (data, len, flags);

  return len;
}

float noahos_audio_get_delay(void)
{
	return jz47_av_decp->os_audio_get_delay ();
}

int noahos_audio_control(int cmd,void *arg)
{
	return jz47_av_decp->os_audio_control (cmd, arg);
}

// ================ NOAH OS mplayer plugin load segment =================
#if defined(JZ4755_BSP)
__attribute__ ((__section__ (".video_codec_sect"))) unsigned int _video_codecs_plugin_load_sect[(600 + 1)*1024/4] ;
__attribute__ ((__section__ (".audio_codec_sect"))) unsigned int _audio_codecs_plugin_load_sect[(360 + 1)*1024/4];
#else
__attribute__ ((__section__ (".video_codec_sect"))) unsigned int _video_codecs_plugin_load_sect[(640 + 1)*1024/4];
__attribute__ ((__section__ (".audio_codec_sect"))) unsigned int _audio_codecs_plugin_load_sect[(360 + 1)*1024/4];
#endif

#define MAX_PROGRAM_NAME 36
typedef struct __tag_LOADPROGRAM
{
	char progname[MAX_PROGRAM_NAME];
	unsigned int nand_offset;
	unsigned int addr;
	unsigned int binsize;
}LoadProgram,*PLoadProgram;

#define MAX_PROG_COUNT 64
LoadProgram Prog_buf[MAX_PROG_COUNT];

static int mystrcmp(char *s1,char* s2)
{
	while(*s1 && *s2 && (*s1++ == *s2++));
	if(*s1 | *s2)
		return 1;
	return 0;	
	
}

void* Load_Program(int audio, char *pname, void *varg1, void *varg2)
{
	int i;
	PLoadProgram g_Prog = Prog_buf;
	void* (*startprog)(void *,void *);

	
	if (audio)
		startprog = (void *)_audio_codecs_plugin_load_sect;
	else
		startprog = (void *)_video_codecs_plugin_load_sect;
	  
	// skip the file header
	jz47_av_decp->os_fseek (jz47_av_decp->plugstream, 0, SEEK_SET);
	jz47_av_decp->os_fread (g_Prog, sizeof(LoadProgram) , MAX_PROG_COUNT, jz47_av_decp->plugstream);
	
	if((pname == 0) || (pname[0] == 0))
			return;
	
	for(i = 0;i < MAX_PROG_COUNT;i++)
	{
		if(g_Prog->progname[0] == 0)
			break;
		if(mystrcmp(pname,g_Prog->progname) == 0)
		{
			jz47_av_decp->os_fseek (jz47_av_decp->plugstream, g_Prog->nand_offset*2048, SEEK_SET);
			if (audio)
				jz47_av_decp->os_fread (_audio_codecs_plugin_load_sect, 1, g_Prog->binsize, jz47_av_decp->plugstream);
			else
				jz47_av_decp->os_fread (_video_codecs_plugin_load_sect, 1, g_Prog->binsize, jz47_av_decp->plugstream);
			__dcache_writeback_all();
			__icache_invalidate_all();
//			jz47_av_decp->os_audio_play(_audio_codecs_plugin_load_sect,g_Prog->binsize,0);
			return startprog(varg1,varg2);
		}
		g_Prog++;
	}
	return NULL;
}

void* Load_ProgramBin(char *pname,void *varg1,void *varg2)
{
	int i;
	PLoadProgram g_Prog = Prog_buf;
	void* (*startprog)(void *,void *);
	  
	// skip the file header
	jz47_av_decp->os_fseek (jz47_av_decp->plugstream, 0, SEEK_SET);
	jz47_av_decp->os_fread (g_Prog, sizeof(LoadProgram) , MAX_PROG_COUNT, 
	                        jz47_av_decp->plugstream);
	
	if((pname == 0) || (pname[0] == 0))
			return;
	
	for(i = 0;i < MAX_PROG_COUNT;i++)
	{
		if(g_Prog->progname[0] == 0)
			break;
		if(mystrcmp(pname,g_Prog->progname) == 0)
		{
			jz47_av_decp->os_fseek (jz47_av_decp->plugstream, g_Prog->nand_offset*2048, SEEK_SET);
			jz47_av_decp->os_fread (g_Prog->addr, 1, g_Prog->binsize, jz47_av_decp->plugstream);

			noah_kprintf("Loader = %s %s\n",pname,g_Prog->progname);		
			__dcache_writeback_all();
			__icache_invalidate_all();
			 return (void *)g_Prog->addr;
		}
		g_Prog++;
	}
	return NULL;
}

void* Load_ProgramBin_tobuf(char *pname,void *varg1,void *varg2)
{
	int i;
	PLoadProgram g_Prog = Prog_buf;
	void* (*startprog)(void *,void *);
	  
	// skip the file header
	jz47_av_decp->os_fseek (jz47_av_decp->plugstream, 0, SEEK_SET);
	jz47_av_decp->os_fread (g_Prog, sizeof(LoadProgram) , MAX_PROG_COUNT, 
	                        jz47_av_decp->plugstream);
	if((pname == 0) || (pname[0] == 0))
			return;
	
	for(i = 0;i < MAX_PROG_COUNT;i++)
	{
		if(g_Prog->progname[0] == 0)
			break;
		if(mystrcmp(pname,g_Prog->progname) == 0)
		{
			jz47_av_decp->os_fseek (jz47_av_decp->plugstream, g_Prog->nand_offset*2048, SEEK_SET);
			jz47_av_decp->os_fread (g_Prog->addr, 1, g_Prog->binsize, jz47_av_decp->plugstream);
			if (varg1)
				g_Prog->addr = (unsigned int)varg1;

			noah_kprintf("Loader = %s %s\n",pname,g_Prog->progname);		
			__dcache_writeback_all();
			__icache_invalidate_all();
			 return (void *)g_Prog->binsize;
		}
		g_Prog++;
	}
	return NULL;
}	                        	

int lrintf (float f)
{
	#if 1
	noah_kprintf ("++++ calling lrintf ++++\n");
	while (1);
#else	
	*(unsigned int *)5 = 1;
#endif
	//return (int)(f + 0.5);
}

#undef printf
void printf ()
{
#if 1
	noah_kprintf ("++++ calling printf ++++\n");
	//while (1);
#else	
	*(unsigned int *)1 = 1;
#endif	
}

void noah_kprintf(const char *format, ... )
{
	va_list va;
	char tmp[MSGSIZE_MAX];
	
	if( !jz47_av_decp->kprintf_enable )
		return;
	
	va_start(va, format);
	vsprintf(tmp, format, va);
	va_end(va);
	kprintf("%s", tmp);
}

void noah_open_osd(int w, int h)
{
	jz47_av_decp->os_open_video_osd(w,h);
}

int noah_get_ipu_status()
{
	return jz47_av_decp->fIpuEnable;
}

char* noah_get_file_name()
{
	return jz47_av_decp->FileName;
}

int noah_get_out_format()
{
	return jz47_av_decp->OutFormat;
}

int noah_long_jump(int err)
{
	if( err )
		return 0;

	if( jz47_av_decp->fMplayerInit )
		jz47_av_decp->os_audio_long_jump((void*)jz47_av_decp->stream,jz47_av_decp->AudioJumpBuf,1);

	return 1;
}
