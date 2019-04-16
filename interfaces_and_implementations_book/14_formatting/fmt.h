#ifndef FMT_INCLUDED
#define FMT_INCLUDED

#include <stdarg.h>
#include <stdio.h>
#include "except.h"

#define T Fmt_T
typedef void (*T)(int code, va_list *app,
	int put(int c, void *cl), void *cl,
	unsigned char flags[256], int width, int precision);

extern char *Fmt_flags;
extern const Except_T Fmt_Overflow;


////////////////////////
// exported functions //
////////////////////////

extern void Fmt_fmt  (int put (int c, void *cl), void *cl,
	const char *fmt, ...);
extern void Fmt_vfmt (int put (int c, void *cl), void *cl,
	const char *fmt, va_list ap);

extern void Fmt_print (const char *fmt, ...);
extern void Fmt_fprint (FILE *stream, const char *fmt, ...);
extern int Fmt_sfmt (char *buf, int size, const char *fmt, ...);
extern int Fmt_vsfmt (char *buf, int size, const char *fmt, va_list ap);

extern char *Fmt_string (const char *fmt, ...);
extern char *Fmt_vstring (const char *fmt, va_list ap);

extern T Fmt_register (int code, T cvt);

///////////////////////////////////
// Default conversion specifiers //
///////////////////////////////////

//  -----------------------------------------------------------------------------------------
// |  conversion specifier 	|																 |
// |     argument type 		|	Description													 |
//  -----------------------------------------------------------------------------------------
// |	c 					| The argument is interpreted as an unsigned character and 		 |
// |   int 					| is emitted. 													 |
//  -----------------------------------------------------------------------------------------
// | 	d 					| The argument is converted to its signed decimal representation.|
// |   int 					| The precision, if given, specifies the minimun number of 		 |
// |						| digits; leading zeros are added, if necessary. The default 	 |
// |						| precision is one. If both the - and 0 flags appear, or if a 	 |
// |						| precision is given, the 0 flag is ignored. If both the + and 	 |
// |						| space flags appear, the space flag is ignored. If the argument |
// |						| and the precision are zero, there are no characters in the 	 |
// |						| converted result. 											 |
//  -----------------------------------------------------------------------------------------
// |	o u x				| The argument is converted to its unsigned representation 		 |
// |   unsigned 			| in octal (o), decimal (u), or hexadecimal (x). For x, the 	 |
// | 						| letters abcdef are used for the digits whose values 			 |
// | 						| exceed 9. The flags and precision are interpreted as for d. 	 |
//  -----------------------------------------------------------------------------------------
// |	f 					| The argument is converted to its decimal representation 		 |
// |  double 				| with the form x.y. The precision gives the number of digits 	 |
// | 						| to the right of the decimal point; the default is 6. If the 	 |
// |						| precision is given explicitly as 0, the decimal point is 		 |
// |						| omitted. When a decimal point appears, x has at least one 	 |
// |						| digit. It is a checked runtime error for the precision to 	 |
// |						| exceed 99. The flags are interpreted as for d. 				 |
//  -----------------------------------------------------------------------------------------
// |	e 					| The argument is converted to its decimal representation 		 |
// |  double				| with the form x.y e +/- p. x is always one digit and p 		 |
// |						| is always two digits. The flags and precision are 			 |
// |						| interpreted as for d. 										 |
//  -----------------------------------------------------------------------------------------
// |	g 					| The argument is converted to its decimal representation 		 |
// | double					| as for f or e depending on its value. The precision gives 	 |
// | 						| the number of significant digits; the default is one. The 	 |
// |						| result has the form x.y e +/- p if p is less than -4 or p 	 |
// |						| is greater than or equal to the precision; otherwise, the 	 |
// |						| result has the form x.y. There are no trailing zeros in y, 	 |
// |						| and the decimal point is omited when y is zero. It is a 		 |
// |						| checked runtime error for the precision to exceed 99. 		 |
//  -----------------------------------------------------------------------------------------
// |	p 					| The argument is converted to the hexadecimal representation 	 |
// | void *					| of its value as for u. The flags and precision are 			 |
// |						| interpreted as for d. 										 |
//  -----------------------------------------------------------------------------------------
// |	s 					| Successive characters from the argument are emitted until 	 |
// | void *					| a null character is encountered or the number of 				 |
// |						| characters given by an explicit precision have been 			 |
// |						| emitted. All flags except - are ignored. 						 |
//  -----------------------------------------------------------------------------------------


extern void Fmt_putd (const char *str, int len,
	int put (int c, void *cl), void *cl,
	unsigned char flags[256], int width, int precision);
extern void Fmt_puts (const char *str, int len,
	int put (int c, void *cl), void *cl,
	unsigned char flags[256], int width, int precision);

#undef T
#endif