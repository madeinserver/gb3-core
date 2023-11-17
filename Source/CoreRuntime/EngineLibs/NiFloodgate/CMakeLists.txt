set(NIFLOODGATE_SRC
    NiFloodgateLibType.h
    NiFloodgatePCH.h
    NiFloodgateSDM.cpp
    NiFloodgateSDM.h
    NiSPAlgorithms.h
    NiSPAlgorithms.inl
    NiSPAnalyzer.cpp
    NiSPAnalyzer.h
    NiSPDataStream.h
    NiSPDataStream.inl
    NiSPKernel.h
    NiSPStream.cpp
    NiSPStream.h
    NiSPStream.inl
    NiSPTask.cpp
    NiSPTask.h
    NiSPTask.inl
    NiSPThreadPool.h
    NiSPThreadPool.inl
    NiSPWorkflow.cpp
    NiSPWorkflow.h
    NiSPWorkflow.inl
    NiSPWorkflowManager.cpp
    NiSPWorkflowManager.h
    NiSPWorkflowManager.inl
    NiStreamProcessor.cpp
    NiStreamProcessor.h
    NiStreamProcessor.inl

    Win32/NiSPJob.cpp
    Win32/NiSPJob.h
    Win32/NiSPJob.inl
    Win32/NiSPJobList.cpp
    Win32/NiSPJobList.h
    Win32/NiSPJobList.inl
    Win32/NiSPKernelMacros.h
    Win32/NiSPStreamPartitioner.cpp
    Win32/NiSPStreamPartitioner.h
    Win32/NiSPStreamPartitioner.inl
    Win32/NiSPTaskImpl.cpp
    Win32/NiSPTaskImpl.h
    Win32/NiSPTaskImpl.inl
    Win32/NiSPThreadPool.cpp
    Win32/NiSPWorkflowImpl.cpp
    Win32/NiSPWorkflowImpl.h
    Win32/NiSPWorkload.h
    Win32/NiSPWorkload.inl
    Win32/NiStreamProcessor_Win32.cpp

    Kernels/NiNopKernel/NiNopKernel.cpp
    Kernels/NiNopKernel/NiNopKernel.h
)

add_library(NiFloodgate STATIC ${NIFLOODGATE_SRC})
target_link_libraries(NiFloodgate PUBLIC NiMain)
target_include_directories(NiFloodgate PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/Win32 ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiNopKernel)
set_target_properties(NiFloodgate PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/EngineLibs")
target_precompile_headers(NiFloodgate PRIVATE NiFloodgatePCH.h)

if (GB3_BUILD_TOOLS)
    add_library(NiFloodgateDLL SHARED ${NIFLOODGATE_SRC})
    target_link_libraries(NiFloodgateDLL PUBLIC NiMainDLL)
    target_include_directories(NiFloodgateDLL PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/Win32 ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiNopKernel)
    set_target_properties(NiFloodgateDLL PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/EngineLibs")
    target_precompile_headers(NiFloodgateDLL PRIVATE NiFloodgatePCH.h)
    target_compile_definitions(NiMeshDLL PRIVATE -DNIFLOODGATE_EXPORT -D_USRDLL INTERFACE -DNIFLOODGATE_IMPORT)
endif()
