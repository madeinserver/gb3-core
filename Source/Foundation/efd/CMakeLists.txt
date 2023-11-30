set(EFD_SRC
    efdLibType.h
    Asserts.h
    File.h
    Archive.cpp
    Archive.h
    Archive.inl
    AssertDialog.h
    AssetApplicationInfoMsg.cpp
    AssetApplicationInfoMsg.h
    AssetCacheResponse.cpp
    AssetCacheResponse.h
    AssetCacheResponse.inl
    AssetChangeHandler.h
    AssetConfigService.cpp
    AssetConfigService.h
    AssetConfigService.inl
    AssetFactoryManager.cpp
    AssetFactoryManager.h
    AssetFactoryManager.inl
    AssetID.cpp
    AssetID.h
    AssetID.inl
    AssetLoaderThreadFunctor.cpp
    AssetLoaderThreadFunctor.h
    AssetLoaderThreadFunctor.inl
    AssetLoadRequest.cpp
    AssetLoadRequest.h
    AssetLoadRequest.inl
    AssetLoadResponse.cpp
    AssetLoadResponse.h
    AssetLoadResponse.inl
    AssetLocatorRequest.cpp
    AssetLocatorRequest.h
    AssetLocatorResponse.cpp
    AssetLocatorResponse.h
    AssetLocatorService.cpp
    AssetLocatorService.h
    AssetMetaGraph.cpp
    AssetMetaGraph.h
    AssetUriReader.cpp
    AssetUriReader.h
    AssetUtil.cpp
    AssetUtil.h
    AssetWeb.cpp
    AssetWeb.h
    AtomicOperations.h
    BaseAssetFactory.cpp
    BaseAssetFactory.h
    BaseAssetFactory.inl
    BinaryLoadSave.h
    BinaryStream.cpp
    BinaryStream.h
    BinaryStream.inl
    BinaryStreamMacro.h
    BitUtils.h
    Category.h
    Category.inl
    CircularBuffer.cpp
    CircularBuffer.h
    ClassIDHelpers.h
    ClassInfo.h
    ClassInfo.inl
    Color.cpp
    Color.h
    Color.inl
    ConfigManager.cpp
    ConfigManager.h
    ConfigManager.inl
    ConfigSection.cpp
    ConfigSection.h
    ConfigSource.cpp
    ConfigSource.h
    ConnectionID.cpp
    ConnectionID.h
    ConnectionID.inl
    CriticalSection.h
    customalloc.h
    customalloc.inl
    DataDrivenEnum.h
    DataDrivenEnum.inl
    DataStream.cpp
    DataStream.h
    DataStream.inl
    DataStreamUtils.h
    DataStreamUtils.inl
    DDEHeaderGenerator.h
    DDEParser.cpp
    DDEParser.h
    DebugOutDestination.cpp
    DebugOutDestination.h
    DeclareIDHelpers.h
    DefaultInitializeMemoryLogHandler.h
    DefaultInitializeMemoryManager.h
    DefaultInitializeMemoryManager.inl
    DependencyGraph.h
    DependencyGraph.inl
    DynamicModule.h
    ecrLogIDs.h
    EEBasicString.h
    EEHelpers.h
    EEMath.cpp
    EEMath.h
    efdBaseIDs.h
    efdClassIDs.h
    efdLogIDs.h
    efdMessageIDs.h
    efdNetFeatures.h
    efdPCH.h
    efdPrimitiveTypeIDs.h
    efdQualityOfService.h
    efdSystemServiceIDs.h
    efdVirtualQualityOfService.h
    Endian.h
    EnumManager.cpp
    EnumManager.h
    EnumManager.inl
    ExternalMemAllocator.h
    ExternalMemAllocator.inl
    File.cpp
    FileCommon.cpp
    FileCommon.h
    FileDestination.cpp
    FileDestination.h
    FixedNodePool.h
    FixedNodePool.inl
    FixedSizeAllocator.cpp
    FixedSizeAllocator.h
    FixedString.cpp
    FixedString.h
    FixedString.inl
    Float16.h
    Float16.inl
    Foundation.h
    FoundationSDM.cpp
    FoundationSDM.h
    GenericAssetFactory.cpp
    GenericAssetFactory.h
    GenericAssetFactory.inl
    GenericAssetLoadResponse.cpp
    GenericAssetLoadResponse.h
    GenericFactory.h
    GenericFactory.inl
    GlobalStringTable.cpp
    GlobalStringTable.h
    GlobalStringTable.inl
    GrowableBuffer.cpp
    GrowableBuffer.h
    GrowableBuffer.inl
    Helpers.cpp
    Helpers.h
    IAllocator.h
    IAssetFactory.cpp
    IAssetFactory.h
    IAssetFactory.inl
    IAssetImporter.h
    IAssetLocatorService.h
    IAssetServer.h
    IAssetTagger.h
    IBase.cpp
    IBase.h
    IConfigManager.h
    IConfigSection.h
    IConfigSection.inl
    IConfigSource.h
    ID128.cpp
    ID128.h
    ID128.inl
    IDs.h
    IFrameEvent.h
    IFrameEvent.inl
    ILogDestination.h
    ILogDestination.inl
    ILogger.cpp
    ILogger.h
    IMemLogHandler.cpp
    IMemLogHandler.h
    IMessage.cpp
    IMessage.h
    IMessage.inl
    IMessageHelperSystemService.cpp
    IMessageHelperSystemService.h
    INetCallback.h
    INetListener.h
    INetService.cpp
    INetService.h
    InitOptions.cpp
    InitOptions.h
    IntrusiveLockFreeStack.h
    IntrusiveLockFreeStack.inl
    IServiceDetailRegister.h
    IStreamable.h
    ISystemService.cpp
    ISystemService.h
    ISystemService.inl
    IUtilities.h
    LockFreeQueue.h
    LockFreeQueue.inl
    LockFreeStack.h
    LockFreeStack.inl
    Logger.cpp
    Logger.h
    LoggerMacros.h
    LoopedThread.cpp
    LoopedThread.h
    LoopedThread.inl
    LoopedThreadProcedure.cpp
    LoopedThreadProcedure.h
    Matrix3.h
    MemHint.h
    MemHint.inl
    MemManager.cpp
    MemManager.h
    MemManager.inl
    MemObject.cpp
    MemObject.h
    MemObject.inl
    MemoryDefines.cpp
    MemoryDefines.h
    MemTracker.cpp
    MemTracker.h
    MemTracker.inl
    MessageFactory.h
    MessageHandlerBase.h
    MessageService.cpp
    MessageService.h
    MessageService.inl
    Metrics.cpp
    Metrics.h
    NetEnums.h
    NetMessage.cpp
    NetMessage.h
    NetMessage.inl
    NetMetricsDestination.cpp
    NetMetricsDestination.h
    NetMetricsDestination.inl
    OrderedMap.h
    OrderedMap.inl
    OS.h
    ParameterList.cpp
    ParameterList.h
    ParameterList.inl
    ParameterListMessage.cpp
    ParameterListMessage.h
    ParameterListMessage.inl
    ParseHelper.h
    ParseHelper.inl
    PathUtils.cpp
    PathUtils.h
    PlatformService.h
    Point2.cpp
    Point2.h
    Point2.inl
    Point3.cpp
    Point3.h
    Point4.cpp
    Point4.h
    Point4.inl
    PrintDestination.cpp
    PrintDestination.h
    ProcessorAffinity.h
    ProcessorAffinity.inl
    QOSCompare.cpp
    QOSCompare.h
    QOSCompare.inl
    Quaternion.cpp
    Quaternion.h
    Quaternion.inl
    RefCount.h
    RefCount.inl
    ReferenceCounting.h
    ReloadManager.cpp
    ReloadManager.h
    RTLib.h
    SDMMacros.h
    Semaphore.h
    SequenceNumber.h
    SequenceNumber.inl
    Serialize.cpp
    Serialize.h
    SerializeEfdHelpers.h
    SerializeRoutines.h
    SerializeStlHelpers.h
    ServiceAllocator.cpp
    ServiceAllocator.h
    ServiceDiscoveryMessage.cpp
    ServiceDiscoveryMessage.h
    ServiceManager.cpp
    ServiceManager.h
    ServiceManager.inl
    SmallObjectAllocator.h
    SmallObjectAllocator.inl
    SmartBuffer.cpp
    SmartBuffer.h
    SmartCriticalSection.h
    SmartCriticalSection.inl
    SmartPointer.h
    SmartPointer.inl
    StackUtils.cpp
    StackUtils.h
    StandardAllocator.h
    StaticDataManager.cpp
    StaticDataManager.h
    StaticDataManager.inl
    StdContainers.h
    StdContainers.inl
    StdHashMap.cpp
    StdHashMap.h
    StdStreamFile.cpp
    StdStreamFile.h
    StreamInfo.h
    StreamMessage.cpp
    StreamMessage.h
    StreamMessage.inl
    String.cpp
    String.h
    String.inl
    StringConverters.h
    StringUtilities.h
    SystemDesc.cpp
    SystemDesc.h
    SystemDesc.inl
    SystemFrameworkEnums.h
    SystemLogger.cpp
    SystemLogger.h
    SystemUniqueIndex.h
    TemplateHelpers.h
    Thread.cpp
    Thread.h
    Thread.inl
    ThreadFunctor.cpp
    ThreadFunctor.h
    ThreadLocal.h
    TimeType.cpp
    TimeType.h
    TimeType.inl
    TinyXML.h
    ToolServiceManager.cpp
    ToolServiceManager.h
    UniversalID.cpp
    UniversalID.h
    UniversalID.inl
    UniversalTypes.h
    utf16char.cpp
    utf16char.inl
    utf8char.cpp
    utf8char.h
    utf8char.inl
    utf8string.cpp
    utf8string.h
    utf8string.inl
    utf8string_iterator.h
    utf8string_iterator.inl
    Utilities.h
    XMLMemLogHandler.cpp
    XMLMemLogHandler.h
    XMLUtils.cpp
)

set(SDL2_SRC
    SDL2/AtomicOperations_SDL2.inl
    SDL2/TimeType_SDL2.cpp
    SDL2/AssertDialog_SDL2.cpp
    SDL2/Utilities_SDL2.inl
    SDL2/Utilities_SDL2.cpp
    SDL2/CriticalSection_SDL2.inl
    SDL2/Semaphore_SDL2.inl
    SDL2/OS_SDL2.h
    SDL2/Utilities_SDL2.inl
    SDL2/Utilities_SDL2.cpp
    SDL2/PathUtils_SDL2.cpp
    SDL2/DynamicModule_SDL2.cpp
    SDL2/SDL2PlatformService.cpp
    SDL2/SDL2PlatformService.h
    SDL2/SDL2File.h
    SDL2/SDL2File.cpp
    SDL2/UniversalTypes_SDL2.h
    SDL2/PathUtils_SDL2.inl
    SDL2/RTLib_SDL2.h
    SDL2/StringUtilities_SDL2.cpp
    SDL2/StringUtilities_SDL2.inl
    SDL2/UniversalID_SDL2.h
    SDL2/EEMath_SDL2.inl
    SDL2/Point3_SDL2.cpp
    SDL2/Point3_SDL2.inl
    SDL2/Point4_SDL2.cpp
    SDL2/Point4_SDL2.inl
    SDL2/Matrix3_SDL2.cpp
    SDL2/Matrix3_SDL2.inl
    SDL2/ThreadLocal_SDL2.inl
    SDL2/SystemUniqueIndex_SDL2.inl
    SDL2/Endian_SDL2.inl
)

source_group("SDL2" FILES ${SDL2_SRC})

add_library(efdBase INTERFACE )
target_include_directories(efdBase INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../)
set_target_properties(efdBase PROPERTIES FOLDER "${GB3_PREFIX}Foundation")

target_compile_definitions(efdBase INTERFACE 
    $<$<CONFIG:DEBUG>:EE_CONFIG_DEBUG>;
    $<$<CONFIG:DEBUG>:EE_EFD_CONFIG_DEBUG>;
    $<$<CONFIG:DEBUG>:EE_USE_ASSERT_STACKTRACE>;
    $<$<CONFIG:DEBUG>:EE_USE_MEMORY_MANAGEMENT>;

    $<$<CONFIG:RELEASE>:EE_CONFIG_RELEASE>;
    $<$<CONFIG:RELEASE>:EE_EFD_CONFIG_RELEASE>;

    $<$<CONFIG:SHIPPING>:EE_CONFIG_SHIPPING>;
    $<$<CONFIG:SHIPPING>:EE_EFD_CONFIG_SHIPPING>;
    $<$<CONFIG:SHIPPING>:EE_DISABLE_LOGGING>;

    EE_USE_PCH;
    EE_USE_NATIVE_STL; # Do not use stlport!
    EE_GAMEBRYO_VERSION=7563629; # 7563629 -> MIS
)

target_link_libraries(efdBase INTERFACE
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    #tinyxml2::tinyxml2
    efdXML
)
target_compile_definitions(efdBase INTERFACE -DEE_PLATFORM_SDL2=1)

if("${GB3_ARCH}" STREQUAL "64")
    target_compile_definitions(efdBase INTERFACE -DEE_PLATFORM_64BIT=1)
else()
    target_compile_definitions(efdBase INTERFACE -DEE_PLATFORM_32BIT=1)
endif()

if(WIN32)
    target_compile_definitions(efdBase INTERFACE -DEE_PLATFORM_WIN32=1 -DSTRICT=1)
elseif(GB3_LINUX)
    target_compile_definitions(efdBase INTERFACE -DEE_PLATFORM_LINUX=1)
elseif(GB3_MACOSX)
    target_compile_definitions(efdBase INTERFACE -DEE_PLATFORM_MACOSX=1)
endif()

if (MSVC)
    target_compile_definitions(efdBase INTERFACE -DEE_COMPILER_MSVC=1)
elseif(GB3_GCC)
    target_compile_definitions(efdBase INTERFACE -DEE_COMPILER_GCC=1)
elseif(GB3_CLANG)
    target_compile_definitions(efdBase INTERFACE -DEE_COMPILER_CLANG=1)
endif()


add_library(efd STATIC ${EFD_SRC} ${SDL2_SRC})
set_target_properties(efd PROPERTIES FOLDER "${GB3_PREFIX}Foundation")
target_precompile_headers(efd PRIVATE efdPCH.h)
target_link_libraries(efd PUBLIC efdBase)
target_compile_definitions(efd PUBLIC -DEE_EFD_NO_IMPORT)

if (GB3_ENABLE_DLL)
    add_library(efdDLL SHARED ${EFD_SRC} ${SDL2_SRC})
    set_target_properties(efdDLL PROPERTIES FOLDER "${GB3_PREFIX}Foundation")
    target_precompile_headers(efdDLL PRIVATE efdPCH.h)
    target_link_libraries(efdDLL PUBLIC efdBase)
    target_compile_definitions(efdDLL PRIVATE -DEE_EFD_EXPORT INTERFACE -DEE_EFD_IMPORT PRIVATE -D_USRDLL)

    if (WIN32)
       target_link_libraries(efdDLL PRIVATE Rpcrt4)
     endif()
endif()