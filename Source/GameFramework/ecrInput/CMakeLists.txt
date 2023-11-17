set(SRC
    ControlBuiltins.cpp
    ControlBuiltins.h
    ecrInput.i
    ecrInputLibType.h
    ecrInputMessageIDs.h
    ecrInputPCH.h
    InputActionMapsLoaded.cpp
    InputActionMapsLoaded.h
    InputActionMessage.cpp
    InputActionMessage.h
    InputActionMessage.inl
    InputHandler.cpp
    InputHandler.h
    InputService.cpp
    InputService.h
    InputService.inl
    InputServiceAction.cpp
    InputServiceAction.h
    InputServiceAction.inl
    InputServiceActionBase.cpp
    InputServiceActionBase.h
    InputServiceActionBase.inl
    InputServiceActionMap.cpp
    InputServiceActionMap.h
    InputServiceActionMap.inl
    InputServiceDPad.cpp
    InputServiceDPad.h
    InputServiceDPad.inl
    InputServiceEvent.cpp
    InputServiceEvent.h
    InputServiceEvent.inl
    InputServiceMouse.cpp
    InputServiceMouse.h
    InputServiceStick.cpp
    InputServiceStick.h
    InputServiceStick.inl
    InputStickMessage.cpp
    InputStickMessage.h
    InputStickMessage.inl
    KeyboardMessages.cpp
    KeyboardMessages.h
    KeyboardMessages.inl
    MouseMessages.cpp
    MouseMessages.h
    MouseMessages.inl
    StandardModelLibraryBehaviorIDs.h
    StandardModelLibraryFlatModelIDs.h
    StandardModelLibraryPropertyIDs.h
)

set(SWIG_SRC
    ecrInput.i
)

add_library(ecrInput STATIC ${SRC})
set_target_properties(ecrInput PROPERTIES FOLDER "${GB3_PREFIX}GameFramework")
target_precompile_headers(ecrInput PRIVATE ecrInputPCH.h)
target_link_libraries(ecrInput PUBLIC ecr)
SwigTarget(ecrInput ${SWIG_SRC})
