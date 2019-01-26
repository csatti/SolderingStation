#include "resources_manager.h"

static font_t _fontsArray[5];

bool_t guiResourcesManagerInit(void)
{

	// Open fonts
	//_fontsArray[0] = gdispOpenFont("general_sourcesanspro_bold20");
	_fontsArray[1] = gdispOpenFont("largetemp_loaded2110");
	_fontsArray[2] = gdispOpenFont("smalltemp_loaded24_aa");
	#ifdef THIN_BUILD
		_fontsArray[0] = gdispOpenFont("general_sourcesanspro_bold22");
		_fontsArray[3] = gdispOpenFont("general_sourcesanspro_bold16");
		_fontsArray[4] = gdispOpenFont("general_sourcesanspro_bold30");
		_fontsArray[5] = gdispOpenFont("general_sourcesanspro_bold12");
	#else
		_fontsArray[0] = gdispOpenFont("general_sourcesanspro_bold22_aa");
		_fontsArray[3] = gdispOpenFont("general_sourcesanspro_bold16_aa");
		_fontsArray[4] = gdispOpenFont("general_sourcesanspro_bold30_aa");
		_fontsArray[5] = gdispOpenFont("general_sourcesanspro_bold12_aa");
	#endif


	return TRUE;
}

GFXINLINE font_t gstudioGetFont(int fontIndex)
{
	return _fontsArray[fontIndex];
}

