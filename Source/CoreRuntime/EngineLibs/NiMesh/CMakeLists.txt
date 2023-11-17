set(NIMESH_SRC
    NiBatchedUpdateProcess.cpp
    NiBatchedUpdateProcess.h
    NiDataStream.cpp
    NiDataStream.h
    NiDataStream.inl
    NiDataStreamElement.cpp
    NiDataStreamElement.h
    NiDataStreamElement.inl
    NiDataStreamElementLock.cpp
    NiDataStreamElementLock.h
    NiDataStreamElementLock.inl
    NiDataStreamFactory.cpp
    NiDataStreamFactory.h
    NiDataStreamLock.h
    NiDataStreamLock.inl
    NiDataStreamLockPolicy.h
    NiDataStreamLockPolicy.inl
    NiDataStreamPrimitiveLock.cpp
    NiDataStreamPrimitiveLock.h
    NiDataStreamPrimitiveLock.inl
    NiDataStreamRef.cpp
    NiDataStreamRef.h
    NiDataStreamRef.inl
    NiDebugVisualizationClick.cpp
    NiDebugVisualizationClick.h
    NiGeometryConverter.cpp
    NiGeometryConverter.h
    NiGeometryConverter.inl
    NiImmediateModeAdapter.cpp
    NiImmediateModeAdapter.h
    NiImmediateModeAdapter.inl
    NiImmediateModeMacro.cpp
    NiImmediateModeMacro.h
    NiImmediateModeMacro.inl
    NiInstancingMeshModifier.cpp
    NiInstancingMeshModifier.h
    NiInstancingMeshModifier.inl
    NiInstancingUtilities.cpp
    NiInstancingUtilities.h
    NiInstancingUtilities.inl
    NiMatrix3x4.h
    NiMesh.cpp
    NiMesh.h
    NiMesh.inl
    NiMesh2DRenderView.cpp
    NiMesh2DRenderView.h
    NiMesh2DRenderView.inl
    NiMeshAlgorithms.h
    NiMeshAlgorithms.inl
    NiMeshCullingProcess.cpp
    NiMeshCullingProcess.h
    NiMeshErrorInterface.h
    NiMeshHWInstance.cpp
    NiMeshHWInstance.h
    NiMeshLib.h
    NiMeshLibType.h
    NiMeshMaterialBinding.cpp
    NiMeshMaterialBinding.h
    NiMeshMaterialBinding.inl
    NiMeshModifier.cpp
    NiMeshModifier.h
    NiMeshModifier.inl
    NiMeshPCH.h
    NiMeshRequirements.cpp
    NiMeshRequirements.h
    NiMeshRequirements.inl
    NiMeshScreenElements.cpp
    NiMeshScreenElements.h
    NiMeshScreenElements.inl
    NiMeshSDM.cpp
    NiMeshSDM.h
    NiMeshUpdateProcess.cpp
    NiMeshUpdateProcess.h
    NiMeshUpdateProcess.inl
    NiMeshUtilities.cpp
    NiMeshUtilities.h
    NiMeshUtilities.inl
    NiMeshVertexOperators.cpp
    NiMeshVertexOperators.h
    NiMeshVertexOperators.inl
    NiMorphingKernelData.h
    NiMorphMeshModifier.cpp
    NiMorphMeshModifier.h
    NiMorphMeshModifier.inl
    NiScreenFillingRenderViewImpl.cpp
    NiScreenFillingRenderViewImpl.h
    NiScreenFillingRenderViewImpl.inl
    NiSkinningKernel.h
    NiSkinningKernelData.h
    NiSkinningMeshModifier.cpp
    NiSkinningMeshModifier.h
    NiSkinningMeshModifier.inl
    NiTIndexGenerationIterator.h
    NiTIndexGenerationIterator.inl
    NiToolDataStream.cpp
    NiToolDataStream.h
    NiTPrimitiveRandomAccessIterator.h
    NiTPrimitiveRandomAccessIterator.inl
    NiTSimpleArray.h
    NiTSimpleArray.inl
    NiTStridedRandomAccessIterator.h
    NiTStridedRandomAccessIterator.inl

    Win32/NiMatrix3x4.inl

    Kernels/NiCalculateBoneMatricesKernel/NiCalculateBoneMatricesKernel.h
    Kernels/NiCalculateBoneMatricesKernel/NiCalculateBoneMatricesKernel.cpp
    Kernels/NiInstanceCullingKernel/NiInstanceCullingKernel.cpp
    Kernels/NiInstanceCullingKernel/NiInstanceCullingKernel.h
    Kernels/NiMorphingKernel/NiMorphingKernel.cpp
    Kernels/NiMorphingKernel/NiMorphingKernel.h
    Kernels/NiSkinningKernelP/NiSkinningKernelP.h
    Kernels/NiSkinningKernelP/NiSkinningKernelP.cpp
    Kernels/NiSkinningKernelPN16/NiSkinningKernelPN16.h
    Kernels/NiSkinningKernelPN16/NiSkinningKernelPN16.cpp
    Kernels/NiSkinningKernelPN32/NiSkinningKernelPN32.cpp
    Kernels/NiSkinningKernelPN32/NiSkinningKernelPN32.h
    Kernels/NiSkinningKernelPNBT16/NiSkinningKernelPNBT16.cpp
    Kernels/NiSkinningKernelPNBT16/NiSkinningKernelPNBT16.h
    Kernels/NiSkinningKernelPNBT32/NiSkinningKernelPNBT32.cpp
    Kernels/NiSkinningKernelPNBT32/NiSkinningKernelPNBT32.h
)

add_library(NiMesh STATIC ${NIMESH_SRC})
target_link_libraries(NiMesh PUBLIC NiFloodgate)
target_include_directories(NiMesh PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/Win32
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPNBT32
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPNBT16
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPN32
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPN16
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelP
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiMorphingKernel
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiInstanceCullingKernel
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiCalculateBoneMatricesKernel
)
set_target_properties(NiMesh PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/EngineLibs")
target_precompile_headers(NiMesh PRIVATE NiMeshPCH.h)

if (GB3_BUILD_TOOLS)
    add_library(NiMeshDLL SHARED ${NIMESH_SRC})
    target_link_libraries(NiMeshDLL PUBLIC NiFloodgateDLL)
    target_include_directories(NiMeshDLL PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/Win32
        ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPNBT32
        ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPNBT16
        ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPN32
        ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPN16
        ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelP
        ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiMorphingKernel
        ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiInstanceCullingKernel
        ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiCalculateBoneMatricesKernel
    )
    set_target_properties(NiMeshDLL PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/EngineLibs")
    target_precompile_headers(NiMeshDLL PRIVATE NiMeshPCH.h)
    target_compile_definitions(NiMeshDLL PRIVATE -DNIMESH_EXPORT -D_USRDLL INTERFACE -DNIMESH_IMPORT)
endif()
