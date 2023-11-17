add_library(NiUserInterface STATIC
    NiNavBaseController.cpp
    NiNavBaseController.h
    NiNavBaseController.inl
    NiNavFlyController.cpp
    NiNavFlyController.h
    NiNavFlyController.inl
    NiNavInterpToController.cpp
    NiNavInterpToController.h
    NiNavManager.cpp
    NiNavManager.h
    NiNavManager.inl
    NiNavOrbitController.cpp
    NiNavOrbitController.h
    NiNavOrbitController.inl
    NiUIAtlasMap.cpp
    NiUIAtlasMap.h
    NiUIAtlasMap.inl
    NiUIBaseElement.cpp
    NiUIBaseElement.h
    NiUIBaseElement.inl
    NiUIButton.cpp
    NiUIButton.h
    NiUIButton.inl
    NiUICheckBox.cpp
    NiUICheckBox.h
    NiUICheckBox.inl
    NiUIGroup.cpp
    NiUIGroup.h
    NiUIGroup.inl
    NiUIHotkeyImage.cpp
    NiUIHotkeyImage.h
    NiUIHotkeyImage.inl
    NiUIImage.cpp
    NiUIImage.h
    NiUIImage.inl
    NiUIImageButton.cpp
    NiUIImageButton.h
    NiUIImageButton.inl
    NiUILabel.cpp
    NiUILabel.h
    NiUILabel.inl
    NiUIManager.cpp
    NiUIManager.h
    NiUIManager.inl
    NiUIManagerRenderClick.cpp
    NiUIManagerRenderClick.h
    NiUIRenderGroup.cpp
    NiUIRenderGroup.h
    NiUIRenderGroup.inl
    NiUISignal.h
    NiUISignal.inl
    NiUISignalSlotMacros.h
    NiUISlider.cpp
    NiUISlider.h
    NiUISlider.inl
    NiUISlot.h
    NiUISlot.inl
    NiUserInterfaceLibType.h
    NiUserInterfacePCH.h
)

target_link_libraries(NiUserInterface PUBLIC NiFont NiInput NiCursor)
target_include_directories(NiUserInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiUserInterface PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/UtilityLibs")
target_precompile_headers(NiUserInterface PRIVATE NiUserInterfacePCH.h)
