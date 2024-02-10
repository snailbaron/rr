#include <mm.hpp>

#include <error.hpp>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstring>
#include <source_location>

namespace rr {

namespace {

void checkErrno(std::source_location sl = std::source_location::current())
{
    int e = errno;
    throw Error{sl} << strerrorname_np(e) << ": " << strerrordesc_np(e);
}

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
    std::swap(_addr, other._addr);
    std::swap(_len, other._len);
}

MemoryMap& MemoryMap::operator=(MemoryMap&& other) noexcept
{
    if (this != &other) {
        clear();
        std::swap(_addr, other._addr);
        std::swap(_len, other._len);
    }
    return *this;
}

void MemoryMap::map(const std::filesystem::path& path)
{
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
}

void MemoryMap::clear()
{
    if (_addr) {
        munmap(_addr, _len);
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

} // namespace rr
