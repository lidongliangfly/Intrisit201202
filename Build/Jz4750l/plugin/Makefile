
TOP = ../../..

#
# ���빤��·��
CC      := /noahos/ToolChain/gcc-mips/bin/noahos-gcc 
CXX     := /noahos/ToolChain/gcc-mips/bin/noahos-g++ 
AR		:= /noahos/ToolChain/gcc-mips/bin/noahos-ar
LD		:= /noahos/ToolChain/gcc-mips/bin/noahos-ld
OBJCOPY	:= /noahos/ToolChain/gcc-mips/bin/noahos-objcopy
NM		:= /noahos/ToolChain/gcc-mips/bin/noahos-nm
OBJDUMP	:= /noahos/ToolChain/gcc-mips/bin/noahos-objdump
RM      := rm -fr 
#
AD_LDOPT = --static -nostdlib -T link_ad.xn
AD_PLUGIN = mpcodecs_ad_libmad \
            mpcodecs_ad_realaac \
            mpcodecs_ad_faad   \
            mpcodecs_ad_msgsm  \
            mpcodecs_ad_hwac3  \
            mpcodecs_ad_liba52 \
            ffmpeg_ad_one      \
            ffmpeg_ad_two      \
            ffmpeg_ad_three    \
            ffmpeg_ad_four     \
            ffmpeg_ad_five     \
            ffmpeg_ad_six
#
VD_LDOPT = --static -nostdlib -T link_vd.xn
VD_PLUGIN = mpcodecs_vd_realvid  \
            mpcodecs_vd_libmpeg2 \
            ffmpeg_vd_h264       \
            mpcodecs_vd_xvid     \
            ffmpeg_vd_svq3       \
            ffmpeg_vd_mpeg4      \
            ffmpeg_vd_mpegvideo  \
            ffmpeg_vd_mpegmisc   \
            ffmpeg_vd_mpegmisc2  \
            mpcodecs_vd_realvid_rv8


#
MLIBS := libmplugin.a libavcodec_p.a libmpcodecs_p.a libavutil.a libmpeg2.a libfaad2.a liba52.a spc.a fipop.a librv89_aac.a libmad.a \
         libavcodec_p.a librv89_jz4740.a libxvidcore.a
LIBS :=  $(addprefix $(TOP)/UsrLib/LibOut/JZ4750L/, $(MLIBS))
LIBS 	+= $(TOP)/Usrlib/libout/libgcc_mipsel.a $(TOP)/Usrlib/libout/mips_klibc.a

MLIBDEMUX := libmpdemux.a
LIBDEMUX :=  $(addprefix $(TOP)/UsrLib/LibOut/JZ4750L/, $(MLIBDEMUX))

MLIBVORBIS := libvorbisidec.a
LIBVORBIS :=  $(addprefix $(TOP)/UsrLib/LibOut/JZ4750L/, $(MLIBVORBIS))

MLIBVC1 := libmplugin.a libvc1dec.a
LIBVC1 :=  $(addprefix $(TOP)/UsrLib/LibOut/JZ4750L/, $(MLIBVC1))

MLIBH264 := libmplugin.a libh264dec.a libavcodec_p.a
LIBH264 :=  $(addprefix $(TOP)/UsrLib/LibOut/JZ4750L/, $(MLIBH264))

MLIBRV8 := librv89_jz4740.a
LIBRV8 :=  $(addprefix $(TOP)/UsrLib/LibOut/JZ4750L/, $(MLIBRV8))

MLIBRV9 := librv89_jz4740.a
LIBRV9 :=  $(addprefix $(TOP)/UsrLib/LibOut/JZ4750L/, $(MLIBRV9))

AUXBIN := $(addprefix $(TOP)/mplayer/libs/codecs/, aux_task.bin h264_aux_task.bin vc1_aux_task.bin xvid_aux_task.bin)

all: mplayer_plugin.bin

mplayer_plugin.bin: newsym.lds $(AD_PLUGIN) $(VD_PLUGIN) $(EXTPLUG) av_codec_table copy_aux 
#
#	echo "aux_task = 0xB3808000"       > address.lds
#	echo "h264_aux_task  = 0xB3808000"  >> address.lds
#	echo "xvid_aux_task  = 0x93808000"  >> address.lds
#	echo "vc1_aux_task   = 0x93808000"  >> address.lds
#	echo "h264_cavlc_tcsm_text = 0xF4000000"   >> address.lds
#	echo "h264_cabac_tcsm_text = 0xF4002000"   >> address.lds
#	echo "tcsm0_bank3 = 0xF4003000"            >> address.lds
#	echo "tcsm0_bank2 = 0xF4002000"            >> address.lds
#	echo "vc1_J1_tcsm_text = 0xF4002000"       >> address.lds
#	
	export AudioADDR=`$(OBJDUMP) -t ../_noahos.elf | grep _audio_codecs_plugin_load_sect | awk '{print	 $$1}'`; \
	for i in ${AD_PLUGIN} av_codec_table; do echo $$i = 0x$$AudioADDR >> address.lds; done
	export VideoADDR=`$(OBJDUMP) -t ../_noahos.elf | grep _video_codecs_plugin_load_sect | awk '{print	 $$1}'`; \
	for i in ${VD_PLUGIN}; do echo $$i = 0x$$VideoADDR >> address.lds; done	
#	
	cd HostLink; ../LinkBin 0 ../$@ `find . -name "*.bin" | sed 's:^./::'` ../address.lds
#
BDEPS := Makefile link_vd.xn link_ad.xn newsym.lds
##

newsym.lds: ../_noahos.elf Makefile	
	mkdir -p HostLink
	export OBJDUMP=$(OBJDUMP); ./genplglds ../_noahos.elf undsym.err > newsym.lds
	$(OBJDUMP) -t ../_noahos.elf | grep _video_codecs_plugin_load_sect | awk '{printf "s/ram .* ORIGIN = .*, /ram  (wx) : ORIGIN = 0x%s, /", $$1}' >repl_videolds.sed
	$(OBJDUMP) -t ../_noahos.elf | grep _audio_codecs_plugin_load_sect | awk '{printf "s/ram .* ORIGIN = .*, /ram  (wx) : ORIGIN = 0x%s, /", $$1}' >repl_audiolds.sed
	sed -f ./repl_audiolds.sed link_ad_orig.xn > link_ad.xn
	sed -f ./repl_videolds.sed link_vd_orig.xn > link_vd.xn
	
#####################################################################################
define BUILD_Aprogbin
$(1): $(LIBS) $(BDEPS)
	$(CC) $(AD_LDOPT) -Xlinker -u -Xlinker $(2) -Xlinker --gc-sections -o $$@.elf $(LIBS) $(LIBRV9) $(3)
	$(OBJCOPY) -O binary $$@.elf $$@
	cp -f $$@ ./HostLink/$$@.bin
endef

define BUILD_Vprogbin
$(1): $(LIBS)  $(BDEPS)
	$(CC) $(VD_LDOPT) -Xlinker -u -Xlinker $(2) -Xlinker --gc-sections -o $$@.elf $(LIBS) $(3)
	$(OBJCOPY) -O binary $$@.elf $$@
#	$(NM) $$@.elf | sort > $$@.sym
	cp -f $$@ ./HostLink/$$@.bin
endef
#####################################################################################

## Audio plugin Build
$(eval $(call BUILD_Aprogbin, mpcodecs_ad_libmad, ad_libmad_start))
$(eval $(call BUILD_Aprogbin, mpcodecs_ad_realaac,   ad_realaac_start))
$(eval $(call BUILD_Aprogbin, mpcodecs_ad_faad,   ad_faad_start))
$(eval $(call BUILD_Aprogbin, mpcodecs_ad_msgsm,  ad_msgsm_start))
$(eval $(call BUILD_Aprogbin, mpcodecs_ad_hwac3,  ad_hwac3_start))
$(eval $(call BUILD_Aprogbin, mpcodecs_ad_liba52, ad_liba52_start))
#
$(eval $(call BUILD_Aprogbin, ffmpeg_ad_one,   _start_one))
$(eval $(call BUILD_Aprogbin, ffmpeg_ad_two,   _start_two))
$(eval $(call BUILD_Aprogbin, ffmpeg_ad_three, _start_three))
$(eval $(call BUILD_Aprogbin, ffmpeg_ad_four,  _start_four))
$(eval $(call BUILD_Aprogbin, ffmpeg_ad_five,  _start_five, -lgcc))
$(eval $(call BUILD_Aprogbin, ffmpeg_ad_six,   _start_six))

## Video plugin Build
$(eval $(call BUILD_Vprogbin, mpcodecs_vd_realvid, _vstart_realvid))
$(eval $(call BUILD_Vprogbin, mpcodecs_vd_libmpeg2, _vstart_libmpeg2))
$(eval $(call BUILD_Vprogbin, mpcodecs_vd_xvid,    _vstart_xvid))
$(eval $(call BUILD_Vprogbin, ffmpeg_vd_mpeg4,     _vstart_mpeg4))
$(eval $(call BUILD_Vprogbin, ffmpeg_vd_mpegvideo, _vstart_mpegvideo))
$(eval $(call BUILD_Vprogbin, ffmpeg_vd_mpegmisc,  _vstart_mpegmisc))
$(eval $(call BUILD_Vprogbin, ffmpeg_vd_mpegmisc2, _vstart_mpegmisc2))
$(eval $(call BUILD_Vprogbin, ffmpeg_vd_svq3,      _vstart_h264))
$(eval $(call BUILD_Vprogbin, ffmpeg_vd_h264,      _vstart_h264))
$(eval $(call BUILD_Vprogbin, mpcodecs_vd_realvid_rv8, _vstart_realvid))

### EXTPLUGIN video build
#mpcodecs_vd_realvid_rv8: $(LIBS) $(LIBRV8) $(BDEPS)
#	$(CC) $(VD_LDOPT) -Xlinker -u -Xlinker _vstart_realvid -Xlinker --gc-sections -o $@.elf $(LIBS) $(LIBRV8)
#	$(OBJCOPY) -O binary $@.elf $@
#	cp -f $@ ./HostLink/$@.bin
#
#mpcodecs_vd_realvid:$(LIBS) $(LIBRV9) $(BDEPS)
#	$(CC) $(VD_LDOPT) -Xlinker -u -Xlinker _vstart_realvid -Xlinker --gc-sections -o $@.elf $(LIBS) $(LIBRV9)
#	$(OBJCOPY) -O binary $@.elf $@
#	cp -f $@ ./HostLink/$@.bin

#ffmpeg_vd_h264:$(LIBS) $(LIBH264)  $(BDEPS)
#	$(CC) $(VD_LDOPT) -Xlinker -u -Xlinker _vstart_h264 -Xlinker --gc-sections -o $@.elf $(LIBH264)
#	$(OBJCOPY) --only-section .h264_cavlc_tcsm_text -O binary $@.elf h264_cavlc_tcsm_text.bin
#	$(OBJCOPY) --only-section .h264_cabac_tcsm_text -O binary $@.elf h264_cabac_tcsm_text.bin
#	$(OBJCOPY) --remove-section .h264_cabac_tcsm_text --remove-section .h264_cavlc_tcsm_text -O binary $@.elf $@
#	cp -f $@ ./HostLink/$@.bin
#	cp -f h264_cavlc_tcsm_text.bin ./HostLink/h264_cavlc_tcsm_text.bin
#	cp -f h264_cabac_tcsm_text.bin ./HostLink/h264_cabac_tcsm_text.bin
	
#ffmpeg_vd_svq3:$(LIBS) $(LIBH264)  $(BDEPS)
#	$(CC) $(VD_LDOPT) -Xlinker -u -Xlinker _vstart_h264 -Xlinker --gc-sections -o $@.elf $(LIBH264)
#	$(OBJCOPY) --only-section .h264_cavlc_tcsm_text -O binary $@.elf h264_cavlc_tcsm_text.bin
#	$(OBJCOPY) --only-section .h264_cabac_tcsm_text -O binary $@.elf h264_cabac_tcsm_text.bin
#	$(OBJCOPY) --remove-section .h264_cabac_tcsm_text --remove-section .h264_cavlc_tcsm_text -O binary $@.elf $@
#	cp -f $@ ./HostLink/$@.bin
#	cp -f h264_cavlc_tcsm_text.bin ./HostLink/h264_cavlc_tcsm_text.bin
#	cp -f h264_cabac_tcsm_text.bin ./HostLink/h264_cabac_tcsm_text.bin	

#ffmpeg_vd_mpeg4:$(LIBS) $(LIBVC1)  $(BDEPS)
#	$(CC) $(VD_LDOPT) -Xlinker -u -Xlinker _vstart_mpeg4 -Xlinker --gc-sections -o $@.elf $(LIBVC1) $(LIBS)
#	$(OBJCOPY) --only-section .vc1_J1_tcsm_text -O binary $@.elf vc1_J1_tcsm_text.bin
#	$(OBJCOPY) --remove-section .vc1_J1_tcsm_text -O binary $@.elf $@
#	cp -f $@ ./HostLink/$@.bin
#	cp -f vc1_J1_tcsm_text.bin ./HostLink/vc1_J1_tcsm_text.bin

### av codec table build
av_codec_table:$(LIBS) demux_ogg.a $(LIBVORBIS)  $(BDEPS)
	$(CC) $(AD_LDOPT) -Xlinker -u -Xlinker _start_avcodec_table -Xlinker --gc-sections -o $@.elf \
	      $(LIBS) demux_ogg.a $(LIBVORBIS) 
	$(OBJCOPY) -O binary $@.elf $@
	cp -f $@ ./HostLink/$@.bin
	
demux_ogg.a: $(LIBDEMUX)
	$(AR) x $(LIBDEMUX) demux_ogg.o
	$(AR) cruv demux_ogg.a demux_ogg.o
##
copy_aux: #$(AUXBIN)
#	cp -fv $^ ./HostLink
#	touch $@

clean:
	rm -fv newsym.lds link_vd.xn link_ad.xn $(AD_PLUGIN) $(VD_PLUGIN) $(EXTPLUG) \
	       *.elf av_codec_table demux_ogg* *.dump *.a *.bin copy_aux HostLink/* *.sed address.lds

