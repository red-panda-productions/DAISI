
#ifndef IPCLIB_EXPORT_H
#define IPCLIB_EXPORT_H

#ifdef IPCLIB_STATIC_DEFINE
#  define IPCLIB_EXPORT
#  define IPCLIB_NO_EXPORT
#else
#  ifndef IPCLIB_EXPORT
#    ifdef IPCLib_EXPORTS
        /* We are building this library */
#      define IPCLIB_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define IPCLIB_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef IPCLIB_NO_EXPORT
#    define IPCLIB_NO_EXPORT 
#  endif
#endif

#ifndef IPCLIB_DEPRECATED
#  define IPCLIB_DEPRECATED __declspec(deprecated)
#endif

#ifndef IPCLIB_DEPRECATED_EXPORT
#  define IPCLIB_DEPRECATED_EXPORT IPCLIB_EXPORT IPCLIB_DEPRECATED
#endif

#ifndef IPCLIB_DEPRECATED_NO_EXPORT
#  define IPCLIB_DEPRECATED_NO_EXPORT IPCLIB_NO_EXPORT IPCLIB_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef IPCLIB_NO_DEPRECATED
#    define IPCLIB_NO_DEPRECATED
#  endif
#endif

#endif /* IPCLIB_EXPORT_H */
