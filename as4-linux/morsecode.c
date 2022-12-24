// Morse Code Encodings (from http://en.wikipedia.org/wiki/Morse_code)
//   Encoding created by Brian Fraser. Released under GPL.
//
// Encoding description:
// - msb to be output first, followed by 2nd msb... (left to right)
// - each bit gets one "dot" time.
// - "dashes" are encoded here as being 3 times as long as "dots". Therefore
//   a single dash will be the bits: 111.
// - ignore trailing 0's (once last 1 output, rest of 0's ignored).
// - Space between dashes and dots is one dot time, so is therefore encoded
//   as a 0 bit between two 1 bits.
//
// Example:
//   R = dot   dash   dot       -- Morse code
//     =  1  0 111  0  1        -- 1=LED on, 0=LED off
//     =  1011 101              -- Written together in groups of 4 bits.
//     =  1011 1010 0000 0000   -- Pad with 0's on right to make 16 bits long.
//     =  B    A    0    0      -- Convert to hex digits
//     = 0xBA00                 -- Full hex value (see value in table below)
//
// Between characters, must have 3-dot times (total) of off (0's) (not encoded here)
// Between words, must have 7-dot times (total) of off (0's) (not encoded here).
//

#include "includes/morsecode.h"

static unsigned short morsecode_codes[] = {
	0xB800, // A 1011 1
	0xEA80, // B 1110 1010 1
	0xEBA0, // C 1110 1011 101
	0xEA00, // D 1110 101
	0x8000, // E 1
	0xAE80, // F 1010 1110 1
	0xEE80, // G 1110 1110 1
	0xAA00, // H 1010 101
	0xA000, // I 101
	0xBBB8, // J 1011 1011 1011 1
	0xEB80, // K 1110 1011 1
	0xBA80, // L 1011 1010 1
	0xEE00, // M 1110 111
	0xE800, // N 1110 1
	0xEEE0, // O 1110 1110 111
	0xBBA0, // P 1011 1011 101
	0xEEB8, // Q 1110 1110 1011 1
	0xBA00, // R 1011 101
	0xA800, // S 1010 1
	0xE000, // T 111
	0xAE00, // U 1010 111
	0xAB80, // V 1010 1011 1
	0xBB80, // W 1011 1011 1
	0xEAE0, // X 1110 1010 111
	0xEBB8, // Y 1110 1011 1011 1
	0xEEA0	// Z 1110 1110 101
};

// Return the flash code based on the input character.
// Returns 0 if not a-z or A-Z.
unsigned short MorseCode_getFlashCode(char ch)
{
	unsigned short flashCode = 0;

	// Convert to lower case:
	// Lower case has an extra bit (0x20) set. Remove that bit.
	ch &= ~(0x20);

	// If valid letter, look it up in the array:
	if (ch >= 'A' && ch <= 'Z')
	{
		flashCode = morsecode_codes[ch - 'A'];
	}
	return flashCode;
}

// Convert string to morse code bits and Have 64 items of 32 bits each

void MorseCode_getBinaryMorseCode(char *message, uint32_t *buff, int *numBits)
{
	char curChar;		  // the current character we are evaluating in message
	uint32_t buff_32 = 0; // holds the temporary buffer of continuous values
	uint32_t ret = 0;	  // holds the binary representation of a character in morsecode
	int location = 0;	  // holds the location we are in the buff array (index)
	int count = 0;		  // holds the current bit we are supposed to change in buff_32
	*numBits = count;
	for (int i = 0; message[i]; i++)
	{
		curChar = *(message + i); // get the character at that point in the array
		if (curChar == ' ')
		{
			int numspaces = TIME_BITS_FOR_SPACES;
			if (i > 0)
			{
				char prevChar = *(message + i - 1);
				if (prevChar == ' ')
				{
					numspaces = 0;
				}
				else
				{
					numspaces = 4; // so that the 3 after the last character plus the current 4 makes 7 total spaces
				}
			}

			for (int j = 0; j < numspaces; j++)
			{
				buff_32 &= ~(0x80000000 >> count); // set the bit of interest to 0
				count++;
				if (count > 31)
				{
					buff[location] = buff_32;
					buff_32 = 0;
					location++;
					*numBits += count;
					count = 0;
				}
			}
			continue;
		}
		ret = MorseCode_getFlashCode(curChar); // get the short associated with this character in morse code
		while (ret)
		{
			uint32_t MSB = (ret & 0x8000) << 16;
			// printf("count:%d\n", count);
			buff_32 &= ~(0x80000000 >> count); // clear the bit I want to change
			buff_32 |= (MSB >> count);		   // set the bit cleared spot

			ret <<= 1; // shift the short left by 1
			ret = ret & 0xFFFF;
			count++;
			if (count > 31)
			{
				buff[location] = buff_32;
				buff_32 = 0;
				location++;
				*numBits += count;
				count = 0;
			}
		}
		if (message[i + 1])
		{
			for (int j = 0; j < TIME_BITS_BETWEEN_CHARS; j++)
			{
				buff_32 &= ~(0x80000000 >> count); // set the bit of interest to 0
				count++;
				if (count > 31)
				{
					buff[location] = buff_32;
					buff_32 = 0;
					location++;
					*numBits += count;
					count = 0;
				}
			}
		}
	}
	if (buff_32)
	{ 	
		buff[location] = buff_32;
		*numBits += count;
	}
}
// Note: you may need to shift the bits to include the 0's in the MSB positions
// because the values represented will drop 0's on the right
// so you should read always do a buff[i] &= 0xFFFFFFFF to get all the 0's on the right before displaying
// or you could simply read and pop off 32 times and display X if 1 and _ if 0