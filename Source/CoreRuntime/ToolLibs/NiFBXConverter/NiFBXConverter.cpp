// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#include "NiFBXConverter.h"
#include "NiFBXExporter.h"

//-----------------------------------------------------------------------------------------------
bool NiFBXConverter::ExportToFBX(const char* pcFilename, NiTPrimitiveArray<NiAVObject*>& kScenes,
        NiTObjectArray<NiString>& kNames)
{
    NiFBXExporter kExporter;
    for (NiUInt32 ui = 0; ui < kScenes.GetAllocatedSize(); ui++)
    {
        if (kScenes.GetAt(ui) == NULL)
            continue;

        kExporter.AddSceneGraph(kScenes.GetAt(ui), kNames.GetAt(ui));
    }

    return kExporter.SaveScene(pcFilename);
}
//-----------------------------------------------------------------------------------------------
