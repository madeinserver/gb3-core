// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 2022 Arves100/Made In Server Developers.
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#pragma once
#ifndef EE_SDL2PlatformServiceService_h
#define EE_SDL2PlatformServiceService_h

#include <efd/OS.h>

#include <efd/ISystemService.h>
#include <efd/ServiceManager.h>


namespace efd
{
class EE_EFD_ENTRY SDL2PlatformService : public efd::ISystemService
{
    /// @cond EMERGENT_INTERNAL
    EE_DECLARE_CLASS1(SDL2PlatformService, efd::kCLASSID_SDL2PlatformService, ISystemService);
    EE_DECLARE_CONCRETE_REFCOUNT;
    /// @endcond

protected:
    /// Destructor
    virtual ~SDL2PlatformService();

public:
    /// Constructor
    SDL2PlatformService(
#ifdef _WIN32
        HINSTANCE hInstance,
#endif
        int argc,
        char** argv,
        efd::SInt32 flags = SDL_WINDOW_MINIMIZED);

    /// @name Configuration options
    /// These functions are only useful to call prior to PreInit.  These values are used
    /// during the default RegisterClass/InitInstance functions.
    //@{

    /// Sets the initial window title from a string. The window title should be set shortly
    /// after creating the platform service and before ticking services.
    void SetWindowTitle(const efd::utf8string& strTitle);

#ifdef EE_PLATFORM_WIN32
    /// Sets the initial window title from a resource string. The window title should be set
    /// shortly after creating the platform service and before ticking services.
    void SetWindowTitle(efd::UInt32 titleResourceID);

    /// Sets the icon for the created window using resource IDs. If
    /// the icon is not set the system will use a default icon. The window icon, if set, should be
    /// set shortly after creating the platform service and before ticking services.
    void SetWindowIcon(efd::UInt32 iconResourceID);
#endif

    /// Sets the initial window width. This can be called as soon as the service is created. If
    /// called before the service is initialized this will set the initial width of the window. If
    /// called after that point this will adjust the window size on the next tick of this service.
    void SetWindowWidth(efd::UInt32 width);

    /// Sets the initial window height. This can be called as soon as the service is allocated. If
    /// called before the service is initialized this will set the initial height of the window. If
    /// called after that point this will adjust the window size on the next tick of this service.
    void SetWindowHeight(efd::UInt32 height);

    /// Sets the window X position. This can be called as soon as the service is allocated. If
    /// called before the service is initialized this will set the initial position of the window.
    /// If called after that point this will adjust the window position on the next tick of this
    /// service.
    void SetWindowX(efd::UInt32 x);

    // Sets the window Y position. This can be called as soon as the service is allocated. If
    /// called before the service is initialized this will set the initial position of the window.
    /// If called after that point this will adjust the window position on the next tick of this
    /// service.
    void SetWindowY(efd::UInt32 y);
    //@}

    /// @name Window accessor functions
    //@{
    /// Returns the HWND for this window.
    efd::WindowRef GetWindowRef() const;
    //@}

protected:
    /// Creates the window.
    virtual bool InitInstance(UInt32 nFlags);

    /// Sets up window instance.
    virtual efd::SyncResult OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar);

    /// Default message pump.  Quits on quit message.
    virtual efd::AsyncResult OnTick();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual const char* GetDisplayName() const;

    int m_nCmds;
    char** m_lpCmdLine;
    efd::UInt32 m_nFlags;
    efd::WindowRef m_hWnd;

    efd::utf8string m_strWindowTitle;
    efd::utf8string m_strWindowClass;

    efd::SInt32 m_windowWidth;
    efd::SInt32 m_windowHeight;
    efd::SInt32 m_windowX;
    efd::SInt32 m_windowY;
    bool m_isDirty;

    efd::UInt32 m_maxMessagesPerTick;

    SDL_Surface* m_hIcon;

#ifdef _WIN32
    efd::InstanceRef m_hInstance;
#endif
};

typedef efd::SmartPointer<SDL2PlatformService> SDL2PlatformServicePtr;

} // end namespace efd

#endif // EE_SDL2PlatformServiceService_h
