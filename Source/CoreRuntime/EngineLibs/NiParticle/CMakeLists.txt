set(MOD_EXTRA_SRC_PATH 
    _Deprecated
    Kernels/NiPSAlignedQuadGeneratorKernelFF
    Kernels/NiPSAlignedQuadGeneratorKernelFV
    Kernels/NiPSAlignedQuadGeneratorKernelVF
    Kernels/NiPSAlignedQuadGeneratorKernelVV
    Kernels/NiPSAlignedQuadTextureKernel
    Kernels/NiPSFacingQuadGeneratorKernel
    Kernels/NiPSSimulatorCollidersKernel
    Kernels/NiPSSimulatorFinalKernel
    Kernels/NiPSSimulatorForcesKernel
    Kernels/NiPSSimulatorGeneralKernel
    Kernels/NiPSSimulatorMeshAlignKernelFF
    Kernels/NiPSSimulatorMeshAlignKernelFV
    Kernels/NiPSSimulatorMeshAlignKernelVF
    Kernels/NiPSSimulatorMeshAlignKernelVV
)

set(MOD_INCLUDES
    ${CMAKE_CURRENT_LIST_DIR} 
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSAlignedQuadGeneratorKernelFF
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSAlignedQuadGeneratorKernelFV
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSAlignedQuadGeneratorKernelVF
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSAlignedQuadGeneratorKernelVV
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSAlignedQuadTextureKernel
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSFacingQuadGeneratorKernel
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSSimulatorCollidersKernel
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSSimulatorFinalKernel
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSSimulatorForcesKernel
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSSimulatorGeneralKernel
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSSimulatorMeshAlignKernelFF
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSSimulatorMeshAlignKernelFV
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSSimulatorMeshAlignKernelVF
    ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiPSSimulatorMeshAlignKernelVV
)

set(MOD_NAME NiParticle)
set(MOD_P_INCLUDES _Deprecated)
set(MOD_PCH NiParticlePCH.h)
set(MOD_GB_LIBS NiAnimation)

generate_project()
