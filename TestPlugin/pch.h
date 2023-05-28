// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// STL
#include <memory>
#include <iostream>

// MFC
#include <afxwin.h>
#include <afxext.h>
#include <afxpriv.h>

// Win32
#include <tlhelp32.h>

// ObjectARX
#include "rxregsvc.h"
#include "acutads.h"
#include "accmd.h"
#include "aced.h"
#include "rxmfcapi.h"
#include "acedCmdNF.h"
#include "adscodes.h"
#include "AcPlPlotProgress.h"
#include "acplplotreactor.h"
#include "acpublishreactors.h"
#include "acplplotreactormgr.h"

#include "MyDatabaseReactor.h"
#include "MyDocManagerReactor.h"
#include "MyEditorReactor.h"
#include "MyPlotReactor.h"
#include "MyPublishReactor.h"
#include "Reactors.h"
#include "PublishWatcher.h"

#include "resource1.h"



// add headers that you want to pre-compile here

#endif //PCH_H
