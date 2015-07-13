/*
 * copyright 2015, Wink Saville
 *
 * this software may be distributed and modified according to the terms of
 * the bsd 2-clause license. note that no warranty is provided.
 * see "license_bsd2.txt" for details.
 */

#include <sel4/simple_types.h>
#include <sel4/string.h>
#include <sel4/printf.h>
#include <sel4/putchar.h>
#include <sel4/vargs.h>
#include <sel4/debug_assert.h>

#define NO_LEADING_0 seL4_False
#define RADIX16_LEADING_0 seL4_True

/**
 * Write a character using seL4_PutChar
 */
static void writeChar(seL4_Writer* this, void* param) {
    (void)(this);
    seL4_PutChar((char)(((int)param) & 0xff));
}

/**
 * Output a string
 */
static seL4_Uint32
writeStr(seL4_Writer *writer, char *str) {
    seL4_Uint32 count = 0;
    char c;

    while((c = *str++)) {
        writer->writeParam(writer, WRITE_PARAM(c));
        count += 1;
    }

    return count;
}

/**
 * Output an unsigned 32 bit value
 */
static seL4_Uint32
writeUint32(seL4_Writer* writer, seL4_Uint32 val, seL4_Bool radix16Leading0, seL4_Uint32 radix) {
    static const char valToChar[] = "0123456789abcdef";
    seL4_Uint32 count = 0;
    char result[32];

    // Must support radix 10
    seL4_DebugAssert(sizeof(valToChar) >= 10);

    // Check that we can support binary output
    seL4_DebugAssert(sizeof(result) >= 32);

    // Validate radix
    if ((radix <= 1) || (radix > sizeof(valToChar))) {
        count = writeStr(writer, "Bad Radix ");
        count += writeUint32(writer, radix, NO_LEADING_0, 10);
    } else {
        seL4_Int32 idx;
        for (idx = 0; idx < sizeof(result); idx++) {
            result[idx] = valToChar[val % radix];
            val /= radix;
            if (val == 0) {
                break;
            }
        }
        count = idx + 1;
        if ((radix == 16) && radix16Leading0) {
            seL4_Int32 pad0Count = (sizeof(val) * 2) - count;
            count += pad0Count;
            while (pad0Count-- > 0) {
                writer->writeParam(writer, WRITE_PARAM('0'));
            }
        }
        for (; idx >= 0; idx--) {
            writer->writeParam(writer, WRITE_PARAM(result[idx]));
        }
    }
    return count;
}

/**
 * Output an unsigned 64 bit value
 */
static seL4_Uint32
writeUint64_Radix16(seL4_Writer* writer, seL4_Uint64 val, seL4_Bool radix16Leading0) {
    seL4_Uint32 count = 0;
    seL4_Uint32 upper = (val >> 32) & 0xFFFFFFFF;
    seL4_Uint32 lower = val & 0xFFFFFFFF;
    if ((upper != 0) || radix16Leading0) {
        count = writeUint32(writer, upper, radix16Leading0, 16);
        count += writeUint32(writer, lower, RADIX16_LEADING_0, 16);
    } else {
        count += writeUint32(writer, lower, radix16Leading0, 16);
    }
    return count;
}


/**
 * Output a signed 32 bit value
 */
static seL4_Uint32
writeInt32(seL4_Writer* writer, seL4_Int32 val, seL4_Uint32 radix) {
    seL4_Uint32 count = 0;
    if ((val < 0) && (radix == 10)) {
        writer->writeParam(writer, WRITE_PARAM('-'));
        count += 1;
        val = -val;
    }
    return count + writeUint32(writer, val, NO_LEADING_0, radix);
}

/**
 * Print a formatted string to the writer function. This supports a
 * subset of the typical libc printf:
 *   - %% ::= prints a percent
 *   - %d ::= prints a positive or negative long base 10
 *   - %u ::= prints an seL4_Uint32 base 10
 *   - %x ::= prints a seL4_Uint32 base 16
 *   - %p ::= prints a seL4_Uint32 assuming its a pointer base 16 with 0x prepended
 *   - %s ::= prints a string
 *   - %llx ::= prints a seL4_Uint64 base 16
 *
 * Returns number of characters consumed
 */
seL4_Uint32
seL4_Formatter(seL4_Writer* writer, const char* format, seL4_VaList args) {
    seL4_Uint32 count = 0;

    // Check inputs
    if (format == seL4_Null || args == seL4_Null || writer == seL4_Null) {
        goto done;
    }

    if (writer->writeBeg != seL4_Null) {
        writer->writeBeg(writer);
    }
    char c;
    while ((c = *format++) != 0) {
        if (c != '%') {
            // Not the format escape character
            writer->writeParam(writer, WRITE_PARAM(c));
            count += 1;
        } else {
            // Is a '%' so get the next character to decide the format
            char nextC = *format++;
            if (nextC == 0) {
                count += 1;
                goto done;
            }
            switch (nextC) {
                case '%': {
                    // was %% just echo a '%'
                    writer->writeParam(writer, WRITE_PARAM(nextC));
                    count += 1;
                    break;
                }
                case 's': {
                    // Handle string specifier
                    char *s = seL4_VaArg(args, char *);
                    count += writeStr(writer, s);
                    break;
                }
                case 'b': {
                    count += writeUint32(writer, seL4_VaArg(args, seL4_Int32), NO_LEADING_0, 2);
                    break;
                }
                case 'd': {
                    count += writeInt32(writer, seL4_VaArg(args, seL4_Int32), 10);
                    break;
                }
                case 'u': {
                    count += writeUint32(writer, seL4_VaArg(args, seL4_Uint32), NO_LEADING_0, 10);
                    break;
                }
                case 'x': {
                    count += writeUint32(writer, seL4_VaArg(args, seL4_Uint32), NO_LEADING_0, 16);
                    break;
                }
                case 'l': {
                    if (seL4_StrNCmp("lx", format, 2) == 0) {
                        format += 2;
                        count += writeUint64_Radix16(writer, seL4_VaArg(args, seL4_Uint64), NO_LEADING_0);
                    } else {
                        count += writeStr(writer, "%l");
                    }
                    break;
                }
                case 'p': {
                    seL4_Uint32 sizePtr = sizeof(void *);
                    switch (sizePtr) {
                        case sizeof(seL4_Uint32): {
                            count += writeUint32(writer, seL4_VaArg(args, seL4_Uint32), RADIX16_LEADING_0, 16);
                            break;
                        }
                        case sizeof(seL4_Uint64): {
                            count += writeUint64_Radix16(writer, seL4_VaArg(args, seL4_Uint64), RADIX16_LEADING_0);

                            break;
                        }
                        default: {
                            writeStr(writer, "Bad ptr size:");
                            writeUint32(writer, sizePtr, 10, NO_LEADING_0);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    writer->writeParam(writer, WRITE_PARAM(c));
                    writer->writeParam(writer, WRITE_PARAM(nextC));
                    count += 1;
                    break;
                }
            }
        }
    }
    if (writer->writeEnd != seL4_Null) {
        writer->writeEnd(writer);
    }

done:
    return count;
}

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
seL4_WPrintf(seL4_Writer* writer, const char *format, ...) {
    seL4_VaList args;
    seL4_Uint32 count;

    seL4_VaStart(args, format);
    count = seL4_Formatter(writer, format, args);
    seL4_VaEnd(args);
    return count;
}

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
seL4_Printf(const char *format, ...) {
    seL4_VaList args;
    seL4_Uint32 count;
    seL4_Writer writer = {
            .writeBeg = seL4_Null,
            .writeParam = writeChar,
            .writeEnd = seL4_Null,
            .data = seL4_Null,
    };

    seL4_VaStart(args, format);
    count = seL4_Formatter(&writer, format, args);
    seL4_VaEnd(args);
    return count;
}

