#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x )	\
	if ( NULL != x )	\
{	\
	x->Release( );	\
	x = NULL;	\
}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE( x )	\
	if ( NULL != x )	\
{	\
	delete x;	\
	x = NULL;	\
}
#endif

#pragma comment(lib,"STRMBASE.lib")
#include "streams.h"
#include <atlbase.h>
#include <qedit.h>
#include "SampleGrabberCB.h"