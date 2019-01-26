#ifndef _USERFONTS_H
#define _USERFONTS_H

#include "main.h"

#ifdef THIN_BUILD
	#include "resources/general_sourcesanspro_bold30.c"
	#include "resources/general_sourcesanspro_bold22.c"
	#include "resources/general_sourcesanspro_bold16.c"
	#include "resources/general_sourcesanspro_bold12.c"
#else
	#include "resources/general_sourcesanspro_bold30_aa.c"
	#include "resources/general_sourcesanspro_bold22_aa.c"
	#include "resources/general_sourcesanspro_bold16_aa.c"
	#include "resources/general_sourcesanspro_bold12_aa.c"
#endif
#include "resources/largetemp_loaded2110.c"
#include "resources/smalltemp_loaded24_aa.c"

#endif // _USERFONTS_H
