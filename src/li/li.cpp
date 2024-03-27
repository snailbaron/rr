#include <li.hpp>

#include "error.hpp"

namespace rr {

DynamicLibrary::DynamicLibrary(const std::filesystem::path& path)
{
    load(path);
}

DynamicLibrary::DynamicLibrary(DynamicLibrary&& other) noexcept
{
    swap(*this, other);
}

DynamicLibrary::~DynamicLibrary()
{
    clear();
}

DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& other) noexcept
{
    if (this != &other) {
        clear();
        swap(*this, other);
    }
    return *this;
}

DynamicLibrary::operator bool() const noexcept
{
#if defined(_WIN32)
    return _instance != NULL;
#endif
}

void DynamicLibrary::load(const std::filesystem::path& path)
{
#if defined(_WIN32)
    _instance = LoadLibraryA(path.string().c_str());
    if (_instance == NULL) {
        throw Error{} << "could not load dynamic library: " << path;
    }
#endif
}

void DynamicLibrary::clear() noexcept
{
#if defined(_WIN32)
    if (_instance != NULL) {
        FreeLibrary(_instance);
    }
#endif
}

void swap(DynamicLibrary& lhs, DynamicLibrary& rhs) noexcept
{
#if defined(_WIN32)
    std::swap(lhs._instance, rhs._instance);
#endif
}

void* DynamicLibrary::loadInternalProcAddress(const char* name) const
{
#if defined(_WIN32)
    FARPROC address = GetProcAddress(_instance, name);
    if (address == NULL) {
        throw Error() << "could not load proc address: " << name;
    }
    return address;
#endif
}

} // namespace rr