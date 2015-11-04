/* Returns the sample rate index */

#include "aachead.h"
#include "StdAfx.h"
int GetSRIndex(unsigned int sampleRate)
{
    if (92017 <= sampleRate) return 0;
    if (75132 <= sampleRate) return 1;
    if (55426 <= sampleRate) return 2;
    if (46009 <= sampleRate) return 3;
    if (37566 <= sampleRate) return 4;
    if (27713 <= sampleRate) return 5;
    if (23004 <= sampleRate) return 6;
    if (18783 <= sampleRate) return 7;
    if (13856 <= sampleRate) return 8;
    if (11502 <= sampleRate) return 9;
    if (9391 <= sampleRate) return 10;
	
    return 11;
}

