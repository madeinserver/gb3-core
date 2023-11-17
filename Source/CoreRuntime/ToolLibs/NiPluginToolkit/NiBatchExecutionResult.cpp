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

#include "NiBatchExecutionResult.h"

NiImplementRTTI(NiBatchExecutionResult, NiExecutionResult);

//--------------------------------------------------------------------------------------------------
NiBatchExecutionResult::NiBatchExecutionResult(ReturnCode eReturnCode) :
    NiExecutionResult(eReturnCode)
{
}

//--------------------------------------------------------------------------------------------------
bool NiBatchExecutionResult::WasSuccessful()
{
    for (unsigned int ui = 0; ui < m_pkPluginResults.GetSize(); ui++)
    {
        NiExecutionResult* pkPluginResult = m_pkPluginResults.GetAt(ui);
        if (pkPluginResult && pkPluginResult->m_eReturnCode !=
            NiExecutionResult::EXECUTE_SUCCESS)
            return false;
    }
    return true;
}

//--------------------------------------------------------------------------------------------------
