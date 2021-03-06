/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-99,2003 by Solar Designer
 *
 * ...with changes in the jumbo patch, by JimF and magnum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 */

/*
 * Miscellaneous routines.
 */

#ifndef _JOHN_MISC_H
#define _JOHN_MISC_H

#include <stdio.h>
#ifndef _MSC_VER
#include <strings.h>
#else
#undef inline
#define inline static
#endif
#include <string.h>

/*
 * Exit on error. Logs the event, closes john.pot and the log file, and
 * terminates the process with non-zero exit status.
 */
extern void error(void);

/*
 * Similar to perror(), but supports formatted output, and calls error().
 */
extern void pexit(char *format, ...)
#ifdef __GNUC__
	__attribute__ ((format (printf, 1, 2)));
#else
	;
#endif

/*
 * Attempts to write all the supplied data. Returns the number of bytes
 * written, or -1 on error.
 */
extern int write_loop(int fd, const char *buffer, int count);

/*
 * Similar to fgets(), but doesn't leave the newline character in the buffer,
 * and skips to the end of long lines. Handles both Unix and DOS style text
 * files correctly.
 */
extern char *fgetl(char *s, int size, FILE *stream);

/*
 * Similar to strncpy(), but terminates with only one NUL if there's room
 * instead of padding to the supplied size like strncpy() does.
 */
extern char *strnfcpy(char *dst, const char *src, int size);

/*
 * Similar to the above, but always NUL terminates the string.
 */
extern char *strnzcpy(char *dst, const char *src, int size);

/*
 * Similar to the strnzcpy, but returns the length of the string.
 */
extern int strnzcpyn(char *dst, const char *src, int size);

/*
 * Similar to strncat(), but total buffer size is supplied, and always NUL
 * terminates the string.
 */
extern char *strnzcat(char *dst, const char *src, int size);

/*
 * Portable basename() function.  DO NOT USE basename().  Use this
 * proper working equivelent.  The _r version is thread safe. In the
 * _r version, pass in a buffer that is at least strlen(name)+1 bytes
 * long, however, PATH_BUFFER_SIZE+1 can also be used.
 *
 *  here is what defined:
 *    if name is null, or points to a null string (0 byte), then a '.' is returned.
 *    if name is all / chars (or \ chars), then a single / (or \) is returned.
 *    DOS drive letters are ignored.
 *    / or \ chars are properly handled.
 *    Trailing / (or \) are removed, IF there was real path data in there.
 *
 *  here are some examples:
 *    jtr_basename("/user/lib")      == lib
 *    jtr_basename("/user/")         == user
 *    jtr_basename("/")              == /
 *    jtr_basename("//")             == /
 *    jtr_basename("///")            == /
 *    jtr_basename("//user//lib//")  == lib
 *    jtr_basename("c:\\txt.doc")    == txt.doc
 *    jtr_basename("c:txt.doc")      == txt.doc
 *    jtr_basename("c:b/c\\txt.doc/")== txt.doc
 *    jtr_basename("c:\\txt.doc\\")  == txt.doc
 *    jtr_basename("c:")             == .
 *    jtr_basename("")               == .
 *    jtr_basename(NULL)             == .
 *    jtr_basename("\\user\\lib")    == lib
 *    jtr_basename("\\user\\")       == user
 *    jtr_basename("\\")             == \
 *    jtr_basename("\\\\")           == \
 *    jtr_basename("one")            == one
 */
extern char *jtr_basename(const char *name);
extern char *jtr_basename_r(const char *name, char *buf);
#undef basename
#define basename(a) jtr_basename(a)

/*
 * Removes suffixes frome src.
 */
extern char *strip_suffixes(const char *src, const char *suffixes[], int count);

/* Return the first occurrence of NEEDLE in HAYSTACK.
   Faster implementation by Christian Thaeter <ct at pipapo dot org>
   http://sourceware.org/ml/libc-alpha/2007-12/msg00000.html
   LGPL 2.1+ */
#ifndef INCLUDED_FROM_MISC_C
#if __GNUC__ && !__GNUC_STDC_INLINE__
extern
#endif
inline
#endif
void *jtr_memmem(const void *haystack, size_t haystack_len,
                        const void *needle, size_t needle_len)
{
	/* not really Rabin-Karp, just using additive hashing */
	char* haystack_ = (char*)haystack;
	char* needle_ = (char*)needle;
	int hash = 0;		/* static hash value of the needle */
	int hay_hash = 0;	/* rolling hash over the haystack */
	char* last;
	size_t i;

	if (haystack_len < needle_len)
		return NULL;

	if (!needle_len)
		return haystack_;

	/* initialize hashes */
	for (i = needle_len; i; --i)
	{
		hash += *needle_++;
		hay_hash += *haystack_++;
	}

	/* iterate over the haystack */
	haystack_ = (char*)haystack;
	needle_ = (char*)needle;
	last = haystack_+(haystack_len - needle_len + 1);
	for (; haystack_ < last; ++haystack_)
	{
		if (hash == hay_hash &&
		    *haystack_ == *needle_ &&	/* prevent calling memcmp */
		    !memcmp (haystack_, needle_, needle_len))
			return haystack_;

		/* roll the hash */
		hay_hash -= *haystack_;
		hay_hash += *(haystack_+needle_len);
	}

	return NULL;
}

/*
 * Converts a string to lowercase.
 */
#ifndef _MSC_VER
extern char *strlwr(char *s);
extern char *strupr(char *s);
#else
#define bzero(a,b) memset(a,0,b)
#define strlwr _strlwr
#define strupr _strupr
#define strdup _strdup
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define alloca _alloca
#define unlink _unlink
#define fileno _fileno
#pragma warning (disable : 4018 297 )
#undef inline
#define inline _inline
#define snprintf sprintf_s
#define atoll _atoi64
#endif

#ifndef __has_feature
# define __has_feature(x) 0
#endif

#if /* is ASAN enabled? */ \
    __has_feature(address_sanitizer) /* Clang */ || \
    defined(__SANITIZE_ADDRESS__)  /* GCC 4.8.x */
  #define ATTRIBUTE_NO_ADDRESS_SAFETY_ANALYSIS \
        __attribute__((no_address_safety_analysis)) \
        __attribute__((noinline))
#else
  #define ATTRIBUTE_NO_ADDRESS_SAFETY_ANALYSIS
#endif

#endif
