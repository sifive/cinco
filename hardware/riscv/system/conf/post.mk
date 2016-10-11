#
# Copyright (c) 2013-2015 Marko Zec, University of Zagreb
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# $Id$
#

LOADADDR = 0x80000000

ARCH_DIR = ${ARCH}
# Includes
MK_INCLUDES += -I${BASE_DIR}include
MK_STDINC = -nostdinc -include sys/param.h

# Libs
LIBDIR = ${BASE_DIR}lib/${ARCH_DIR}

ifndef WITHOUT_LIBS
	ifdef WITHOUT_FLOAT
		MK_LIBS = ${LIBS} -lcint
	else
		MK_LIBS = ${LIBS} -lc
	endif
endif

#MK_CFLAGS += -mno-muldiv

# Optimization options
ifeq ($(findstring -O,$(CFLAGS)),)
#	MK_CFLAGS += -Os -fpeel-loops -mrvc -msoft-float -ffunction-sections -fdata-sections
	MK_CFLAGS += -Os -fpeel-loops -msoft-float 
endif

# Do not try to link with libc and standard startup files
MK_CFLAGS += -ffreestanding

# Do not link; use a pipe to feed the as
MK_CFLAGS += -c -pipe

# Every function goes in a separate section, so that unused ones can be GC-ed
#MK_CFLAGS += -ffunction-sections -fdata-sections

# Default is to warn and abort on all standard errors and warnings
ifndef WARNS
	WARNS = 1
endif

# Warning flags
ifeq ($(findstring ${WARNS}, "01234"),)
	$(error Unsupportde WARNS level ${WARNS})
endif
ifneq ($(findstring ${WARNS}, "1234"),)
	MK_CFLAGS += -Wall
endif
ifneq ($(findstring ${WARNS}, "234"),)
	MK_CFLAGS += -Werror
endif
ifneq ($(findstring ${WARNS}, "34"),)
	MK_CFLAGS += -Wextra -Wsystem-headers -Wshadow
endif
ifneq ($(findstring ${WARNS}, "4"),)
	MK_CFLAGS += -Winline
endif

# Too strict to be practical:
#MK_CFLAGS += -Wpadded

# Include debugging info
#MK_CFLAGS += -g

# Pull in any module-specific compiler flags
MK_CFLAGS += ${CFLAGS}

# Linker flags
#MK_LDFLAGS += --section-start=.init=${LOADADDR}
MK_LDFLAGS += --library-path=${LIBDIR}
ifndef WITHOUT_LIBS
	MK_LDFLAGS += -lcrt0
endif

# Garbage-collect unused section (unreferenced functions)
MK_LDFLAGS += -gc-sections

# Pull in any module-specific linker flags
MK_LDFLAGS += ${LDFLAGS}

# Library construction flags
MK_ARFLAGS = rc

# Disregard metadata sections which objcopy may misinterpret
OBJFLAGS = -R .rel.dyn -R .MIPS.abiflags

CC = riscv32-unknown-elf-gcc ${MK_CFLAGS} ${MK_STDINC} ${MK_INCLUDES}
CXX = riscv32-unknown-elf-g++ ${MK_CFLAGS} ${MK_STDINC} ${MK_INCLUDES} -fno-rtti -fno-exceptions
AS = riscv32-unknown-elf-gcc ${MK_CFLAGS} ${MK_ASFLAGS} ${MK_INCLUDES}
LD = riscv32-unknown-elf-ld ${MK_LDFLAGS}
AR = riscv32-unknown-elf-ar ${MK_ARFLAGS}
OBJCOPY = riscv32-unknown-elf-objcopy


#ifeq ($(shell uname -s), FreeBSD)
#	ISA_CHECK = ${BASE_DIR}tools/isa_check.tcl
#else
#	ISA_CHECK = tclsh ${BASE_DIR}tools/isa_check.tcl
#endif
MKDEP = ${CC} -MM


#
# All object files go to OBJDIR
#

ifndef OBJDIR
	OBJDIR=./obj/${ARCH_DIR}
endif

#
# Autogenerate targets
#

ifeq ($(PROG),)
	ifeq ($(LIB),)
		.DEFAULT_GOAL := abort
	endif
endif

ASM_OBJS = $(addprefix ${OBJDIR}/,$(ASFILES:.S=.O))
CXX_OBJS = $(addprefix ${OBJDIR}/,$(CXXFILES:.cpp=.o))
C_OBJS = $(addprefix ${OBJDIR}/,$(CFILES:.c=.o))
OBJS = ${ASM_OBJS} ${C_OBJS} ${CXX_OBJS}

BIN = ${PROG}.bin
HEX = ${PROG}.hex

${HEX}: ${BIN} Makefile
	${OBJCOPY} ${OBJFLAGS} -O srec ${PROG} ${HEX}

#${ISA_CHECK} ${ARCH} ${PROG}

${BIN}: ${PROG} Makefile
	${OBJCOPY} ${OBJFLAGS} -O binary ${PROG} ${BIN}

${PROG}: ${OBJS} Makefile
	${LD} -o ${PROG} ${OBJS} ${MK_LIBS}

${LIB}: ${OBJS} Makefile
	${AR} ${LIBDIR}/lib${LIB}.a ${OBJS}

depend:
	${MKDEP} ${CFILES} > .depend

clean:
	rm -f ${OBJS} ${PROG} ${BIN} ${HEX}

cleandepend:
	rm -f .depend

abort:
	@ echo Error: unspecified target!

#
# Rule for compiling C files
#
$(addprefix ${OBJDIR}/,%.o) : %.c
	@mkdir -p $(dir $@)
	${CC} -o $@ $<

#
# Rule for compiling C++ files
# XXX fixme extensions: .cc, .cxx, .c++ etc.
#
$(addprefix ${OBJDIR}/,%.o) : %.cpp
	@mkdir -p $(dir $@)
	${CXX} -o $@ $<

#
# Rule for compiling ASM files
#
$(addprefix ${OBJDIR}/,%.O) : %.S
	@mkdir -p $(dir $@)
	${AS} -o $@ $<

-include .depend
