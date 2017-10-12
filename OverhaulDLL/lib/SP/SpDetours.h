// Author: Sean Pesce

#pragma once

#ifndef _SP___HOOK___PROCEDURE___H_
	#define _SP___HOOK___PROCEDURE___H_


#ifdef _WIN32

#include <Windows.h>


/*
	Initializes variables and types for the proceeding attach/detach macros:

	    -Creates a type definition for the original function using the return type, calling convention, and argument list.
		-Creates a handle for the original function.
		-Creates the declaration for the new function that is called in place of the original.
		
	WARNING: the new function is only declared, and must be followed by a body (see the 3 commented-out lines below the macro) or a semicolon,
	in which case the entire function will need to be defined somewhere else.
*/
#define SP_HOOK_INIT(P, RETURN_DATATYPE, CALLING_CONVENTION, ...)typedef RETURN_DATATYPE##(CALLING_CONVENTION *t##P)(##__VA_ARGS__);t##P o##P;RETURN_DATATYPE hk##P(##__VA_ARGS__)
//	{
//		return o##P(args);
//
//		// Outside of a preprocessor macro, the previous line would look like this:
//      // return oP(args);
//	}


// Attempts to hook the function P in the library lib. If successful, system error code is set to ERROR_SUCCESS. If the macro fails, system error code is set to ERROR_PROC_NOT_FOUND.
#define SP_HOOK_PROC_ATTACH(P, lib) if((o##P=(t##P)GetProcAddress(GetModuleHandle(lib),#P))){DetourTransactionBegin();DetourUpdateThread(GetCurrentThread());DetourAttach(&(PVOID&)o##P,hk##P);DetourTransactionCommit();}else{o##P=NULL;}if(o##P==NULL){SetLastError(ERROR_PROC_NOT_FOUND);}else{SetLastError(ERROR_SUCCESS);}



// Attempts to hook the function P in the library lib. This macro loops indefinitely (with a wait time of ms milliseconds between loop iterations) until the procedure is found or the thread is terminated externally. If successful, system error code is set to ERROR_SUCCESS. If the macro fails after finding the procedure, system error code is set to ERROR_PROC_NOT_FOUND.
#define SP_WAIT_FOR_HOOK_PROC_ATTACH(P, lib, ms) while(!(o##P=(t##P)GetProcAddress(GetModuleHandle(lib),#P))){Sleep(ms);}DetourTransactionBegin();DetourUpdateThread(GetCurrentThread());DetourAttach(&(PVOID&)o##P,hk##P);DetourTransactionCommit();if(o##P==NULL){SetLastError(ERROR_PROC_NOT_FOUND);}else{SetLastError(ERROR_SUCCESS);}

// Attempts to hook the function P in the library lib, retrying for a total of n attempts (waiting ms milliseconds between each attempt) if the procedure isn't found. If successful, system error code is set to ERROR_SUCCESS. If the macro fails, system error code is set to ERROR_PROC_NOT_FOUND.
#define SP_WAIT_N_FOR_HOOK_PROC_ATTACH(P, lib, retries, ms) {for(int i=0;i<retries&&!(o##P=(t##P)GetProcAddress(GetModuleHandle(lib),#P));i++){o##P=NULL;Sleep(ms);}}if(o##P!=NULL){DetourTransactionBegin();DetourUpdateThread(GetCurrentThread());DetourAttach(&(PVOID&)o##P,hk##P);DetourTransactionCommit();}if(o##P==NULL){SetLastError(ERROR_PROC_NOT_FOUND);}else{SetLastError(ERROR_SUCCESS);}



// Removes detour from hooked function P
#define SP_HOOK_DETACH(P) DetourTransactionBegin();DetourUpdateThread(GetCurrentThread());DetourDetach(&(PVOID&)o##P, hk##P);DetourTransactionCommit();

#else



#endif // _WIN32




#endif // _SP___HOOK___PROCEDURE___H_
