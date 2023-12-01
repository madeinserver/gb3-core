set(NISYSTEM_SRC
    NiAllocator.h
    NiBinaryLoadSave.h
    NiBinaryStream.h
    NiBinaryStreamMacro.h
    NiCriticalSection.h
    NiDebug.cpp
    NiDebug.h
    NiDebugDefines.h
    NiEndian.h
    NiEnumerationStrings.h
    NiExternalMemAllocator.h
    NiFile.h
    NiFilename.h
    NiFixedAllocator.h
    NiFloat16.h
    NiInitOptions.cpp
    NiInitOptions.h
    NiLog.cpp
    NiLog.h
    NiLog.inl
    NiLogBehavior.cpp
    NiLogBehavior.h
    NiLoopedThread.h
    NiLoopedThreadProcedure.h
    NiMemHint.h
    NiMemManager.h
    NiMemObject.h
    NiMemoryDefines.h
    NiMemStream.cpp
    NiMemStream.h
    NiMemTracker.h
    NiMessageBox.h
    NiMessageBox.inl
    NiOS.h
    NiPath.h
    NiProcessorAffinity.h
    NiRTLib.h
    NiRWLock.h
    NiSDMMacros.h
    NiSearchPath.cpp
    NiSearchPath.h
    NiSearchPath.inl
    NiSemaphore.h
    NiStandardAllocator.h
    NiStaticDataManager.cpp
    NiStaticDataManager.h
    NiSystem.h
    NiSystemDesc.h
    NiSystemLibType.h
    NiSystemPCH.h
    NiSystemSDM.cpp
    NiSystemSDM.h
    NiTCreate.h
    NiThread.h
    NiThreadProcedure.h
    NiTSmallAllocStrategy.h
    NiTThreadLocal.h
    NiUniversalTypes.h
    NiVersion.h
)

set(SDL2_SRC
    SDL2/NiFilename_SDL2.cpp
    SDL2/NiMessageBox_SDL2.cpp
    SDL2/NiSystem_SDL2.inl
    SDL2/NiRWLock_SDL2.inl
)

source_group("SDL2" FILES ${SDL2_SRC})

add_library(NiSystem STATIC ${NISYSTEM_SRC} ${SDL2_SRC})
target_link_libraries(NiSystem PUBLIC efd)
target_precompile_headers(NiSystem PRIVATE NiSystemPCH.h)
target_include_directories(NiSystem PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/..)
set_target_properties(NiSystem PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/EngineLibs")
target_compile_definitions(NiSystem PUBLIC 
    $<$<CONFIG:DEBUG>:NIDEBUG>;
    $<$<CONFIG:RELEASE>:NIRELEASE>;
    $<$<CONFIG:SHIPPING>:NISHIPPING>;

    NI_USE_PCH
)

if (GB3_BUILD_TOOLS)
    add_library(NiSystemDLL SHARED ${NISYSTEM_SRC})
    target_link_libraries(NiSystemDLL PUBLIC efdDLL)
    target_precompile_headers(NiSystemDLL PRIVATE NiSystemPCH.h)
    target_include_directories(NiSystemDLL PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/..)
    set_target_properties(NiSystemDLL PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/EngineLibs")
    target_compile_definitions(NiSystemDLL PUBLIC 
        $<$<CONFIG:DEBUG>:NIDEBUG>;
        $<$<CONFIG:RELEASE>:NIRELEASE>;
        $<$<CONFIG:SHIPPING>:NISHIPPING>;
        NI_USE_PCH
        PRIVATE -DNISYSTEM_EXPORT -D_USRDLL
        INTERFACE -DNISYSTEM_IMPORT
    )
endif()
