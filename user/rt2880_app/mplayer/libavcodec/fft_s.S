	#include "mips.h"

/*	
int ff_fft_calc_s(FFTContext *s, FFTComplex *z)	
	a0->s
	a1->z
*/	
	.text
	.section ".text.ff_fft_calc_s" , "ax"
	.set	noreorder
	.globl	ff_fft_calc_s
	.ent	ff_fft_calc_s
ff_fft_calc_s:
	subu	sp, sp, 64
	sw		ra, 20(sp)
	sw		s0, 24(sp)
	sw		s1, 28(sp)	;	move	s0, a0	//s0=s
	sw		s2, 32(sp)	;	move	s1, a1	//s1=z
	sw		s3, 36(sp)	;	move	s2, a2
	sw		s4, 40(sp)	;	move	s3, a3	
	sw		s5, 44(sp)	;	move	s4, t4	
	sw		s6, 48(sp)	;	move	s5, t5	
	sw		s7, 52(sp)	;	move	s6, t6	
	sw		s8, 56(sp)
	/////////////////
	lw		t0, 0(s0)	//t0=ln=s->nbits;
	li		t1, 10
	sub		s2, t1, t0	//s2=tabshift
	li		t1, 1
	sll		s3, t1, t0	//s3=np=1<<ln
	
	/* pass 0 */
	move	s5, s1
	sra		s4, s3, 1
1:
	lw		t0, 0(s5)	//p0.re
	lw		t1, 4(s5)	//p0.im
	lw		t2, 8(s5)	//p1.re
	lw		t3, 12(s5)	//p1.im

	addq_s.w		t4, t0, t2//p[0].re+p[1].re
	addq_s.w		t5, t1, t3//p[0].im+p[1].im
	subq_s.w		t6, t0, t2//p[0].re-p[1].re
	subq_s.w		t7, t1, t3//p[0].im-p[1].im
	
	sw		t4, 0(s5)
	sw		t5, 4(s5)
	sw		t6, 8(s5)
	sw		t7, 12(s5)
2:	
	sub		s4, s4, 1
	bnez	s4, 1b
	addi	s5, s5, 16
	
	/* pass 1 */
	move	s5, s1
	sra		s4, s3, 2
	lw		t8, 4(s0)
	beqz	t8, 3f
	nop
	/* s->inverse */
1:
	lw		t0, 0(s5)	//p[0].re
	lw		t1, 4(s5)	//p[0].im
	lw		t2, 8(s5)	//p[1].re
	lw		t3, 12(s5)	//p[1].im
	lw		t4, 16(s5)	//p[2].re
	lw		t5, 20(s5)	//p[2].im
	lw		t6, 24(s5)	//p[3].re
	lw		t7, 28(s5)	//p[3].im
	
	addq_s.w		v0, t0, t4	//p[0].re+p[2].re
	addq_s.w		v1, t1, t5	//p[0].im+p[2].im
	subq_s.w		a2, t0, t4	//p[0].re-p[2].re
	subq_s.w		a3, t1, t5	//p[0].im-p[2].im
	
	subq_s.w		t8, t2, t7	//p[1].re-p[3].im
	addq_s.w		t9, t3, t6	//p[1].im+p[3].re
	addq_s.w		s6, t2, t7	//p[1].re+p[3].im
	subq_s.w		s7, t3, t6	//p[1].im-p[3].re
	
	sw		v0, 0(s5)
	sw		v1, 4(s5)
	sw		a2, 16(s5)
	sw		a3, 20(s5)
	
	sw		t8, 8(s5)
	sw		t9, 12(s5)
	sw		s6, 24(s5)
	sw		s7, 28(s5)
	
2:
	sub		s4, s4, 1
	bnez	s4, 1b
	addi	s5, s5, 32
	j		4f
	nop
3:
	lw		t0, 0(s5)	//p[0].re
	lw		t1, 4(s5)	//p[0].im
	lw		t2, 8(s5)	//p[1].re
	lw		t3, 12(s5)	//p[1].im
	lw		t4, 16(s5)	//p[2].re
	lw		t5, 20(s5)	//p[2].im
	lw		t6, 24(s5)	//p[3].re
	lw		t7, 28(s5)	//p[3].im
	
	addq_s.w		v0, t0, t4	//p[0].re+p[2].re
	addq_s.w		v1, t1, t5	//p[0].im+p[2].im
	subq_s.w		a2, t0, t4	//p[0].re-p[2].re
	subq_s.w		a3, t1, t5	//p[0].im-p[2].im
	
	addq_s.w		t8, t2, t7	//p[1].re+p[3].im
	subq_s.w		t9, t3, t6	//p[1].im-p[3].re
	subq_s.w		s6, t2, t7	//p[1].re-p[3].im
	addq_s.w		s7, t3, t6	//p[1].im+p[3].re
	
	sw		v0, 0(s5)
	sw		v1, 4(s5)
	sw		a2, 16(s5)
	sw		a3, 20(s5)
	
	sw		t8, 8(s5)
	sw		t9, 12(s5)
	sw		s6, 24(s5)
	sw		s7, 28(s5)
	
2:
	sub		s4, s4, 1
	bnez	s4, 3b
	addi	s5, s5, 32
4:	
	/* pass 2 .. ln-1 */
	sra		a2, s3, 3	//a2=nblocks
	sra		s4, s3, 1	//s4=np2=np >> 1;
	li		s3, 4		//s3=nloops
1:
	move	s5, s1		//s5=p
	sll		t0, s3, 3
	add		s6, s1, t0	//s6=q
	li		s7, 0
3:
	lw		t0, 0(s5)	//p.re
	lw		t1, 4(s5)	//p.im
	lw		t2, 0(s6)	//q.re
	lw		t3, 4(s6)	//q.im

	addq_s.w		t4, t0, t2	//p.re+q.re
	addq_s.w		t5, t1, t3	//p.im+q.im
	subq_s.w		t6, t0, t2	//p.re-q.re
	subq_s.w		t7, t1, t3	//p.im-q.im
	
	sw		t4, 0(s5)
	sw		t5, 4(s5)
	sw		t6, 0(s6)
	sw		t7, 4(s6)

	addi	s5, s5, 8
	addi	s6, s6, 8
	
	//la		t9, exptab0
	lw		t9, 12(s0)	//s->exptab
	move	s8, a2		//s8=l=nblocks
5:
	sll		a3, s8, s2
	sll		a3, a3, 3
	add		a3, a3, t9
	lw		t0, 0(s6)	//t0=q->re
	lw		t1, 4(s6)	//t1=q->im
	lw		t2, 0(a3)	//t2=exptab0[(l<<tabshift)].re
	lw		t3, 4(a3)	//t3=exptab0[(l<<tabshift)].im

	mulq_rs.w			t4, t2, t0
	mulq_rs.w			t5, t3, t1
	mulq_rs.w			t6, t2, t1
	mulq_rs.w			t7, t3, t0
	
	subq_s.w		t2, t4, t5
	addq_s.w		t3, t6, t7
	
	lw		t0, 0(s5)	//p.re
	lw		t1, 4(s5)	//p.im

	addq_s.w		t4, t0, t2	//p.re+tmp.re
	addq_s.w		t5, t1, t3	//p.im+tmp.im
	subq_s.w		t6, t0, t2	//p.re-tmp.re
	subq_s.w		t7, t1, t3	//p.im-tmp.im
	
	sw		t4, 0(s5)
	sw		t5, 4(s5)
	sw		t6, 0(s6)
	sw		t7, 4(s6)
	
	addi	s5, s5, 8
	addi	s6, s6, 8
6:
	add		s8, s8, a2
	sub		t0, s8, s4
	bltz	t0, 5b
	nop

	sll		t0, s3, 3
	add		s5, s5, t0		//p += nloops;
	add		s6, s6, t0		//q += nloops;
4:
	addi	s7, s7, 1
	bne		s7, a2, 3b
	nop


2:	
	sra		a2, a2, 1	//nblocks = nblocks >> 1;
	sll		s3, s3, 1
	bnez	a2, 1b
	nop
	
	move	v0, zero
	/////////////////
	lw		ra, 20(sp)
	lw		s0, 24(sp)
	lw		s1, 28(sp)
	lw		s2, 32(sp)
	lw		s3, 36(sp)
	lw		s4, 40(sp)
	lw		s5, 44(sp)
	lw		s6, 48(sp)
	lw		s7, 52(sp)
	lw		s8, 56(sp)	
	jr		ra
	addu	sp, sp, 64
	.set	reorder
	.end	ff_fft_calc_s