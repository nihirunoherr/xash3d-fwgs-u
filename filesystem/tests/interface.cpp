#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "port.h"
#include "build.h"
#include "VFileSystem009.h"
#include "filesystem.h"

#if XASH_POSIX
#include <dlfcn.h>
#define LoadLibrary( x ) dlopen( x, RTLD_NOW )
#define GetProcAddress( x, y ) dlsym( x, y )
#define FreeLibrary( x ) dlclose( x )
#elif XASH_WIN32
#include <windows.h>
#endif

void *g_hModule;
FSAPI g_pfnGetFSAPI;
typedef void *(*pfnCreateInterface_t)( const char *, int * );
pfnCreateInterface_t g_pfnCreateInterface;
fs_api_t g_fs;
fs_globals_t *g_nullglobals;

static qboolean LoadFilesystem( void )
{
	int temp = -1;

	g_hModule = LoadLibrary( "filesystem_stdio." OS_LIB_EXT );
	if( !g_hModule )
		return false;

	// check our C-style interface existence
	g_pfnGetFSAPI = (FSAPI)GetProcAddress( g_hModule, GET_FS_API );
	if( !g_pfnGetFSAPI )
		return false;

	if( !g_pfnGetFSAPI( FS_API_VERSION, &g_fs, &g_nullglobals, NULL ))
		return false;

	// check Valve-style interface existence
	g_pfnCreateInterface = (pfnCreateInterface_t)GetProcAddress( g_hModule, "CreateInterface" );
	if( !g_pfnCreateInterface )
		return false;

	if( !g_pfnCreateInterface( "VFileSystem009", &temp ) || temp != 0 )
		return false;

	if( !g_pfnCreateInterface( FS_API_CREATEINTERFACE_TAG, &temp ) || temp != 0 )
		return false;

	return true;
}

int main( void )
{
	if( !LoadFilesystem() )
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}