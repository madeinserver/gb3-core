// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net
//--------------------------------------------------------------------------------------------------
// Precompiled Header
#include "NSFParserLibPCH.h"

#include "NSFLoader.h"

//--------------------------------------------------------------------------------------------------
unsigned int NSFLoader::LoadAllNSFFilesInDirectory(
    const char* pcDirectory, const char* pcExt, bool bRecurseDirectories,
    NiTPointerList<char*>* pkFileList)
{
    if (!pcDirectory || (strcmp(pcDirectory, "") == 0))
        return 0;
    if (!pcExt || (strcmp(pcExt, "") == 0))
        return 0;

    unsigned int uiCount    = 0;
    char acFilePath[NI_MAX_PATH];

    NiStrncpy(acFilePath, NI_MAX_PATH, pcDirectory, NI_MAX_PATH - 1);
    size_t stLen = strlen(acFilePath);
    if ((acFilePath[stLen - 1] != '\\') && (acFilePath[stLen - 1] != '/'))
    {
        acFilePath[stLen] = '\\';
        acFilePath[stLen + 1] = 0;
    }

    NiPath::Standardize(acFilePath);

#ifdef EE_PLATFORM_WIN32
    WIN32_FIND_DATA wfd ;
    HANDLE hFile = NULL;
    DWORD dwAttrib;
#else
    DIR* dir = NULL;
    struct dirent* rd = NULL;
    bool isDir = false;
    struct stat lst;
#endif
    char acFileName[_MAX_PATH];
    char acFileName2[_MAX_PATH];
    bool bDone = false;

#ifdef EE_PLATFORM_WIN32
    memset(&wfd, 0, sizeof(WIN32_FIND_DATA));
#endif

    NiStrcpy(acFileName, _MAX_PATH, pcDirectory);
    stLen = strlen(acFileName);
    if ((acFileName[stLen - 1] != '\\') && (acFileName[stLen - 1] != '/'))
    {
        acFileName[stLen] = '\\';
        acFileName[stLen + 1] = 0;
    }
    // This will cover the case when the directory is a mapped network
    // drive...
    NiStrcat(acFileName, _MAX_PATH, "*");

    NiPath::Standardize(acFileName);

#ifdef EE_PLATFORM_WIN32
    hFile = FindFirstFile(acFileName, &wfd);
#else
    dir = opendir(acFileName);
    rd = readdir(dir);
#endif

#ifdef EE_PLATFORM_WIN32
    if (INVALID_HANDLE_VALUE != hFile)
#else
    if (dir)
#endif
    {

        NiStrcpy(acFileName2, NI_MAX_PATH, acFilePath);
#ifdef EE_PLATFORM_WIN32
        NiStrcat(acFileName2, NI_MAX_PATH, wfd.cFileName);
#else
        NiStrcat(acFileName2, NI_MAX_PATH, rd.d_name);
#endif

        NiPath::Standardize(acFileName2);
        while (!bDone)
        {
            NiStrcpy(acFileName2, NI_MAX_PATH, acFilePath);

#ifdef EE_PLATFORM_WIN32
            NiStrcat(acFileName2, NI_MAX_PATH, wfd.cFileName);
            if (hFile == INVALID_HANDLE_VALUE)
#else
            NiStrcat(acFileName2, NI_MAX_PATH, rd.d_name);
            if (!rd)
#endif
            {
                NILOG(NIMESSAGE_GENERAL_0,
                    "Invalid handle on FindXXXXXFile\n");
                bDone = true;
            }
            else
            {
#ifdef EE_PLATFORM_WIN32
                dwAttrib = GetFileAttributes(acFileName2);
                if ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
                {
                    if (strcmp(wfd.cFileName, ".") != 0 && strcmp(wfd.cFileName, "..") != 0)
                    {
                        // If we are recursing... do it
                        if (bRecurseDirectories)
                        {
                            NiStrcat(acFileName2, NI_MAX_PATH, "\\");
                            NILOG(NIMESSAGE_GENERAL_0,
                                "    Recurse directory %s\n",
                                acFileName2);
                            uiCount += LoadAllNSFFilesInDirectory(
                                acFileName2, pcExt, bRecurseDirectories,
                                pkFileList);
                        }
                    }
                }
#else
                if (rd.d_type == DT_UNKNOWN)
                {
                    if (lstat(rd.d_name, &lst) == 0)
                    {
                        isDir = S_ISDIR(lst.st_mode);
                    }
                }
                else if (rd.d_type & DT_DIR)
                    isDir = true;

                if (isDir)
                {
                    if (strcmp(rd.d_name, ".") != 0 && strcmp(rd.d_name, "..") != 0)
                    {
                        // If we are recursing... do it
                        if (bRecurseDirectories)
                        {
                            NiStrcat(acFileName2, NI_MAX_PATH, "\\");
                            NILOG(NIMESSAGE_GENERAL_0,
                                "    Recurse directory %s\n",
                                acFileName2);
                            uiCount += LoadAllNSFFilesInDirectory(
                                acFileName2, pcExt, bRecurseDirectories,
                                pkFileList);
                        }
                    }
                }
#endif
                else
                {
                    if (ProcessNSFFile(acFileName2, pcExt, pkFileList))
                        uiCount++;
                }
            }

#ifdef EE_PLATFORM_WIN32
            if (FindNextFile(hFile, &wfd) == false)
                bDone = true;
#else
            rd = readdir(dir);
            if (!rd)
                bDone = true;
#endif
        }

#ifdef EE_PLATFORM_WIN32
        FindClose(hFile);
#else
        closedir(dir);
#endif
    }

    return uiCount;
}

//--------------------------------------------------------------------------------------------------
