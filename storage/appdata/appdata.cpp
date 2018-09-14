#define _CRT_SECURE_NO_WARNINGS

#include "appdata.hpp"

#include <cstdlib>
#include <iterator>
#include <mlib/platform.hpp>

#ifdef MLIB_PLATFORM_WIN32
    #include <Windows.h>
#elif defined MLIB_PLATFORM_UNIX
    #include <unistd.h>
    #ifdef MLIB_PLATFORM_APPLEOS
        #include <corefoundation/CFBundle.h>
    #endif
#else
    #error unknown platform
#endif

namespace mlib::storage::appdata
{

using namespace std::filesystem;

static void appendGenericPath(path &p, const std::string &generic)
{
    for(auto c = generic.begin(); c != generic.end(); ++c)
    {
        if(*c == '/')
            p += path::preferred_separator;
        else if(!(*c == '.' && c+1 != generic.end() && c[1] == '.')) //ignore '..'
            p += *c;
    }
}

Appdata::Appdata(const std::string &appsubdir, const std::string &resourcesubdir)
{

#ifdef MLIB_PLATFORM_WIN32
    const wchar_t *env = _wgetenv(L"appdata");
    if(!env)
        throw RetrieveError("%appdata%");

    wchar_t cwd[MAX_PATH];
    if(!_wgetcwd(cwd, MAX_PATH))
        throw RetrieveError("_wgetcwd");
    
    path appdata = env;
    path resdir = cwd;
#elif defined MLIB_PLATFORM_UNIX
    const char *env = getenv("HOME");
    if(!env)
        throw RetrieveError("$HOME");

    path appdata = u8path(env);

    #ifdef MLIB_PLATFORM_APPLEOS
        CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
        if(!appUrlRef)
            throw RetrieveError("CFBundleCopyBundleURL");

        CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
        const char *rp = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());

        CFRelease(macPath);
        CFRelease(appUrlRef);

        resdir = rp;
    #else  
        char cwd[260];
        if(!getcwd(cwd, 260))
            throw RetrieveError("getcwd");

        path resdir = cwd;
    #endif
#endif

    appdata += path::preferred_separator;
    appendGenericPath(appdata, appsubdir);
    appdata += path::preferred_separator;

    create_directories(appdata.parent_path());

    resdir += path::preferred_separator;
    resdir += resourcesubdir;
    resdir += path::preferred_separator;

    StoragePath = std::move(appdata);
    ResourcePath = std::move(resdir);
}

std::fstream Appdata::open(const std::string &spath, std::ios::openmode flags, Type type)
{
    path p = (type == Type::Storage) ? StoragePath : ResourcePath;
    appendGenericPath(p, spath);
    return std::fstream(p, flags);
}

}