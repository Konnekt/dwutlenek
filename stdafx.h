// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <stdstring.h>
#include <io.h>
#include <fstream>
#include <list>
#include <time.h>
#include <direct.h>
#include <Winsock2.h>
extern "C" {
#include "base64.h"
}

using namespace std;

#include "konnekt/plug_export.h"
#include "konnekt/ui.h"
#include "konnekt/plug_func.h"
#include "konnekt/knotify.h"

#include "libtlen2.h"

#include "konnekt/dwutlenek.h"
using namespace kJabber;
using namespace dwuTlenek;
#include "jabber_class.h"
#include "dwutlenek_class.h"
#include "dwutlenek_main.h"



// TODO: reference additional headers your program requires here
