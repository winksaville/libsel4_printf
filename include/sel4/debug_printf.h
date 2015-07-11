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

/** NOP seL4_DebugWPrintf as NDEBUG is defined */
#define seL4_DebugWPrintf(...) ((void)(0))

/** NOP seL4_DebugPrintf as NDEBUG is defined */
#define seL4_DebugPrintf(...) ((void)(0))

#else

#include <sel4/printf.h>

/**
 * Print a formatted string to the writer. This supports a
 * subset of the typical libc printf:
 *   - %% ::= prints a percent
 *   - %d ::= prints a positive or negative long base 10
 *   - %u ::= prints an seL4_Uint32 base 10
 *   - %x ::= prints a seL4_Uint32 base 16
 *   - %p ::= prints a seL4_Uint32 assuming its a pointer base 16 with 0x prepended
 *   - %s ::= prints a string
 *   - %llx ::= prints a seL4_Uint32 long base 16
 *
 * Returns number of characters printed
 */
#define seL4_DebugWPrintf(writer, ...) seL4_Printf(writer, __VA_ARGS__)

/**
 * Print a formatted string to a "terminal". This supports a
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
