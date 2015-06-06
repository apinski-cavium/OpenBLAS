/*****************************************************************************
Copyright (c) 2011-2015, The OpenBLAS Project
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. Neither the name of the OpenBLAS project nor the names of 
      its contributors may be used to endorse or promote products 
      derived from this software without specific prior written 
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********************************************************************************/

#ifndef COMMON_ARM64
#define COMMON_ARM64

#define MB   __asm__ __volatile__ ("dmb  ish" : : : "memory")
#define WMB  __asm__ __volatile__ ("dmb  ishst" : : : "memory")


#define INLINE inline

#define RETURN_BY_COMPLEX

#ifndef ASSEMBLER

#ifdef __LP64__
#define LONG_MOD "x"
#else
#define LONG_MOD "w"
#endif

static void __inline blas_lock(volatile BLASULONG *address){

  int register ret;

  do {
    while (*address) {YIELDING;};

    __asm__ __volatile__(
                         "1:                                                            \n\t"
                         "ldaxr " LONG_MOD "2, [%1]                                     \n\t"
                         "mov   w2, #0                                                  \n\t"
                         "stxr  w3, " LONG_MOD "2, [%1]                                 \n\t"
                         "cmp   w3, #0                                                  \n\t"
                         "b.ne   1b                                                     \n\t"
                         "mov   %w0 , w3                                                \n\t"
                         : "=r"(ret), "=r"(address)
                         : "1"(address)
                         : "memory", "w2" , "w3"


    );

  } while (ret);

}


static inline unsigned long long rpcc(void){
  unsigned long long ret=0;
  double v;
  struct timeval tv;
  gettimeofday(&tv,NULL);
  v=(double) tv.tv_sec + (double) tv.tv_usec * 1e-6;
  ret = (unsigned long long) ( v * 1000.0d );
  return ret;
}

static inline int blas_quickdivide(blasint x, blasint y){
  return x / y;
}

#if defined(DOUBLE)
#define GET_IMAGE(res)  __asm__ __volatile__("str d1, %0" : "=m"(res) : : "memory")
#else
#define GET_IMAGE(res)  __asm__ __volatile__("str s1, %0" : "=m"(res) : : "memory")
#endif

#define GET_IMAGE_CANCEL

#endif


#ifndef F_INTERFACE
#define REALNAME ASMNAME
#else
#define REALNAME ASMFNAME
#endif

#if defined(ASSEMBLER) && !defined(NEEDPARAM)

#define PROLOGUE \
	.global	REALNAME ;\
	.func	REALNAME  ;\
REALNAME:

#define EPILOGUE

#define PROFCODE

#endif


#define SEEK_ADDRESS

#ifndef PAGESIZE
#define PAGESIZE        ( 4 << 10)
#endif
#define HUGE_PAGESIZE   ( 4 << 20)

#define BUFFER_SIZE     (16 << 20)


#define BASE_ADDRESS (START_ADDRESS - BUFFER_SIZE * MAX_CPU_NUMBER)

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#endif

