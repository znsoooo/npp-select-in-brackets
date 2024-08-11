//this file is part of notepad++
//Copyright (C)2022 Don HO <don.h@free.fr>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"

//
// put the headers you need here
//
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <shlwapi.h>

const TCHAR sectionName[] = TEXT("Insert Extesion");
const TCHAR keyName[] = TEXT("doCloseTag");
const TCHAR configFileName[] = TEXT("pluginDemo.ini");

#ifdef UNICODE 
	#define generic_itoa _itow
#else
	#define generic_itoa itoa
#endif

FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;


TCHAR iniFilePath[MAX_PATH];
bool doCloseTag = false;

#define DOCKABLE_DEMO_INDEX 15

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
	// Initialize dockable demo dialog
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
	::WritePrivateProfileString(sectionName, keyName, doCloseTag?TEXT("1"):TEXT("0"), iniFilePath);
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
	//
	// Firstly we get the parameters from your plugin config file (if any)
	//

	// get path of plugin configuration
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)iniFilePath);

	// if config path doesn't exist, we create it
	if (PathFileExists(iniFilePath) == FALSE)
	{
		::CreateDirectory(iniFilePath, NULL);
	}

	// make your plugin config file full file path name
	PathAppend(iniFilePath, configFileName);

	// get the parameter value from plugin config
	doCloseTag = (::GetPrivateProfileInt(sectionName, keyName, 0, iniFilePath) != 0);


    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );

    ShortcutKey *shKey = new ShortcutKey;
    shKey->_isAlt = true;
    shKey->_isCtrl = false;
    shKey->_isShift = false;
    shKey->_key = 0x51; //VK_Q
    setCommand(0, TEXT("Hello Notepad++"), hello, shKey, false);

}


//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
    // Don't forget to deallocate your shortcut here
    delete funcItem[0]._pShKey;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//

int FindMatchingBracket(const char* str, int pos, int* ptr_start, int* ptr_end);

void MyMessageBox(TCHAR* fmt, ...)
{
    va_list args;
    TCHAR msg[1000];

    va_start(args, fmt);
    vsnwprintf(msg, sizeof(msg), fmt, args);
    va_end(args);
    ::MessageBox(nppData._nppHandle, msg, TEXT("Message"), MB_OK);
}

void hello()
{
    // Get the current scintilla
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return;
    HWND hwnd_scin = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

    // Get scintilla infomation
    int pos = ::SendMessage(hwnd_scin, SCI_GETCURRENTPOS, 0, 0);
    int length = (int)::SendMessage(hwnd_scin, SCI_GETTEXTLENGTH, 0, 0);
    char text[length + 1];
    ::SendMessage(hwnd_scin, SCI_GETTEXT, length + 1, (LPARAM)text);

    // Find span and set selection
    int sel_start, sel_end;
    if (FindMatchingBracket(text, pos, &sel_start, &sel_end)) {
        ::SendMessage(hwnd_scin, SCI_SETSEL, sel_start, MAKELONG(sel_end, 0));
    }
}

//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}
