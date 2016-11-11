// DialParserPrefixHash.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "nclog.h"
#include <astdtapi.h>
#pragma comment (lib, "cellcore.lib")

LONG DialNumber(LPCTSTR lpszPhoneNum)
{
	LONG lResult;
	lResult = tapiRequestMakeCall(lpszPhoneNum, NULL, NULL, NULL);
	return lResult;
}

// Dial Parser Extensibility response recommendations
#define DPEF_ALLOW_DIAL_REQUEST 0x01
#define DPEF_ABORT_DIAL_REQUEST 0x02
#define DPEF_REQUEST_NOT_SECURE 0x04

/****************************************************************************

   FUNCTION: IsImmediateDialRequestRequired

   SIGNATURE:
      BOOL IsImmediateDialRequestRequired(LPCTSTR lpszDialString);

   PURPOSE:
      Give Dial Parser Extensibility a chance to decide whether
      the framework should regard the digits collected so far as
      an actionable dial string.

   PARAMETERS:
      lpszDialString - The digits collected so far

   RETURNS:
      TRUE if lpszDialString is a complete dial request, meaning
           that no additional digits need to be collected and
           the Talk button does not need to be pressed in order
           for a meaningful dial action to take place
      FALSE otherwise

****************************************************************************/
BOOL IsImmediateDialRequestRequired(LPCTSTR lpszDialString){
	nclog(L"IsImmediateDialRequestRequired: DialString: '%s'\n", lpszDialString);

	return FALSE;
}

/****************************************************************************

   FUNCTION: PreprocessDialRequest

   SIGNATURE:
      DWORD PreprocessDialRequest(HWND hWnd, LPCTSTR lpszDialString, BOOL fSecure);

   PURPOSE:
      Give Dial Parser Extensibility a chance to intercept the
      Dial Request.  If the DLL chooses not to, the framework
      will send the Dial Request to RIL.

      The implementor can take whatever action is appropriate,
      including nothing at all, before returning.  The implementor
      may wish to use hWnd to display a MessageBox, or may wish to
      use tapiRequestMakeCall to issue a call to a number other
      than the one that was entered, etc.

      Typically, if the DLL takes action on its own, it will
      return DPEF_ABORT_DIAL_REQUEST, and if it does not take
      action on its own, it will return DPEF_ALLOW_DIAL_REQUEST.
      However, this is a recommendation, not a requirement.

   PARAMETERS:
      hWnd           - A window handle, typically used as a
                       parent window for MessageBoxes
      lpszDialString - The digits collected so far
      fSecure        - Whether the string represents a secure
                       call request

   RETURNS:
      DPEF_ALLOW_DIAL_REQUEST - the framework should do whatever
                                it would normally do
      DPEF_ABORT_DIAL_REQUEST - the framework should assume that
                                the dial request has been handled
                                and stop processing it further

****************************************************************************/

DWORD PreprocessDialRequest(HWND hWnd, LPCTSTR lpszDialString, BOOL fSecure)
{
	DWORD dwRet=DPEF_ALLOW_DIAL_REQUEST;
	// gives 0xb7ccc68a: PreprocessDialRequest: DialString: '021153601752', secure: true
	nclog(L"PreprocessDialRequest: DialString: '%s', secure: %s\n",
		lpszDialString, fSecure?L"true":L"false");

	if(wcscmp(L"021153601", lpszDialString)==0){
		nclog(L"dialstring matched\n");
		LONG lRes =DialNumber(L"021153601752");
		if (lRes==0){
			nclog(L"dialed successfully\n");
			dwRet=DPEF_ABORT_DIAL_REQUEST;
		}
		else
			nclog(L"dial failed: %i\n", lRes);
	}
	return dwRet;
}

/****************************************************************************

   FUNCTION: PHExtFormatPhoneNumber

   SIGNATURE: BOOL PHExtFormatPhoneNumber(
                   LPCTSTR lpszCurrentCountryCode,
                   LPCTSTR lpszInput,
                   LPTSTR  lpszOut,
                   DWORD   cchOut);

   PURPOSE: Depending on where a device is primarily used the OEM may choose
            to customize the phone number formatting for a particular region. 
            This function will be called each time the user adds a new number
            to the dial string. The OEM can choose to handle the dial string
            or choose to allow Windows Mobile to proceed with default formatting.
            NOTE: As this function is called each time the user adds or removes
            a number from the dial string the OEM needs to take care to make sure
            this function is performant. 

   PARAMETERS:
            lpszCurrentCountryCode  - The Country Code for the current locale
            lpszInput               - The raw dial string
            lpszOut                 - A buffer to place the formatted dial string
            cchOut                  - The size of the buffer (in chars) pointed to by lpszOut

   RETURNS: If the function returns TRUE the OEM is assumed to have handled
            the phone number formatting and no additional formatting will be
            performed. If the function returns FALSE Windows Mobile will proceed
            with normal phone number formatting.

***************************************************************************/


BOOL PHExtFormatPhoneNumber(LPCTSTR lpszCurrentCountryCode, LPCTSTR lpszInput, LPTSTR lpszOut, DWORD cchOut){
	BOOL bRet=FALSE; //no change to number done
	nclog(L"PHExtFormatPhoneNumber: country code:'%s', input: '%s'\n",
		lpszCurrentCountryCode, lpszInput);
	
	if(&lpszInput[0]==L"#"){
	}

	nclog(L"PHExtFormatPhoneNumber...return with %s\n", bRet?L"true":L"false");
	return bRet;
}

BOOL APIENTRY DllMain( HANDLE hinstDLL, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	nclog_LogginEnabled=TRUE;
	switch(ul_reason_for_call){
		case DLL_PROCESS_ATTACH:
			g_hinstDLL = hinstDLL;
			nclog(L"process attached: %s\n", lpReserved);
			break;
		case DLL_PROCESS_DETACH:
			nclog(L"process detached: %s\n", lpReserved);
			break;
		case DLL_THREAD_ATTACH:
			nclog(L"thread attached: %s\n", lpReserved);
			break;
		case DLL_THREAD_DETACH:
			nclog(L"thread detached: %s\n", lpReserved);
			break;
		default:
			break;
	}
    return TRUE;
}

