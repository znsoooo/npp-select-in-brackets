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
#include <vector>
#include <algorithm>

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

    ShortcutKey *sk1 = new ShortcutKey{false, true, false, 'Q'};
    setCommand(0, TEXT("Select in Brackets"), SelectInBrackets, sk1, false);

    ShortcutKey *sk2 = new ShortcutKey{false, true, false, 'X'};
    setCommand(1, TEXT("Swap Selections"), [](){SwapSelections(false);}, sk2, false);

    ShortcutKey *sk3 = new ShortcutKey{false, true, true, 'X'};
    setCommand(2, TEXT("Anti-Swap Selections"), [](){SwapSelections(true);}, sk3, false);
}


//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
    // Don't forget to deallocate your shortcut here
    for (int i = 0; i < nbFunc; i++) {
        delete funcItem[i]._pShKey;
    }
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//

int FindMatchingBracket(const char* str, int length, int sel_start, int sel_end, int& res_start, int& res_end);

void MyMessageBox(TCHAR* fmt, ...)
{
    va_list args;
    TCHAR msg[1000];

    va_start(args, fmt);
    vsnwprintf(msg, sizeof(msg), fmt, args);
    va_end(args);
    ::MessageBox(nppData._nppHandle, msg, TEXT("Message"), MB_OK);
}

HWND GetScintilla()
{
    // Get the current scintilla
    int which = -1;
    SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return 0;
    HWND hwnd_scin = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
    return hwnd_scin;
}

auto GetSelections(HWND hwnd_scin)
{
    std::vector<std::vector<int>> selections;
    int count = SendMessage(hwnd_scin, SCI_GETSELECTIONS, 0, 0);
    for (int i = 0; i < count; i++) {
        int start = SendMessage(hwnd_scin, SCI_GETSELECTIONNSTART, i, 0);
        int end = SendMessage(hwnd_scin, SCI_GETSELECTIONNEND, i, 0);
        selections.push_back(std::vector<int>{start, end});
    }
    return selections;
}

void SelectInBrackets()
{
    // Get the current scintilla
    HWND hwnd_scin = GetScintilla();

    // Get scintilla information
    int length = SendMessage(hwnd_scin, SCI_GETTEXTLENGTH, 0, 0);
    char* text = new char[length + 1];
    SendMessage(hwnd_scin, SCI_GETTEXT, length + 1, (LPARAM)text);

    // Get all selections
    auto sels = GetSelections(hwnd_scin);

    // Find span and set selections
    int sel_start, sel_end;
    for (int i = 0; i < sels.size(); i++) {
        if (FindMatchingBracket(text, length, sels[i][0], sels[i][1], sel_start, sel_end)) {
            SendMessage(hwnd_scin, i ? SCI_ADDSELECTION : SCI_SETSELECTION, sel_end, sel_start);
        }
    }

    // Clean up
    delete[] text;
}

void SwapSelections(bool anti)
{
    // Get the current scintilla
    HWND hwnd_scin = GetScintilla();

    // Get all selections
    auto sels = GetSelections(hwnd_scin);
    if (sels.size() < 2)
        return;

    // Swap selections
    std::sort(sels.begin(), sels.end(), [](const auto& a, const auto& b) { return a[0] < b[0]; });
    SendMessage(hwnd_scin, SCI_BEGINUNDOACTION, 0, 0);
    for (int i = anti ? 0 : sels.size() - 2; anti ? (i < sels.size() - 1) : (i >= 0); anti ? i++ : i--) {
        int j = i + 1;
        int diff = (sels[j][1] - sels[j][0]) - (sels[i][1] - sels[i][0]);
        SendMessage(hwnd_scin, SCI_SETSELECTION, sels[i][0], sels[i][1]);
        SendMessage(hwnd_scin, SCI_COPY, 0, 0);
        SendMessage(hwnd_scin, SCI_SETSELECTION, sels[j][1], sels[j][1]);
        SendMessage(hwnd_scin, SCI_PASTE, 0, 0);
        SendMessage(hwnd_scin, SCI_SETSELECTION, sels[j][1], sels[j][0]);
        SendMessage(hwnd_scin, SCI_CUT, 0, 0);
        SendMessage(hwnd_scin, SCI_SETSELECTION, sels[i][0], sels[i][1]);
        SendMessage(hwnd_scin, SCI_PASTE, 0, 0);
        sels[i][1] += diff;
        sels[j][0] += diff;
    }
    SendMessage(hwnd_scin, SCI_ENDUNDOACTION, 0, 0);

    // Restore selections
    for (int i = 0; i < sels.size(); i++) {
        SendMessage(hwnd_scin, i ? SCI_ADDSELECTION : SCI_SETSELECTION, sels[i][0], sels[i][1]);
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
