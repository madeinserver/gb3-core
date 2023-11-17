add_library(NiInput STATIC
    NiAction.cpp
    NiAction.h
    NiAction.inl
    NiActionData.h
    NiActionData.inl
    NiActionMap.cpp
    NiActionMap.h
    NiActionMap.inl
    NiInput.h
    NiInputDevice.cpp
    NiInputDevice.h
    NiInputDevice.inl
    NiInputErr.h
    NiInputGamePad.cpp
    NiInputGamePad.h
    NiInputGamePad.inl
    NiInputKeyboard.cpp
    NiInputKeyboard.h
    NiInputKeyboard.inl
    NiInputLibType.h
    NiInputMouse.cpp
    NiInputMouse.h
    NiInputMouse.inl
    NiInputPCH.h
    NiInputSystem.cpp
    NiInputSystem.h
    NiInputSystem.inl

    Win32/NiDI8Headers.h
    Win32/NiDI8InputSystem.cpp
    Win32/NiDI8InputSystem.h
    Win32/NiDI8InputSystem.inl
    Win32/NiInputDI8GamePad.cpp
    Win32/NiInputDI8GamePad.h
    Win32/NiInputDI8GamePad.inl
    Win32/NiInputDI8Keyboard.cpp
    Win32/NiInputDI8Keyboard.h
    Win32/NiInputDI8Keyboard.inl
    Win32/NiInputDI8Mouse.cpp
    Win32/NiInputDI8Mouse.h
    Win32/NiInputDI8Mouse.inl
    Win32/NiInputXInputGamePad.cpp
    Win32/NiInputXInputGamePad.h
    Win32/NiInput_D3DHeaders.h
    Win32/NiInput_DI8.h
)

target_link_libraries(NiInput PUBLIC NiDX9Renderer)
target_include_directories(NiInput PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Win32)
set_target_properties(NiInput PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/UtilityLibs")
target_precompile_headers(NiInput PRIVATE NiInputPCH.h)
