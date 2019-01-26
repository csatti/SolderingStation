#ifndef _WIDGETSTYLES_H
#define _WIDGETSTYLES_H

#include "gfx.h"

static const GWidgetStyle SelectedElement = {
	HTML2COLOR(0x00FF00),              // background
	HTML2COLOR(0x000000),              // focus

	// Enabled color set
	{
		HTML2COLOR(0x000000),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

static const GWidgetStyle UnSelectedElement = {
	HTML2COLOR(0x000000),              // background
	HTML2COLOR(0x000000),              // focus

	// Enabled color set
	{
		HTML2COLOR(0x888888),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};



// WidgetStyle: DefaultStyle
static const GWidgetStyle DefaultStyle = {
	HTML2COLOR(0x000000),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0xB4FFFF),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

// WidgetStyle: WhiteButton
static const GWidgetStyle WhiteButton = {
	HTML2COLOR(0xFFFFFF),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0x000000),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xECECEC),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

// WidgetStyle: GreenButton
static const GWidgetStyle GreenButton = {
	HTML2COLOR(0xFFFFFF),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0x000000),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x55FF00),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

// WidgetStyle: RedButton
static const GWidgetStyle RedButton = {
	HTML2COLOR(0xFFFFFF),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0x000000),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0xFF0000),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

// WidgetStyle: Parameter
static const GWidgetStyle TemperatureSPStyle = {
	HTML2COLOR(0x000000),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0x77DD77),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x000000),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

// WidgetStyle: Parameter
static const GWidgetStyle ParameterNameStyle = {
	HTML2COLOR(0x000000),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x000000),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

// WidgetStyle: Parameter
static const GWidgetStyle ParameterStyle = {
	HTML2COLOR(0x000000),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0xA0F0A0),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x000000),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0x808080),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

// WidgetStyle: ParameterDesc
static const GWidgetStyle ParameterDescStyle = {
	HTML2COLOR(0x000000),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0x808080),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x000000),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

// WidgetStyle: AlarmStyle
static const GWidgetStyle AlarmStyle = {
	HTML2COLOR(0x000000),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0xF06060),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x000000),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

// WidgetStyle: WarningStyle
static const GWidgetStyle WarningStyle = {
	HTML2COLOR(0x000000),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0xF0F020),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x000000),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

// WidgetStyle: InfoStyle
static const GWidgetStyle InfoStyle = {
	HTML2COLOR(0x000000),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0xF0F0F0),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x000000),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};

// WidgetStyle: SleepLabel
static const GWidgetStyle SleepLabel = {
	HTML2COLOR(0x00AAFF),              // background
	HTML2COLOR(0x2A8FCD),              // focus

	// Enabled color set
	{
		HTML2COLOR(0xFFFFFF),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	},

	// Disabled color set
	{
		HTML2COLOR(0xC0C0C0),         // text
		HTML2COLOR(0x808080),         // edge
		HTML2COLOR(0xE0E0E0),         // fill
		HTML2COLOR(0xC0E0C0)          // progress (active area)
	},

	// Pressed color set
	{
		HTML2COLOR(0x404040),         // text
		HTML2COLOR(0x404040),         // edge
		HTML2COLOR(0x808080),         // fill
		HTML2COLOR(0x00E000)          // progress (active area)
	}
};



#endif // _WIDGETSTYLES_H
