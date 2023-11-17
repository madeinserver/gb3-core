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

#pragma once
#ifndef NSBUSERDEFINEDDATASET_H
#define NSBUSERDEFINEDDATASET_H

#include "NSBUserDefinedDataBlock.h"

// NSBUserDefinedDataSet is a container for a collection of
// NSBUserDefinedDataBlock instances. This class exists to minimize
// code duplication as NSBShader, NSBImplementation, and NSBPass may
// all have a collection of NSBUserDefinedDataBlock instances.
//
class NSBSHADERLIB_ENTRY NSBUserDefinedDataSet : public NiRefObject
{
public:
    NSBUserDefinedDataSet();
    ~NSBUserDefinedDataSet();

    // The user-defined data
    unsigned int GetBlockCount();
    NSBUserDefinedDataBlock* GetBlock(const char* pcName,
        bool bCreate = false);
    NSBUserDefinedDataBlock* GetFirstBlock(NiTListIterator& kIter);
    NSBUserDefinedDataBlock* GetNextBlock(NiTListIterator& kIter);

    // *** begin Emergent internal use only
    virtual bool SaveBinary(efd::BinaryStream& kStream);
    virtual bool LoadBinary(efd::BinaryStream& kStream);

#if defined(NIDEBUG)
    virtual void Dump(FILE* pf);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    NiTPointerList<NSBUserDefinedDataBlock*> m_kUserDefinedDataBlocks;
};

NiSmartPointer(NSBUserDefinedDataSet);

#endif  //NSBUSERDEFINEDDATASET_H
