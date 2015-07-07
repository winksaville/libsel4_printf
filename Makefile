#
# Copyright 2015, Wink Saville
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.

# Targets
TARGETS := libsel4printf.a

# Source files required to build the target
CFILES   := src/sel4_printf.c

ASMFILES :=

# Libraries we need
LIBS := sel4 sel4putchar

# Header files defined here that need to be staged
HDRFILES := $(SOURCE_DIR)/include/*.h

# extra flags
CFLAGS += -Werror -ggdb -g3
ASFLAGS += -Werror -ggdb -g3
ifdef CONFIG_X86_64
CFLAGS += -mno-sse
endif

include $(SEL4_COMMON)/common.mk
