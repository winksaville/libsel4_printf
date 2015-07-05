/*
 * Copyright 2015, Wink Saville
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 */

#ifndef __SEL4_DEBUG_PRINTF_H
#define __SEL4_DEBUG_PRINTF_H

#ifdef NDEBUG

/** NOP printf as NDEBUG is defined */
#define seL4_DebugPrintf(...) ((void)(0))

#else

#include <sel4_printf.h>

/**
 * Print a formated string to a "terminal". This supports a
 * subset of the typical libc printf:
 *   - %% ::= prints a percent
 *   - %d ::= prints a positive or negative long base 10
 *   - %u ::= prints an unsigned long base 10
 *   - %x ::= prints a unsigned long base 16
 *   - %p ::= prints a unsigned long assuming its a pointer base 16 with 0x prepended
 *   - %s ::= prints a string
 *   - %llx ::= prints a unsigned long long base 16
 */
#define seL4_DebugPrintf(...) seL4_Printf(__VA_ARGS__)

#endif

#endif
