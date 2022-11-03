#include <corefoundation/CFBundle.h>
#include <string>

const std::string& resourcePath()
{
    static auto resourcePath = [] () -> std::string
    {
        char path[PATH_MAX];
        auto bundleRef = CFBundleGetMainBundle();
        auto resourcesUrlRef = CFBundleCopyResourcesDirectoryURL(bundleRef);
        
        if (resourcesUrlRef != NULL)
        {
            CFURLGetFileSystemRepresentation(resourcesUrlRef, true, reinterpret_cast<UInt8 *>(path), PATH_MAX);
            CFRelease(resourcesUrlRef);
            
            return std::string(path) + "/";
        }
        
        return {};
    } ();
    
    return resourcePath;
}
