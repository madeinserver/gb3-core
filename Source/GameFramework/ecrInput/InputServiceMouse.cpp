#include "ecrInputPCH.h"
#include "InputServiceMouse.h"

using namespace ecrInput;

//-------------------------------------------------------------------------------------------------
InputServiceMouse::InputServiceMouse() :
    m_pFilter(NULL)
{

}
//-------------------------------------------------------------------------------------------------
InputServiceMouse::InputServiceMouse(
        efd::UInt32 flags,
        efd::UInt32 appData,
        efd::UInt32 deviceID,
        efd::UInt32 modifiers,
        InputService::InputFilteringCallback pFilter) :
    ecrInput::InputServiceActionBase(flags, appData, deviceID, modifiers),
    m_pFilter(pFilter)
{

}
//-------------------------------------------------------------------------------------------------
InputServiceMouse::~InputServiceMouse()
{
}
//-------------------------------------------------------------------------------------------------
efd::Bool InputServiceMouse::ProcessInput(
    InputService* pInputService,
    efd::UInt32& appData,
    efd::Float32& magnitude,
    efd::Float32& x,
    efd::Float32& y,
    efd::Float32& z)
{
    EE_ASSERT(pInputService);
    NiInputMouse* pMouse = pInputService->GetInputSystem()->GetMouse();
    if (!pMouse)
        return false;

    int xDelta, yDelta, zDelta;
    efd::Bool changed = false;
    if (pMouse->GetPositionDelta(xDelta, yDelta, zDelta))
    {
        changed = (xDelta != 0 || yDelta != 0 || zDelta != 0);
        if (changed)
        {
            // Convert from integer to a reasonably-scaled float.
            x = ConvertAxisValue(xDelta);
            y = ConvertAxisValue(yDelta);
            z = ConvertAxisValue(zDelta);
        }
    }

    if (m_pFilter)
        m_pFilter(m_appData, x, y, z);

    efd::Bool send = (m_flags & InputService::CONTINUOUS) != 0;
    send |= changed;

    // This service is defined as returning a magnitude of 0, but it might also
    // reasonably give the magnitude of the XY vector.
    magnitude = 0.0f;

    if (send)
    {
        appData = m_appData;

        if (m_flags & InputService::REVERSE_X_AXIS)
            x = -x;

        if (m_flags & InputService::REVERSE_Y_AXIS)
            y = -y;

        if (m_flags & InputService::REVERSE_Z_AXIS)
            z = -z;
    }

    return send;
}
//-------------------------------------------------------------------------------------------------
/// Save action data to XML.
efd::Bool InputServiceMouse::SaveXml(efd::TiXmlElement* pElement)
{
    InputServiceActionBase::SaveXml(pElement);
    pElement->SetAttribute("ActionClsID", "MOUSE");

    return true;
}
//-------------------------------------------------------------------------------------------------
/// Load action data from XML.
efd::Bool InputServiceMouse::LoadXml(efd::TiXmlElement* pElement)
{
    InputServiceActionBase::LoadXml(pElement);
    // no parameters - nothing to save
    return true;
}




