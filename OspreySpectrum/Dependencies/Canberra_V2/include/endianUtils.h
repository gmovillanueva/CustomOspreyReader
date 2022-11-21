/*
 * endian.h
 *
 *  Created on: Mar 6, 2009
 *      Author: troyanderson
 */

#ifndef ENDIAN_H_
#define ENDIAN_H_


/* Macros for byte swapping data from Motorola
     680x0 family workstations for use on an Intel 80x86 family.
     These macros can be used to convert from Intel 80x86 to
     Motorola as well

     TDA Created in 1995
*/
#ifdef BIG_ENDIAN_SWAP

/*
   680x0  (double) byte order 8.7.6.5.4.3.2.1
   80x86  (double) byte order 1.2.3.4.5.6.7.8
*/
#define SWAP8BYTES(dVal) \
{\
   char *pszVal, szSVal; \
   pszVal = (char *) (dVal); \
   szSVal = *pszVal; \
   *pszVal = *(pszVal+7); \
   *(pszVal+7) = szSVal; \
   szSVal = *(pszVal+1); \
   *(pszVal+1) = *(pszVal+6); \
   *(pszVal+6) = szSVal; \
   szSVal = *(pszVal+2); \
   *(pszVal+2) = *(pszVal+5); \
   *(pszVal+5) = szSVal; \
}

/*
   680x0  (float or long) byte order 4.3.2.1
   80x86  (float or long) byte order 1.2.3.4
*/
#define SWAP4BYTES(rVal) \
{\
   char *pszVal, szSVal; \
   pszVal = (char *) (rVal); \
   szSVal = *pszVal; \
   *pszVal = *(pszVal+3); \
   *(pszVal+3) = szSVal; \
   szSVal = *(pszVal+1); \
   *(pszVal+1) = *(pszVal+2); \
   *(pszVal+2) = szSVal; \
}
/*
   680x0   (short) byte order 2.1
   80x88   (short) byte order 1.2
*/
#define SWAP2BYTES(sVal) \
{ \
   char *pszVal, szSVal; \
   pszVal = (char *) (sVal); \
   szSVal = *pszVal; \
   *pszVal = *(pszVal+1); \
   *(pszVal+1) = szSVal; \
}


/* Byte swapping macro */
#define SWAPNBYTES(sVal,N) \
{ \
   if (8 == N) {\
	   SWAP8BYTES(sVal)\
   }\
   else if (4 == N) {\
	   SWAP4BYTES(sVal)\
   }\
   else if (2 == N) {\
	   SWAP2BYTES(sVal)\
   }\
}
#else
/* All other platforms do not need these defs */

#define SWAP4BYTES(a) {}
#define SWAP2BYTES(a) {}
#define SWAP8BYTES(a) {}
#define SWAPNBYTES(a, b) {}
#endif
#endif /* ENDIAN_H_ */
