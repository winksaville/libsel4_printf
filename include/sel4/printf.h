/*
 * Copyright 2015, Wink Saville
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 */

#ifndef __SEL4_PRINTF_H
#define __SEL4_PRINTF_H

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
unsigned int seL4_Printf(const char *format, ...);

#endif
