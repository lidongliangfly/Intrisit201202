/*
 * Modified for use with MPlayer, for details see the changelog at
 * http://svn.mplayerhq.hu/mplayer/trunk/
 * $Id: decode_i586.c,v 1.1.1.1 2007-12-14 02:15:35 zpxu Exp $
 */

/*
* mpg123_synth_1to1 works the same way as the c version of this
* file.  only two types of changes have been made:
* - reordered floating point instructions to
*   prevent pipline stalls
* - made WRITE_SAMPLE use integer instead of
*   (slower) floating point
* all kinds of x86 processors should benefit from these
* modifications.
*
* useful sources of information on optimizing x86 code include:
*
*     Intel Architecture Optimization Manual
*     http://www.intel.com/design/pentium/manuals/242816.htm
*
*     Cyrix 6x86 Instruction Set Summary
*     ftp://ftp.cyrix.com/6x86/6x-dbch6.pdf
*
*     AMD-K5 Processor Software Development
*     http://www.amd.com/products/cpg/techdocs/appnotes/20007e.pdf
*
* Stefan Bieschewski <stb@acm.org>
*
* $Id: decode_i586.c,v 1.1.1.1 2007-12-14 02:15:35 zpxu Exp $
*/
#include "config.h"
#include "mangle.h"
#define real float /* ugly - but only way */

static int attribute_used buffs[1088]={0};
static int attribute_used bo=1;
static int attribute_used saved_ebp=0;

int synth_1to1_pent(real *bandPtr, int channel, short *samples)
{
  real tmp[3];
  register int retval;
    __asm __volatile(
"        movl %%ebp,"MANGLE(saved_ebp)"\n\t"
"        movl %1,%%eax\n\t"/*bandPtr*/
"        movl %3,%%esi\n\t"
"        xorl %%edi,%%edi\n\t"
"        movl "MANGLE(bo)",%%ebp\n\t"
"        cmpl %%edi,%2\n\t"
"        jne .L48\n\t"
"        decl %%ebp\n\t"
"        andl $15,%%ebp\n\t"
"        movl %%ebp,"MANGLE(bo)"\n\t"
"        movl $"MANGLE(buffs)",%%ecx\n\t"
"        jmp .L49\n\t"
".L48:\n\t"
"        addl $2,%%esi\n\t"
"        movl $"MANGLE(buffs)"+2176,%%ecx\n\t"
".L49:\n\t"
"        testl $1,%%ebp\n\t"
"        je .L50\n\t"
"        movl %%ecx,%%ebx\n\t"
"        movl %%ebp,%4\n\t"
"        pushl %%eax\n\t"
"        movl 4+%4,%%edx\n\t"
"        leal (%%ebx,%%edx,4),%%eax\n\t"
"        pushl %%eax\n\t"
"        movl 8+%4,%%eax\n\t"
"        incl %%eax\n\t"
"        andl $15,%%eax\n\t"
"        leal 1088(,%%eax,4),%%eax\n\t"
"        addl %%ebx,%%eax\n\t"
"        jmp .L74\n\t"
".L50:\n\t"
"        leal 1088(%%ecx),%%ebx\n\t"
"        leal 1(%%ebp),%%edx\n\t"
"        movl %%edx,%4\n\t"
"        pushl %%eax\n\t"
"        leal 1092(%%ecx,%%ebp,4),%%eax\n\t"
"        pushl %%eax\n\t"
"        leal (%%ecx,%%ebp,4),%%eax\n\t"
".L74:\n\t"
"        pushl %%eax\n\t"
"        call "MANGLE(mp3lib_dct64)"\n\t"
"        addl $12,%%esp\n\t"
"        movl %4,%%edx\n\t"
"        leal 0(,%%edx,4),%%edx\n\t"
"        movl $"MANGLE(mp3lib_decwin)"+64,%%eax\n\t"
"        movl %%eax,%%ecx\n\t"
"        subl %%edx,%%ecx\n\t"
"        movl $16,%%ebp\n\t"
".L55:\n\t"
"        flds (%%ecx)\n\t"
"        fmuls (%%ebx)\n\t"
"        flds 4(%%ecx)\n\t"
"        fmuls 4(%%ebx)\n\t"
"        fxch %%st(1)\n\t"
"        flds 8(%%ecx)\n\t"
"        fmuls 8(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds 12(%%ecx)\n\t"
"        fmuls 12(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        flds 16(%%ecx)\n\t"
"        fmuls 16(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds 20(%%ecx)\n\t"
"        fmuls 20(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        flds 24(%%ecx)\n\t"
"        fmuls 24(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds 28(%%ecx)\n\t"
"        fmuls 28(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        flds 32(%%ecx)\n\t"
"        fmuls 32(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds 36(%%ecx)\n\t"
"        fmuls 36(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        flds 40(%%ecx)\n\t"
"        fmuls 40(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds 44(%%ecx)\n\t"
"        fmuls 44(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        flds 48(%%ecx)\n\t"
"        fmuls 48(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds 52(%%ecx)\n\t"
"        fmuls 52(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        flds 56(%%ecx)\n\t"
"        fmuls 56(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds 60(%%ecx)\n\t"
"        fmuls 60(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        subl $4,%%esp\n\t"
"        faddp %%st,%%st(1)\n\t"
"        fxch %%st(1)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        fistpl (%%esp)\n\t"
"        popl %%eax\n\t"
"        cmpl $32767,%%eax\n\t"
"        jg 1f\n\t"
"        cmpl $-32768,%%eax\n\t"
"        jl 2f\n\t"
"        movw %%ax,(%%esi)\n\t"
"        jmp 4f\n\t"
"1:      movw $32767,(%%esi)\n\t"
"        jmp 3f\n\t"
"2:      movw $-32768,(%%esi)\n\t"
"3:      incl %%edi\n\t"
"4:\n\t"
".L54:\n\t"
"        addl $64,%%ebx\n\t"
"        subl $-128,%%ecx\n\t"
"        addl $4,%%esi\n\t"
"        decl %%ebp\n\t"
"        jnz .L55\n\t"
"        flds (%%ecx)\n\t"
"        fmuls (%%ebx)\n\t"
"        flds 8(%%ecx)\n\t"
"        fmuls 8(%%ebx)\n\t"
"        flds 16(%%ecx)\n\t"
"        fmuls 16(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        flds 24(%%ecx)\n\t"
"        fmuls 24(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        flds 32(%%ecx)\n\t"
"        fmuls 32(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        flds 40(%%ecx)\n\t"
"        fmuls 40(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        flds 48(%%ecx)\n\t"
"        fmuls 48(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        flds 56(%%ecx)\n\t"
"        fmuls 56(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        subl $4,%%esp\n\t"
"        faddp %%st,%%st(1)\n\t"
"        fxch %%st(1)\n\t"
"        faddp %%st,%%st(1)\n\t"
"        fistpl (%%esp)\n\t"
"        popl %%eax\n\t"
"        cmpl $32767,%%eax\n\t"
"        jg 1f\n\t"
"        cmpl $-32768,%%eax\n\t"
"        jl 2f\n\t"
"        movw %%ax,(%%esi)\n\t"
"        jmp 4f\n\t"
"1:      movw $32767,(%%esi)\n\t"
"        jmp 3f\n\t"
"2:      movw $-32768,(%%esi)\n\t"
"3:      incl %%edi\n\t"
"4:\n\t"
".L62:\n\t"
"        addl $-64,%%ebx\n\t"
"        addl $4,%%esi\n\t"
"        movl %4,%%edx\n\t"
"        leal -128(%%ecx,%%edx,8),%%ecx\n\t"
"        movl $15,%%ebp\n\t"
".L68:\n\t"
"        flds -4(%%ecx)\n\t"
"        fchs\n\t"
"        fmuls (%%ebx)\n\t"
"        flds -8(%%ecx)\n\t"
"        fmuls 4(%%ebx)\n\t"
"        fxch %%st(1)\n\t"
"        flds -12(%%ecx)\n\t"
"        fmuls 8(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -16(%%ecx)\n\t"
"        fmuls 12(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -20(%%ecx)\n\t"
"        fmuls 16(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -24(%%ecx)\n\t"
"        fmuls 20(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -28(%%ecx)\n\t"
"        fmuls 24(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -32(%%ecx)\n\t"
"        fmuls 28(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -36(%%ecx)\n\t"
"        fmuls 32(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -40(%%ecx)\n\t"
"        fmuls 36(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -44(%%ecx)\n\t"
"        fmuls 40(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -48(%%ecx)\n\t"
"        fmuls 44(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -52(%%ecx)\n\t"
"        fmuls 48(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -56(%%ecx)\n\t"
"        fmuls 52(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds -60(%%ecx)\n\t"
"        fmuls 56(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        flds (%%ecx)\n\t"
"        fmuls 60(%%ebx)\n\t"
"        fxch %%st(2)\n\t"
"        subl $4,%%esp\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        fxch %%st(1)\n\t"
"        fsubrp %%st,%%st(1)\n\t"
"        fistpl (%%esp)\n\t"
"        popl %%eax\n\t"
"        cmpl $32767,%%eax\n\t"
"        jg 1f\n\t"
"        cmpl $-32768,%%eax\n\t"
"        jl 2f\n\t"
"        movw %%ax,(%%esi)\n\t"
"        jmp 4f\n\t"
"1:      movw $32767,(%%esi)\n\t"
"        jmp 3f\n\t"
"2:      movw $-32768,(%%esi)\n\t"
"3:      incl %%edi\n\t"
"4:\n\t"
".L67:\n\t"
"        addl $-64,%%ebx\n\t"
"        addl $-128,%%ecx\n\t"
"        addl $4,%%esi\n\t"
"        decl %%ebp\n\t"
"        jnz .L68\n\t"
"        movl %%edi,%%eax\n\t"
"        movl "MANGLE(saved_ebp)",%%ebp\n\t"
	:"=a"(retval)
	:"m"(bandPtr),"m"(channel),"m"(samples),"m"(tmp[0])
	:"memory","%edi","%esi","%ebx","%ecx","%edx");
  return retval;
}
