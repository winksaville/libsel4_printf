/*
 * Copyright 2015, Wink Saville
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 */

#ifndef __SEL4_PRINTF_H
#define __SEL4_PRINTF_H

#include <sel4/simple_types.h>

// Cast to a void * for use with valid seL4_Write_fn param
#define WRITE_PARAM(x) ((void *)(long)(x))

// Forward declaration of the seL4_Writer typedef
typedef struct _seL4_Writer seL4_Writer;

// A function to Write the parameter to the seL4_Writer
typedef void (*seL4_Write_fn)(seL4_Writer* this, void* param);

/**
 * An seL4_Writer which has a function that processes the parameter
 * using any information needed in the seL4_Writer.
 */
typedef struct _seL4_Writer {
    seL4_Write_fn write;    // The function which is passed an seL4_Writer
    void* data;             // Typically a pointer to the writers control data
} seL4_Writer;


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
seL4_Uint32
seL4_WPrintf(seL4_Writer* writer, const char *format, ...);

/**
 * Print a formatted string to seL4_PutChar. This supports a
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
seL4_Uint32
seL4_Printf(const char *format, ...);

#endif // __SEL4_PRINTF_H
