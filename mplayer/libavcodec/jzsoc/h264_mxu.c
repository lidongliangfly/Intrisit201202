/*****************************************************************************
 *
 * JZC MXU(SIMD) ISA Accelerate(H.264 Baseline-profile)
 *  the functions below are MXU-ASM of h264.c
 *
* $Id: h264_mxu.c,v 1.3 2009/03/25 06:32:39 jyu Exp $
 *
 ****************************************************************************/
static void clear_blocks_mxu(DCTELEM *block){
# define MB_BUF_WORD (MB_BUF_SIZE>>SPACE_TYPE_SHORT)
  int i;
  int block_pre4_addr = (int)block - 4;
  for(i=0;i<MB_BUF_WORD;i++)
    S32SDI(xr0,block_pre4_addr,4);
}

static void add_block_error_mxu(uint8_t *recon, DCTELEM *idct, int stride){
  int i;
  idct -= (IDCT_BUF_STRIDE>>SPACE_TYPE_SHORT);
  recon -= stride;
  for(i=0;i<4;i++) {
    S32LDIV(xr1,recon,stride,0);
    S32LDIV(xr2,idct,IDCT_BUF_STRIDE,0);
    S32LDD(xr3,idct,4);
    Q8ACCE_AA(xr3,xr1,xr0,xr2);
    Q16SAT(xr1, xr3, xr2);
    S32STD(xr1,recon,0);
  }
}

static inline void tcsm_gbl_int(H264Context *h){
#if 0
  h->last_mb_addr[0][0] = 
  h->last_mb_addr[1][0] = h->s.current_picture.data[0];
  h->last_mb_addr[0][1] =
  h->last_mb_addr[1][1] = h->s.current_picture.data[1];
  h->last_mb_addr[0][2] =
  h->last_mb_addr[1][2] = h->s.current_picture.data[2];
#endif 

  h->dblk_top_4line[0][0] =
  h->dblk_top_4line[1][0] = TCSM_DBLK_TOP_4LINE_YBUF;
  h->dblk_top_4line[0][1] =
  h->dblk_top_4line[1][1] = TCSM_DBLK_TOP_4LINE_UBUF;
  h->dblk_top_4line[0][2] =
  h->dblk_top_4line[1][2] = TCSM_DBLK_TOP_4LINE_VBUF;

  h->mb_x_latch =
  h->mb_xy_latch =
  h->top_edge_wb[0] =
  h->top_edge_wb[1] =
  h->left_edge_wb[0] =
  h->left_edge_wb[1] =
  h->bottom_edge_wb[0] =
  h->bottom_edge_wb[1] = 0;
}

static inline void tcsm_ptr_init(H264Context *h){
  h->yrecon[0] = TCSM_RECON_YBUF0;
  h->yrecon[1] = TCSM_RECON_YBUF1;
  h->urecon[0] = TCSM_RECON_UBUF0;
  h->urecon[1] = TCSM_RECON_UBUF1;
  h->vrecon[0] = TCSM_RECON_VBUF0;
  h->vrecon[1] = TCSM_RECON_VBUF1;

  h->ydblk[0] = TCSM_DBLK_YBUF0;
  h->ydblk[1] = TCSM_DBLK_YBUF1;
  h->udblk[0] = TCSM_DBLK_UBUF0;
  h->udblk[1] = TCSM_DBLK_UBUF1;
  h->vdblk[0] = TCSM_DBLK_VBUF0;
  h->vdblk[1] = TCSM_DBLK_VBUF1;
  
  h->dblk_des_ptr[0] = TCSM_H264_DBLK_NODE0;
  h->dblk_des_ptr[1] = TCSM_H264_DBLK_NODE1;

  intra_pred4x4_top_ptr[0] = intra_pred4x4_top[0]; 
  intra_pred4x4_left_ptr[0] = intra_pred4x4_left[0]; 
  intra_pred4x4_top_ptr[1] = intra_pred4x4_top[1]; 
  intra_pred4x4_left_ptr[1] = intra_pred4x4_left[1]; 
}

static inline void tcsm_ptr_xchg(H264Context *h){
  uint32_t tmp;
# define XCHG2(a,b,t)   t=a;a=b;b=t
  XCHG2(h->yrecon[0],h->yrecon[1],tmp);
  XCHG2(h->urecon[0],h->urecon[1],tmp);
  XCHG2(h->vrecon[0],h->vrecon[1],tmp);

  XCHG2(h->ydblk[0],h->ydblk[1],tmp);
  XCHG2(h->udblk[0],h->udblk[1],tmp);
  XCHG2(h->vdblk[0],h->vdblk[1],tmp);

  XCHG2(h->dblk_des_ptr[1], h->dblk_des_ptr[0], tmp);

  XCHG2(intra_pred4x4_top_ptr[0], intra_pred4x4_top_ptr[1], tmp);
  XCHG2(intra_pred4x4_left_ptr[0], intra_pred4x4_left_ptr[1], tmp);
}
/*dblked pixel transfer*/
static void dblk_pxl_trans_mxu(H264Context *h) {
  uint32_t i;
  int src0, dst0, src1, dst1, src2, dst2;
  /*
    copy last-MB's deblocked most-right 4 pixel to current buffer's most-left 
   */
  src0 = (int)h->ydblk[0] + MB_LUMA_WIDTH - MB_LUMA_EDGED_WIDTH - 4;
  dst0 = (int)h->yrecon[1] - MB_LUMA_EDGED_WIDTH - 4;
  src1 = (int)h->udblk[0] + MB_CHROM_WIDTH - MB_CHROM_EDGED_WIDTH - 4;
  dst1 = (int)h->urecon[1] - MB_CHROM_EDGED_WIDTH - 4;
  for(i=0;i<16;i++){
    S32LDIV(xr1,src0,MB_LUMA_EDGED_WIDTH,0);
    /*note: Cb and Cr transfered here, since they have the same stride*/
    S32LDIV(xr2,src1,MB_CHROM_EDGED_WIDTH,0);
    S32SDIV(xr1,dst0,MB_LUMA_EDGED_WIDTH,0);
    S32SDIV(xr2,dst1,MB_CHROM_EDGED_WIDTH,0);
  }

    
  // pre mb bottom4 mvto top4, bottom line not need
  int left_added4 = h->left_edge_wb[1]? 4 : 0;
  /*Y*/
  dst0 = (int)h->last_mb_addr[1][0] - h->mb_linesize - left_added4;
  src0 = (int)h->ydblk[0] - MB_LUMA_EDGED_WIDTH - left_added4;
  /*Cb*/
  dst1 = (int)h->last_mb_addr[1][1] - h->mb_uvlinesize - left_added4;
  src1 = (int)h->udblk[0] - MB_CHROM_EDGED_WIDTH - left_added4;
  /*Cr*/
  dst2 = (int)h->last_mb_addr[1][2] - h->mb_uvlinesize - left_added4;
  src2 = (int)h->vdblk[0] - MB_CHROM_EDGED_WIDTH - left_added4;

  for(i=0;i<16;i++) {
      /*Y*/
      S32LDIV(xr1,src0,MB_LUMA_EDGED_WIDTH,0);
      S32LDD(xr2,src0,4);
      S32LDD(xr3,src0,8);
      S32LDD(xr4,src0,12);
      S32SDIV(xr1,dst0,h->mb_linesize,0);
      S32STD(xr2,dst0,4);
      S32STD(xr3,dst0,8);
      S32STD(xr4,dst0,12);
      i_cache(0x19, dst0, 0);
      i_cache(0x19, dst0, 4);
      i_cache(0x19, dst0, 8);
      i_cache(0x19, dst0, 12);
      if(h->left_edge_wb[1]){
         S32LDD(xr5,src0,16);
         S32STD(xr5,dst0,16);
         i_cache(0x19, dst0, 16);
      }
  }

  for(i=0;i<8;i++) {
      /*Cb*/
      S32LDIV(xr1,src1,MB_CHROM_EDGED_WIDTH,0);
      S32LDD(xr2,src1,4);
      S32SDIV(xr1,dst1,h->mb_uvlinesize,0);
      S32STD(xr2,dst1,4);
      i_cache(0x19, dst1, 0);
      i_cache(0x19, dst1, 4);
      if(h->left_edge_wb[1]){
         S32LDD(xr3,src1,8);
         S32STD(xr3,dst1,8);
         i_cache(0x19, dst1, 8);
      }

      /*Cr*/
      S32LDIV(xr1,src2,MB_CHROM_EDGED_WIDTH,0);
      S32LDD(xr2,src2,4);
      S32SDIV(xr1,dst2,h->mb_uvlinesize,0);
      S32STD(xr2,dst2,4);
      i_cache(0x19, dst2, 0);
      i_cache(0x19, dst2, 4);
      if(h->left_edge_wb[1]){
         S32LDD(xr3,src2,8);
         S32STD(xr3,dst2,8);
         i_cache(0x19, dst2, 8);
      }
  }


#ifdef DBG_P_DUMP
    if(h264_3frame_quit==221){
    MpegEncContext * const s = &h->s;
          uint8_t *y_ptr, *u_ptr, *v_ptr;
          uint8_t s_buf[1024];
          y_ptr = (uint8_t *)(h->last_mb_addr[1][0]);
          u_ptr = (uint8_t *)(h->last_mb_addr[1][1]);
          v_ptr = (uint8_t *)(h->last_mb_addr[1][2]);
          if(s->mb_x-2>-1){
            sprintf(s_buf, "======mb_x: %3d, mb_y: %3d=======\n", s->mb_x-2, s->mb_y);
          }else{
            sprintf(s_buf, "======mb_x: %3d, mb_y: %3d=======\n", h->sps.mb_width+s->mb_x-2, s->mb_y-1);            
          }
          fwrite(s_buf, 34, sizeof(uint8_t), dbg_fp_std0);
          for(i=0;i<16;i++){
             sprintf(s_buf,
               "%3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d \n", 
               y_ptr[0], y_ptr[1], y_ptr[2], y_ptr[3], y_ptr[4], y_ptr[5], y_ptr[6],
               y_ptr[7], y_ptr[8], y_ptr[9], y_ptr[10], y_ptr[11], y_ptr[12], y_ptr[13],
               y_ptr[14], y_ptr[15]);
             fwrite(s_buf, 80, sizeof(uint8_t), dbg_fp_std0);
             y_ptr += h->mb_linesize;
          }//for
    }
#endif
}
/*
  decode_mb_pipeout
  ==================
  Since hl_decode_mb is a 3-stage pipeline structure as:
         |  hl_decode_mb0 |  hl_decode_mb1 |  hl_decode_mb2 |
          __________________________________________________ 
   pipe0 |  CAVLC         |  MC/IDCT       |  DeBlock       |
          --------------------------------------------------
                           __________________________________________________ 
   pipe1                  |  CAVLC         |  MC/IDCT       |  DeBlock       |
                           --------------------------------------------------
                                            __________________________________________________ 
   pipe2                                   |  CAVLC         |  MC/IDCT       |  DeBlock       |
                                            --------------------------------------------------
                                                            |<------------
    Suppose hl_decode_mb2 is the final MB loop in slice level, 
  DeBlock@pipe1, MC/IDCT & DeBlock@pipe2 must be supplied outside of the slice level loop.
    So this function(decode_mb_pipeout) is used to do the trailing process when decoder steps outside
  of the slice level loop. 
 */
static void decode_mb_pipeout(H264Context *h) {
  int i;
  DCTELEM *idct = TCSM_IDCT_BUF;
  //<-----------
  idct_polling_end();
  mc_polling_end();
  for (i=0; i<16; i++) 
    if(idct_cbp[i]){
      add_block_error_mxu(h->yrecon[1] + h->block_offset_reco[i],
			  idct + i*4, MB_LUMA_EDGED_WIDTH);
      idct_cbp[i] = 0;
    }
  for (i=16; i<16+4; i++) 
    if(idct_cbp[i]){
      add_block_error_mxu(h->urecon[1] + h->block_offset_reco[i],
			  idct + i*4, MB_CHROM_EDGED_WIDTH);
      idct_cbp[i] = 0;
    }
  for (i=20; i<20+4; i++) 
    if(idct_cbp[i]){
      add_block_error_mxu(h->vrecon[1] + h->block_offset_reco[i],
			  idct + i*4, MB_CHROM_EDGED_WIDTH);
      idct_cbp[i] = 0;
    }

  //<-----------
  dblk_polling_end();
  dblk_pxl_trans_mxu(h);
  /*start-up last MB's deblock*/
  SET_DHA_DBLK((unsigned int)h->dblk_des_ptr[1]);
  SET_DCS_DBLK(0x1);
  tcsm_ptr_xchg(h);
  h->last_mb_addr[1][0] = h->last_mb_addr[0][0];
  h->last_mb_addr[1][1] = h->last_mb_addr[0][1];
  h->last_mb_addr[1][2] = h->last_mb_addr[0][2];
  h->dblk_top_4line[1][0] = h->dblk_top_4line[0][0];
  h->dblk_top_4line[1][1] = h->dblk_top_4line[0][1];
  h->dblk_top_4line[1][2] = h->dblk_top_4line[0][2];
  h->left_edge_wb[1] = h->left_edge_wb[0];
  h->top_edge_wb[1] = h->top_edge_wb[0];
  h->bottom_edge_wb[1] = h->bottom_edge_wb[0];
  dblk_polling_end();

  //<-----------
  dblk_pxl_trans_mxu(h);
}

/*JZC_DBG: only for MC HWcore debug use*/
#ifdef JZC_DBG
static void block_copy_tmp(uint8_t *src, uint8_t *dst, int size, 
			      int src_stride, int dst_stride) {
  int i, j;
  for (i=0; i<size; i++) {
    for(j=0; j<size; j++) 
      dst[j] = src[j];
    dst += dst_stride;
    src += src_stride;
  }
}

static void idct_add_tmp(uint8_t *dst, uint8_t *err, DCTELEM *idct,
			 int stride, int err_stride){
    int i;
    int block_stride = 4;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    for(i=0; i<4; i++){
        dst[i + 0*stride]= cm[ err[i + 0*err_stride] + idct[i + 0*96] ];
        dst[i + 1*stride]= cm[ err[i + 1*err_stride] + idct[i + 1*96] ];
        dst[i + 2*stride]= cm[ err[i + 2*err_stride] + idct[i + 2*96] ];
        dst[i + 3*stride]= cm[ err[i + 3*err_stride] + idct[i + 3*96] ];
    }
}

static void idct_tmp(DCTELEM *idct, DCTELEM *block, int stride){
    int i;
    int block_stride = 4;

    block[0] += 32;

    for(i=0; i<4; i++){
        const int z0=  block[0 + block_stride*i]     +  block[2 + block_stride*i];
        const int z1=  block[0 + block_stride*i]     -  block[2 + block_stride*i];
        const int z2= (block[1 + block_stride*i]>>1) -  block[3 + block_stride*i];
        const int z3=  block[1 + block_stride*i]     + (block[3 + block_stride*i]>>1);

        block[0 + block_stride*i]= z0 + z3;
        block[1 + block_stride*i]= z1 + z2;
        block[2 + block_stride*i]= z1 - z2;
        block[3 + block_stride*i]= z0 - z3;
    }

    for(i=0; i<4; i++){
        const int z0=  block[i + block_stride*0]     +  block[i + block_stride*2];
        const int z1=  block[i + block_stride*0]     -  block[i + block_stride*2];
        const int z2= (block[i + block_stride*1]>>1) -  block[i + block_stride*3];
        const int z3=  block[i + block_stride*1]     + (block[i + block_stride*3]>>1);

        idct[i + 0*96]= (z0 + z3) >> 6;
        idct[i + 1*96]= (z1 + z2) >> 6;
        idct[i + 2*96]= (z1 - z2) >> 6;
        idct[i + 3*96]= (z0 - z3) >> 6;
    }
}

void idct_dc_tmp(DCTELEM *idct, DCTELEM *block,
		     int stride){
    int dc = (block[0] + 32) >> 6;
    int i;
    for(i=0;i<4;i++){
        idct[i + 0*96]=
        idct[i + 1*96]=
        idct[i + 2*96]=
        idct[i + 3*96]= dc;
    }
}

void idct_dc_add_tmp(uint8_t *dst, uint8_t *err, DCTELEM *idct,
		     int stride, int err_stride){
    int i, j;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
    int dc = idct[0];
    for( j = 0; j < 4; j++ )
    {
        for( i = 0; i < 4; i++ )
            dst[i] = cm[ err[i] + dc ];
        dst += stride;
	err += err_stride;
    }
}

static void add_pixels4_tmp(uint8_t *pixels, uint8_t *err, DCTELEM *idct, 
			    int line_size, int err_stride)
{
    int i;
    for(i=0;i<4;i++) {
        pixels[0 + i*line_size] += idct[0 + i*96];
        pixels[1 + i*line_size] += idct[1 + i*96];
        pixels[2 + i*line_size] += idct[2 + i*96];
        pixels[3 + i*line_size] += idct[3 + i*96];
    }
}
static void pixels4_tmp(DCTELEM *idct, DCTELEM *block, int stride)
{
    int i;
    for(i=0;i<4;i++) {
        idct[0] = block[0];
        idct[1] = block[1];
        idct[2] = block[2];
        idct[3] = block[3];
        idct += 96;
        block += 4;
    }
}

#endif //JZC_DBG

