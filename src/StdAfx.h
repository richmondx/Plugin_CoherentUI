/* CoherentUI_Plugin - for licensing and copyright see license.txt */

#pragma once

#if defined(XENON) || defined(PS3)
#define MAX_PLAYER_LIMIT 12
#else
#define MAX_PLAYER_LIMIT 16
#endif

// Insert your headers here
#include <platform.h>
#include <algorithm>
#include <vector>
#include <memory>
#include <list>
#include <functional>
#include <limits>
#include <smartptr.h>
#include <CryThread.h>
#include <Cry_Math.h>
#include <ISystem.h>
#include <I3DEngine.h>
#include <IInput.h>
#include <IConsole.h>
#include <ITimer.h>
#include <ILog.h>
#include <IGameplayRecorder.h>
#include <ISerialize.h>

#ifndef _FORCEDLL
#define _FORCEDLL
#endif

#ifndef CoherentUIPLUGIN_EXPORTS
#define CoherentUIPLUGIN_EXPORTS
#endif

#pragma warning(disable: 4018)  // conditional expression is constant

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <Coherent/Libraries/Config.h>
