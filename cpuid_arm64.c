/**************************************************************************
  Copyright (c) 2013, The OpenBLAS Project
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
  derived from this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE OPENBLAS PROJECT OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *****************************************************************************/

#include <string.h>
#ifdef __linux__
#include <sys/auxv.h>
#include <elf.h>

#define MIDR_REVISION_MASK	0xf
#define MIDR_REVISION(midr)	((midr) & MIDR_REVISION_MASK)
#define MIDR_PARTNUM_SHIFT	4
#define MIDR_PARTNUM_MASK	(0xfff << MIDR_PARTNUM_SHIFT)
#define MIDR_PARTNUM(midr)	\
	(((midr) & MIDR_PARTNUM_MASK) >> MIDR_PARTNUM_SHIFT)
#define MIDR_ARCHITECTURE_SHIFT	16
#define MIDR_ARCHITECTURE_MASK	(0xf << MIDR_ARCHITECTURE_SHIFT)
#define MIDR_ARCHITECTURE(midr)	\
	(((midr) & MIDR_ARCHITECTURE_MASK) >> MIDR_ARCHITECTURE_SHIFT)
#define MIDR_VARIANT_SHIFT	20
#define MIDR_VARIANT_MASK	(0xf << MIDR_VARIANT_SHIFT)
#define MIDR_VARIANT(midr)	\
	(((midr) & MIDR_VARIANT_MASK) >> MIDR_VARIANT_SHIFT)
#define MIDR_IMPLEMENTOR_SHIFT	24
#define MIDR_IMPLEMENTOR_MASK	(0xff << MIDR_IMPLEMENTOR_SHIFT)
#define MIDR_IMPLEMENTOR(midr)	\
	(((midr) & MIDR_IMPLEMENTOR_MASK) >> MIDR_IMPLEMENTOR_SHIFT)

#define IS_THUNDERX(midr) (MIDR_IMPLEMENTOR(midr) == 'C'	\
			   && MIDR_PARTNUM(midr) == 0x0a1)
#endif

#define CPU_UNKNOWN     	0
#define CPU_ARMV8       	1
#define CPU_THUNDERX       	2

static char *cpuname[] = {
  "UNKOWN",
  "ARMV8",
  "THUNDERX"
};


int get_feature(char *search)
{

#ifdef linux
	FILE *infile;
  	char buffer[2048], *p,*t;
  	p = (char *) NULL ;

  	infile = fopen("/proc/cpuinfo", "r");

	while (fgets(buffer, sizeof(buffer), infile))
	{

		if (!strncmp("Features", buffer, 8))
		{
			p = strchr(buffer, ':') + 2;
			break;
      		}
  	}

  	fclose(infile);


	if( p == NULL ) return 0;

	t = strtok(p," ");
	while( t = strtok(NULL," "))
	{
		if (!strcmp(t, search))   { return(1); }
	}

#endif
	return(0);
}


int detect(void)
{

#ifdef linux
#ifndef AT_ARM64_MIDR
#define AT_ARM64_MIDR           38
#endif

	FILE *infile;
  	char buffer[512], *p;
  	p = (char *) NULL ;
	unsigned long midr;

	midr = getauxval(AT_ARM64_MIDR);
	if (IS_THUNDERX(midr))
	{
		return CPU_THUNDERX;
	}

  	infile = fopen("/proc/cpuinfo", "r");

	while (fgets(buffer, sizeof(buffer), infile))
	{

		if ((!strncmp("model name", buffer, 10)) || (!strncmp("Processor", buffer, 9)))
		{
			p = strchr(buffer, ':') + 2;
			break;
      		}
  	}

  	fclose(infile);

  	if(p != NULL)
	{

		if (strstr(p, "AArch64"))
		{
			 	return CPU_ARMV8;

		}


	}
#endif

	return CPU_UNKNOWN;
}

char *get_corename(void)
{
	return cpuname[detect()];
}

void get_architecture(void)
{
	printf("ARM");
}

void get_subarchitecture(void)
{
	int d = detect();
	switch (d)
	{

		case CPU_ARMV8:
			printf("ARMV8");
			break;

		case CPU_THUNDERX:
			printf("THUNDERX");
			break;

		default:
			printf("UNKNOWN");
			break;
	}
}

void get_subdirname(void)
{
	printf("arm64");
}

void get_cpuconfig(void)
{

	int d = detect();
	switch (d)
	{

		case CPU_ARMV8:
    			printf("#define ARMV8\n");
    			printf("#define L1_DATA_SIZE 32768\n");
    			printf("#define L1_DATA_LINESIZE 64\n");
    			printf("#define L2_SIZE 262144\n");
    			printf("#define L2_LINESIZE 64\n");
    			printf("#define DTB_DEFAULT_ENTRIES 64\n");
    			printf("#define DTB_SIZE 4096\n");
    			printf("#define L2_ASSOCIATIVE 4\n");
			break;

		case CPU_THUNDERX:
    			printf("#define ARMV8\n");
    			printf("#define THUNDERX\n");
    			printf("#define L1_DATA_SIZE 32768\n");
    			printf("#define L1_DATA_LINESIZE 128\n");
    			printf("#define L2_SIZE 16777216\n");
    			printf("#define L2_LINESIZE 128\n");
    			printf("#define DTB_DEFAULT_ENTRIES 64\n");
    			printf("#define DTB_SIZE 4096\n");
    			printf("#define L2_ASSOCIATIVE 16\n");
			break;

	}
}


void get_libname(void)
{

	int d = detect();
	switch (d)
	{

		case CPU_ARMV8:
    			printf("armv8\n");
			break;

		case CPU_THUNDERX:
    			printf("thunderx\n");
			break;

	}
}


void get_features(void)
{

#ifdef linux
	FILE *infile;
  	char buffer[2048], *p,*t;
  	p = (char *) NULL ;

  	infile = fopen("/proc/cpuinfo", "r");

	while (fgets(buffer, sizeof(buffer), infile))
	{

		if (!strncmp("Features", buffer, 8))
		{
			p = strchr(buffer, ':') + 2;
			break;
      		}
  	}

  	fclose(infile);


	if( p == NULL ) return;

	t = strtok(p," ");
	while( t = strtok(NULL," "))
	{
	}

#endif
	return;
}


