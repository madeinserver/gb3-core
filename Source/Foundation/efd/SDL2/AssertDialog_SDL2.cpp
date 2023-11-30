// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 2022 Arves100/Made In Server Developers.
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

//#include "efdPCH.h"

#include <efd/AssertDialog.h>
#include <efd/Helpers.h>
#include <efd/utf8string.h>

#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_messagebox.h>

using namespace efd;

#define SDL_IDYES 1
#define SDL_IDNO 2
#define SDL_IDCANCEL 3

//--------------------------------------------------------------------------------------------------
// DT32303 If you really want to be proper, you would spawn a thread and display the dialog in the
// spawned thread while this thread blocked waiting for completion.  In windows land creating a
// dialog has side effects for the current thread which could be harmful in some situations.
SInt8 efd::DisplayAssertDialog(
    const char* pFile,
    efd::SInt32 line,
    const char* pFunction,
    const char* pPred,
    const char* pMsg,
    const char* pStack,
    efd::Bool isAVerify)
{
    const char* pszTitle = isAVerify ? "Verify Failed" : "Assert Failed";

    utf8string buffer;
    buffer.reserve(1024);

    if (pPred && pPred != pMsg)
    {
        buffer.append("Predicate: ");
        buffer.append(pPred);
        buffer.append("\n");
    }
    buffer.append("Message: ");
    buffer.append(pMsg);

    if (pFile)
    {
        buffer.sprintf_append("\nLocation: %s(%d)\n", pFile, line);

        if (pFunction)
        {
            buffer.sprintf_append("\nFunction: %s\n", pFunction);
        }
    }

    if (pStack)
    {
        buffer.append("\nStackTrace:\n");
        buffer.append(pStack);
        buffer.append("\n");
    }

    if (isAVerify)
    {
        buffer.append("\nYes to debug, No to ignore once.");
    }
    else
    {
        buffer.append("\nYes to debug, No to ignore once, Cancel to ignore always.");
    }

    bool cursorShowing = efd::IsCursorVisible();

    efd:ShowCursor();

    // Display the message box
    SDL_MessageBoxData data;
    data.title = pszTitle;
    data.message = buffer.c_str();
    data.colorScheme = NULL;
    data.window = NULL;
    data.flags = SDL_MESSAGEBOX_INFORMATION;

    SDL_MessageBoxButtonData buttons[3];
    memset(buttons, 0, sizeof(buttons));

    buttons[0].buttonid = SDL_IDYES;
    buttons[0].text = "Yes";
    buttons[1].buttonid = SDL_IDNO;
    buttons[1].text = "No";
    buttons[2].buttonid = SDL_IDCANCEL;
    buttons[2].text = "Cancel";

    data.buttons = buttons;

    // Don't show the cancel button (ignore for the rest of the execution) option if it is a
    // verify message since that does not work in that case
    data.numbuttons = isAVerify ? 2 : 3;

    int msgboxID;
    SDL_ShowMessageBox(&data, &msgboxID);

    efd::SetCursorVisibility(cursorShowing);

    // Check the return value and let the helpers know how to handle the assert
    switch (msgboxID)
    {
    default:
    case SDL_IDYES:
        return AssertHelper::kDebugAbort;
    case SDL_IDNO:
        return AssertHelper::kIgnoreOnce;
    case SDL_IDCANCEL:
        return AssertHelper::kIgnore;
    }
}

//--------------------------------------------------------------------------------------------------
