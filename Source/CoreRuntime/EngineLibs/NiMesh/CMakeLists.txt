set(MOD_NAME NiMesh)
set(MOD_GB_LIBS NiFloodgate)
set(MOD_EXTRA_SRC_PATH 
    Kernels/NiSkinningKernelPNBT32
    Kernels/NiSkinningKernelPNBT16
    Kernels/NiSkinningKernelPN32
    Kernels/NiSkinningKernelPN16
    Kernels/NiSkinningKernelP
    Kernels/NiMorphingKernel
    Kernels/NiInstanceCullingKernel
    Kernels/NiCalculateBoneMatricesKernel
)
set(MOD_INCLUDES
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPNBT32
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPNBT16
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPN32
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelPN16
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiSkinningKernelP
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiMorphingKernel
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiInstanceCullingKernel
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiCalculateBoneMatricesKernel
)
set(MOD_PCH NiMeshPCH.h)

generate_project()

