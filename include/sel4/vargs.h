/*
 * copyright 2015, Wink Saville
 *
 * this software may be distributed and modified according to the terms of
 * the bsd 2-clause license. note that no warranty is provided.
 * see "license_bsd2.txt" for details.
 */

#ifndef _SEL4_VARGS_H_
#define _SEL4_VARGS_H_

#define seL4_VaList __builtin_va_list
#define seL4_VaStart(list, last_param) __builtin_va_start(list, last_param)
#define seL4_VaEnd(list) __builtin_va_end(list)
#define seL4_VaArg(list, type) __builtin_va_arg(list, type)

#endif
