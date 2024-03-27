#include <mm.hpp>

#include <error.hpp>

#if defined(__linux)
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <unistd.h>
#elif defined(_WIN32)
    #include <Windows.h>
#endif

#include <cstring>
#include <source_location>

namespace rr {

namespace {

#if defined(__linux__)
void checkErrno(std::source_location sl = std::source_location::current())
{
    int e = errno;
    throw Error{sl} << strerrorname_np(e) << ": " << strerrordesc_np(e);
}
#elif defined(_WIN32)
[[noreturn]] void throwWindowsError()
{
    DWORD error = GetLastError();

    LPTSTR messageBuffer = nullptr;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        messageBuffer,
        0,
        NULL);

    std::string message{messageBuffer};

    LocalFree(messageBuffer);

    throw Error{} << message;
}
#endif

} // namespace

MemoryMap::MemoryMap(const std::filesystem::path& path)
{
    map(path);
}

MemoryMap::~MemoryMap()
{
    clear();
}

MemoryMap::MemoryMap(MemoryMap&& other) noexcept
{
    swap(*this, other);
}

MemoryMap& MemoryMap::operator=(MemoryMap&& other) noexcept
{
    if (this != &other) {
        clear();
        swap(*this, other);
    }
    return *this;
}

void MemoryMap::map(const std::filesystem::path& path)
{
#if defined(__linux)
    int fd = open(path.string().c_str(), O_RDONLY);
    if (fd == -1) {
        int e = errno;
        throw Error{} << "cannot open file " << path << ": " <<
            strerrorname_np(e) << ": " << strerrordesc_np(e);
    };

    auto fileSize = lseek(fd, 0, SEEK_END);
    if (fileSize == -1) {
        checkErrno();
    }
    _len = fileSize;

    void* addr = mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        checkErrno();
    }
    _addr = addr;
#elif defined(_WIN32)
    HANDLE fileHandle = CreateFileW(
        path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        throwWindowsError();
    }

    LARGE_INTEGER fileSize;
    if (GetFileSizeEx(fileHandle, &fileSize) == 0) {
        throwWindowsError();
    }
    _len = fileSize.QuadPart;

    HANDLE mappingHandle = CreateFileMapping(
        fileHandle,
        NULL,
        PAGE_READONLY,
        0,
        0,
        NULL);
    if (mappingHandle == NULL) {
        throwWindowsError();
    }
    _mappingHandle = mappingHandle;

    if (CloseHandle(fileHandle) == 0) {
        throwWindowsError();
    }

    LPVOID addr = MapViewOfFile(
        mappingHandle,
        FILE_MAP_READ,
        0,
        0,
        0);
    if (addr == NULL) {
        throwWindowsError();
    }
    _addr = addr;
#endif
}

void MemoryMap::clear()
{
    if (_addr) {
#if defined(__linux__)
        munmap(_addr, _len);
#elif defined(_WIN32)
        UnmapViewOfFile(_addr);
        CloseHandle(_mappingHandle);
#endif
    }
    _addr = nullptr;
    _len = 0;
}

void* MemoryMap::addr() const
{
    return _addr;
}

size_t MemoryMap::size() const
{
    return _len;
}

void swap(MemoryMap& x, MemoryMap& y) noexcept
{
    std::swap(x._addr, y._addr);
    std::swap(x._len, y._len);
#ifdef _WIN32
    std::swap(x._mappingHandle, y._mappingHandle);
#endif
}

} // namespace rr
