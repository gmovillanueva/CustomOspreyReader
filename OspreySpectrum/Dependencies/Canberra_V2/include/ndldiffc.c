/*******************************************************************************
*
* This module contains routines used in the compression and decompression of
* spectra using the ND standard spectral compression algorithm. These routines
* should be portable, assuming little-endian architectures and no alignment
* restrictions.
*
********************************************************************************
*
* Revision History:
*
*	26-Jun-1991	RJH	Original (from NDLDIFF.FOR)
*	17-Jun-2002 TDA Modified for Windows platforms & added RLE compression
*   20-Oct-2003 PMc Fixed bugs in decompression: 1) when consecutive numbers are
*				the same the value stored should be the previous number plus zero.
*				2) when consecutive counts differ by the same amount but the diff
*				is greater than zero, the number stored should be the previous number
*				plus the difference.  change last argument so that it returns the
*				actual size of the decompressed	spectrum. Changed return code of
*				decompression routine
*
*******************************************************************************/


#include "platformTypes.h"
#include "pbsmsgs_h.h"
#include <memory.h>

/*******************************************************************************
*
* NDL_DIFFDATA_DECOMP decompresses compressed spectral data. It assumes that
* the input data consists of an integral number of channels and that you
* know how many channels there are (this routine will not work when getting
* data from an AIM, for example, where you neither of these conditions is true).
*
* Its calling format is:
*
*	status=ndl_diffdata_decomp(input,bytes,output,channels)
*
* where
*
*  "status" is the status of the operation; 1 for success and 0 for
*   failure; errors are not signaled.
*
*  "input" (input pointer to ?) points to the compressed data.
*
*  "bytes" (input ULONG, by value) is the number of bytes of data in "input".
*
*  "output" (input pointer to LONG) points to the destination storage for the
*   decompressed data.
*
*  "channels" (input ULONG, by value) is the number of channels in "input" and
*   "output".
*
*******************************************************************************/

LONG ndl_diffdata_decomp(unsigned char *input,
                         ULONG bytes,
                         LONG *output,
                         LONG *channels)

{

	LONG current_channel_value = 0;		/* holds current channel's value */
	unsigned char *input_ptr = input;	/* points to current byte in "input" */
	LONG *output_ptr = output;		/* points to current channel in "output" */
	unsigned char *end_input_ptr = input+bytes; /* points past last byte in "input" */
	LONG *end_output_ptr = output+(*channels);				/* points past last channel in "output" */
	SHORT short_value=0;
/*
* Set up a loop over the number of bytes in the input/channels in output:
* both of these should go to 0 at the end!
*
* This loop is a bit wierdly constructed: the idea is that the most common
* case should be the 8 bit delta value, so the code should (assumming the
* compiler doesn't do anything too smart) fall thru the tests most of the
* time; the branches should occur less frequently.
*/

	while(input_ptr < end_input_ptr && output_ptr < end_output_ptr) {

   /* Look for 3 byte format: 7F followed by 16 bit delta */

	   if(*input_ptr == (unsigned char ) 0x7F) goto do_16bit;

   /* Look for 5 byte format: 80 followed by 32 bit absolute value */

	   else if(*input_ptr == (unsigned char) 0x80) goto do_32bit;
#if 0
	   else if(*input_ptr == (unsigned char) 0x81) goto do_RLE;
#endif
   /* What it must be is an 8 bit delta */

	   current_channel_value += *((char *) input_ptr);
	   input_ptr++;

   /* Store the current channel value and advance to the next channel */

store:	   *output_ptr = current_channel_value;
	   output_ptr++;

	}

/*
* Make sure that we used up all the input bytes and filled up the output
* array and return.
*/

	*channels = output_ptr - output;
	if(input_ptr != end_input_ptr || output_ptr != end_output_ptr) 	return GEN_COMPRESSION_FAILURE;
	return 0;

/*
* These are the "less-frequently travelled" parts of the loop
*/

   /* Handle the 16 bit delta case */

do_16bit:
	memcpy(&short_value, input_ptr+1, sizeof(SHORT));
	current_channel_value += short_value;
	input_ptr += 3;
	goto store;

   /* Handle the 32 bit absolute case */

do_32bit:
	memcpy(&current_channel_value, input_ptr+1, sizeof(LONG));
	input_ptr += 5;
	goto store;

	/* Handle Run Length Encoding case */

#if 0
do_RLE:
	current_channel_value += *((char *) (input_ptr+2));
	cnt = (LONG) (input_ptr[1]-1);
	for (i=0; i<cnt; i++) {
		*output_ptr = current_channel_value;
		output_ptr++;
		current_channel_value += *((char *) (input_ptr+2));
	}
	input_ptr += 3;
	goto store;
#endif

}

/*******************************************************************************
*
* NDL_DIFFDATA_COMPRESS compresses compressed spectral data.
*
* Its calling format is:
*
*	ndl_diffdata_compress(input,channels,output,bytes)
*
* where
*
*  "input" (input pointer to LONG) points to the channel data to be compressed.
*
*  "channels" (input ULONG, by value) is the number of channels in "input".
*
*  "output" (input pointer to char) points to the destination storage for the
*   compressed data. The size of this array must be "channels" * 5 bytes.
*
*  "bytes" (output ULONG, by reference) is the number of bytes actually
*   written to "output".
*
*******************************************************************************/

LONG ndl_diffdata_compress(LONG *input,
                           ULONG channels,
                           char *output,
                           ULONG *bytes)

{

	char *output_ptr = output;	/* points to current spot in output buffer */
	LONG *input_ptr = input;	/* points to current channel in input buffer */
	LONG last_channel = 0;		/* value of the last channel we processed */
	LONG delta;					/* difference between the current channel and the last one */
	ULONG i;					/* loop counter */
	SHORT Delta;
/*
* Loop over all the channels in the input buffer, converting them to the
* differential format in the output buffer.
*/

	for(i=0; i < channels; i++) {

   /*
   * compute the delta between the current and last channel
   */

	   delta = *input_ptr - last_channel;

   /*
   * If the delta can be contained in a byte (less the two escape characters),
   * just output the byte.
   */

	   if ((delta > -127 && delta < 127)) {

		*(output_ptr) = (char) delta;
		output_ptr++;

   /*
   * If the delta can be contained in two bytes, output a 7F and the 16 bit
   * delta value.
   */

	   } else if(delta > -32767 && delta < 32768) {

		*output_ptr = 0x7f;
		//*((SHORT *) (output_ptr+1)) = (SHORT) delta;
		Delta = (SHORT) delta;
		memcpy(output_ptr+1, &Delta, sizeof(Delta));
		output_ptr += 3;

   /*
   * The delta is bigger than 16 bits, just output the absolute value of the
   * channel preceeded by an 80.
   */

	   } else {

		*output_ptr = (char) 0x80;
		memcpy(output_ptr+1, input_ptr, sizeof(LONG));
		//*((LONG *) (output_ptr+1)) = *input_ptr;
		output_ptr += 5;
	   }

   /*
   * Remember what this channel's value is, and advance to the next one
   */

	   last_channel = *input_ptr;
	   input_ptr++;

	}

/*
* All done, return the number of bytes in the compressed spectrum
*/

	*bytes = output_ptr - output;
#if 0
	output_ptr = output;
	while ((ULONG) (input_bptr - output) < *bytes) {
	    if(*input_bptr == (char) 0x7f) {
	        output_ptr[0] = input_bptr[0];
	        output_ptr[1] = input_bptr[1];
	        output_ptr[2] = input_bptr[2];
	        output_ptr += 3;
	        input_bptr +=3;
	    } else if(*input_bptr == (char) 0x80) {
	        output_ptr[0] = input_bptr[0];
	        output_ptr[1] = input_bptr[1];
	        output_ptr[2] = input_bptr[2];
	        output_ptr[3] = input_bptr[3];
	        output_ptr[4] = input_bptr[4];
	        output_ptr += 5;
	        input_bptr +=5;
	    } else {
	        j = 1;
	        while((j < 254) &&
	              (((input_bptr - output) + j) < *bytes) &&
	              (input_bptr[0] == input_bptr[j])) {
				j += 1;
	        }
	        if(j > 2) {
	            output_ptr[0] = (char) 0x81;
	            output_ptr[1] = (char) j; /* The count */
				output_ptr[2] = input_bptr[j-1];
	            input_bptr += j;
	            output_ptr += 3;
	        } else {
	            *output_ptr = *input_bptr;
	            output_ptr++;
	            input_bptr++;
	        }
	    }
	}

	*bytes = output_ptr - output;
#endif
	return 0;

}
