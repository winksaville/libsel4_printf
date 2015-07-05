/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#include <sel4_types.h>
#include <sel4_vargs.h>
#include <sel4_putchar.h>
#include <sel4_printf.h>

static seL4_Uint32
print_string(const char *s)
{
    seL4_Uint32 n;

    for (n = 0; *s; s++, n++) {
        seL4_PutChar(*s);
    }

    return n;
}

static seL4_Uint32
xdiv(seL4_Uint32 x, seL4_Uint32 denom)
{
    switch (denom) {
    case 16:
        return x / 16;
    case 10:
        return x / 10;
    default:
        return 0;
    }
}

static seL4_Uint32
xmod(seL4_Uint32 x, seL4_Uint32 denom)
{
    switch (denom) {
    case 16:
        return x % 16;
    case 10:
        return x % 10;
    default:
        return 0;
    }
}

seL4_Uint32
print_seL4_Uint32(seL4_Uint32 x, seL4_Uint32 ui_base)
{
    char out[11];
    seL4_Uint32 i, j;
    seL4_Uint32 d;

    /*
     * Only base 10 and 16 supported for now. We want to avoid invoking the
     * compiler's support libraries through doing arbitrary divisions.
     */
    if (ui_base != 10 && ui_base != 16) {
        return 0;
    }

    if (x == 0) {
        seL4_PutChar('0');
        return 1;
    }

    for (i = 0; x; x = xdiv(x, ui_base), i++) {
        d = xmod(x, ui_base);

        if (d >= 10) {
            out[i] = 'a' + d - 10;
        } else {
            out[i] = '0' + d;
        }
    }

    for (j = i; j > 0; j--) {
        seL4_PutChar(out[j - 1]);
    }

    return i;
}


static seL4_Uint32
print_seL4_Uint64(seL4_Uint64 x, seL4_Uint32 ui_base)
{
    seL4_Uint32 upper, lower;
    seL4_Uint32 n = 0;
    seL4_Uint32 mask = 0xF0000000u;

    /* only implemented for hex, decimal is harder without 64 bit division */
    if (ui_base != 16) {
        return 0;
    }

    /* we can't do 64 bit division so break it up into two hex numbers */
    upper = (seL4_Uint32) (x >> 32llu);
    lower = (seL4_Uint32) x;

    /* print first 32 bits if they exist */
    if (upper > 0) {
        n += print_seL4_Uint32(upper, ui_base);

        /* print leading 0s */
        while (!(mask & lower)) {
            seL4_PutChar('0');
            n++;
            mask = mask >> 4;
        }
    }

    /* print last 32 bits */
    n += print_seL4_Uint32(lower, ui_base);

    return n;
}


static int
vprintf(const char *format, seL4_VaList ap)
{
    seL4_Uint32 n;
    seL4_Uint32 formatting;

    if (!format) {
        return 0;
    }

    n = 0;
    formatting = 0;
    while (*format) {
        if (formatting) {
            switch (*format) {
            case '%':
                seL4_PutChar('%');
                n++;
                format++;
                break;

            case 'd': {
                int x = seL4_VaArg(ap, int);

                if (x < 0) {
                    seL4_PutChar('-');
                    n++;
                    x = -x;
                }

                n += print_seL4_Uint32((seL4_Uint32)x, 10);
                format++;
                break;
            }

            case 'u':
                n += print_seL4_Uint32(seL4_VaArg(ap, seL4_Uint32), 10);
                format++;
                break;

            case 'x':
                n += print_seL4_Uint32(seL4_VaArg(ap, seL4_Uint32), 16);
                format++;
                break;

            case 'p': {
                seL4_Uint32 p = seL4_VaArg(ap, seL4_Uint32);
                if (p == 0) {
                    n += print_string("(nil)");
                } else {
                    n += print_string("0x");
                    n += print_seL4_Uint32(p, 16);
                }
                format++;
                break;
            }

            case 's':
                n += print_string(seL4_VaArg(ap, char *));
                format++;
                break;

            case 'l':
		// Support llx only
                if (*(format + 1) == 'l' && *(format + 2) == 'x') {
                    seL4_Uint64 arg = seL4_VaArg(ap, seL4_Uint64);
                    n += print_seL4_Uint64(arg, 16);
                    format += 3;
                }
                break;
            default:
                format++;
                break;
            }

            formatting = 0;
        } else {
            switch (*format) {
            case '%':
                formatting = 1;
                format++;
                break;

            default:
                seL4_PutChar(*format);
                n++;
                format++;
                break;
            }
        }
    }

    return n;
}

/**
 * Print a formated string to a "terminal". This supports a
 * subset of the typical libc printf:
 *   - %% ::= prints a percent
 *   - %d ::= prints a positive or negative long base 10
 *   - %u ::= prints an seL4_Uint32 base 10
 *   - %x ::= prints a seL4_Uint32 base 16
 *   - %p ::= prints a seL4_Uint32 assuming its a pointer base 16 with 0x prepended
 *   - %s ::= prints a string
 *   - %llx ::= prints a seL4_Uint32 long base 16
 */
seL4_Uint32
seL4_Printf(const char *format, ...)
{
    seL4_VaList args;
    seL4_Uint32 i;

    seL4_VaStart(args, format);
    i = vprintf(format, args);
    seL4_VaEnd(args);
    return i;
}
