#include "Hooks.h"
#include "./../mhook/mhook-lib/mhookwrapper.h"

#define GLOG_NO_ABBREVIATED_SEVERITIES
#define GOOGLE_GLOG_DLL_DECL
#include <glog/logging.h>

DWORD realTestProc = 0x641CD