set(SOURCES Connection.cpp
            OptionalsReg.cpp
            Output.cpp
            InputProcessorKeyboard.h
            InputProcessorPointer.h
            InputProcessorTouch.h
            OSScreenSaverIdleInhibitUnstableV1.cpp
            Registry.cpp
            Seat.cpp
            SeatInputProcessing.cpp
            SeatSelection.cpp
            ShellSurface.cpp
            ShellSurfaceWlShell.cpp
            ShellSurfaceXdgShell.cpp
            ShellSurfaceXdgShellUnstableV6.cpp
            Util.cpp
            VideoSyncWpPresentation.cpp
            WindowDecorator.cpp
            WinEventsWayland.cpp
            WinSystemWayland.cpp
            XkbcommonKeymap.cpp)

set(HEADERS Connection.h
            OptionalsReg.h
            Output.h
            InputProcessorKeyboard.cpp
            InputProcessorPointer.cpp
            InputProcessorTouch.cpp
            OSScreenSaverIdleInhibitUnstableV1.h
            Registry.h
            Seat.h
            SeatInputProcessing.h
            SeatSelection.h
            ShellSurface.h
            ShellSurfaceWlShell.h
            ShellSurfaceXdgShell.h
            ShellSurfaceXdgShellUnstableV6.h
            Signals.h
            VideoSyncWpPresentation.h
            WindowDecorator.h
            WinEventsWayland.h
            WinSystemWayland.h
            XkbcommonKeymap.h)

if(EGL_FOUND)
  list(APPEND SOURCES WinSystemWaylandEGLContext.cpp)
  list(APPEND HEADERS WinSystemWaylandEGLContext.h)
endif()

if(OPENGL_FOUND)
  list(APPEND SOURCES WinSystemWaylandEGLContextGL.cpp)
  list(APPEND HEADERS WinSystemWaylandEGLContextGL.h)
endif()
if(OPENGLES_FOUND)
  list(APPEND SOURCES WinSystemWaylandEGLContextGLES.cpp)
  list(APPEND HEADERS WinSystemWaylandEGLContextGLES.h)
endif()
if(TARGET_WEBOS)
  list(APPEND SOURCES OSScreenSaverWebOS.cpp
                      ShellSurfaceWebOSShell.cpp
                      WinSystemWaylandWebOS.cpp)
  list(APPEND HEADERS OSScreenSaverWebOS.h
                      ShellSurfaceWebOSShell.h
                      WinSystemWaylandWebOS.h)
endif()

core_add_library(windowing_WAYLAND)
