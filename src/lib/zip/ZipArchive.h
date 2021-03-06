#pragma once

#include "src/lib/zip/detail/EndOfCentralDirectoryBlock.h"

#include "ZipArchiveEntry.h"

#include <istream>
#include <vector>
#include <algorithm>
#include <atomic>
#include <memory>
#include <optional>
#include <variant>
#include <cassert>

#include "src/main/util/MappedIterator.h"
#include "src/main/util/numbers.h"
#include "src/lib/fs/fs.h"

/**
 * \brief Represents a package of compressed files in the zip archive format.
 */
class ZipArchive {
    
    friend class ZipArchiveEntry;

public:
    
    using Entries = std::vector<std::unique_ptr<ZipArchiveEntry>>;

private:
    
    Entries _entries;
    detail::EndOfCentralDirectoryBlock endOfCentralDirectoryBlock;
    std::unique_ptr<std::istream> stream;

private:
    
    struct Iterators {
        
        static const ZipArchiveEntry& mapConst(const std::unique_ptr<ZipArchiveEntry>& entry) noexcept {
            return *entry;
        }
    
        static ZipArchiveEntry& map(std::unique_ptr<ZipArchiveEntry>& entry) noexcept {
            return *entry;
        }
        
    };
    
    using ConstPtrIterator = typename Entries::const_iterator;
    using PtrIterator = typename Entries::iterator;

public:
    
    using ConstIterator = decltype(iterators::map(_entries.begin(), Iterators::mapConst));
    using Iterator = decltype(iterators::map(_entries.begin(), Iterators::mapConst));

public:
    
    std::string_view comment() const noexcept;
    
    void setComment(std::string_view comment) noexcept;

private:
    
    const Entries& entries() const noexcept;
    
    Entries& entries() noexcept;

public:
    
    size_t size() const noexcept;
    
    decltype(auto) begin() const noexcept {
        return iterators::map(entries().begin(), Iterators::mapConst);
    }
    
    decltype(auto) begin() noexcept {
        return iterators::map(entries().begin(), Iterators::map);
    }
    
    decltype(auto) end() const noexcept {
        return iterators::map(entries().end(), Iterators::mapConst);
    }
    
    decltype(auto) end() noexcept {
        return iterators::map(entries().end(), Iterators::map);
    }
    
    const ZipArchiveEntry& operator[](size_t i) const noexcept;
    
    ZipArchiveEntry& operator[](size_t i) noexcept;

private:
    
    static constexpr size_t invalidIndex = static_cast<size_t>(-1);
    
    size_t findEntry(std::string_view name) const noexcept;
    
    void removeEntry(size_t index);

public:
    
    class MaybeEntry;
    
    class ConstMaybeEntry {
        
        friend MaybeEntry;
    
    private:
        
        const ZipArchive& archive;
        std::variant<size_t, std::string> variant = invalidIndex;
        // store either entry, which has a name, or name for creation
    
    public:
        
        ConstMaybeEntry(const ZipArchive& archive, const std::string& name) noexcept;
    
    private:
        
        std::string_view directName() const noexcept;
        
        size_t index() const noexcept;
        
        const ZipArchiveEntry& entry() const noexcept;
        
        std::string_view entryName() const noexcept;
        
        std::runtime_error error(std::string_view action, std::string_view reason) const noexcept;
    
    public:
        
        bool exists() const noexcept;
        
        operator bool() const noexcept;
        
        std::string_view name() const noexcept;
        
        const ZipArchiveEntry& get() const;
        
    };
    
    class MaybeEntry {
    
    private:
        
        ConstMaybeEntry impl;
    
    public:
        
        MaybeEntry(ZipArchive& archive, const std::string& name) noexcept;
    
    private:
        
        Entries& entries() noexcept;
        
        ZipArchiveEntry& entry() noexcept;
    
    public:
        
        bool exists() const noexcept;
        
        operator bool() const noexcept;
        
        std::string_view name() const noexcept;
        
        void setName(std::string_view name) noexcept;
        
        const ZipArchiveEntry& get() const;
        
        ZipArchiveEntry& get();
    
    private:
        
        void createForcefully();
        
        void removeForcefully();
    
    public:
        
        enum class CreateMode { IF_NOT_EXISTS, OVERWRITE, FAIL_IF_EXISTS };
        
        MaybeEntry& create(CreateMode mode);
        
        enum class RemoveMode { IF_EXISTS, FAIL_IF_NOT_EXISTS };
        
        MaybeEntry& remove(RemoveMode mode);
        
    };
    
    ConstMaybeEntry entry(const std::string& name) const noexcept;
    
    MaybeEntry entry(const std::string& name) noexcept;

private:
    
    enum class SeekDirection : i32 {
        Forward = 1,
        Backward = -1,
    };
    
    bool seekToSignature(u32 signature, SeekDirection direction);
    
    bool readEndOfCentralDirectory();
    
    bool ensureCentralDirectoryRead();
    
    bool init();

public:
    
    ZipArchive(ZipArchive&& other) = delete;
    
    ZipArchive(const ZipArchive& other) = delete;
    
    ZipArchive& operator=(ZipArchive&& other) = delete;
    
    ZipArchive& operator=(const ZipArchive& other) = delete;
    
    explicit ZipArchive(std::unique_ptr<std::istream>&& stream);
    
    explicit ZipArchive(const fs::path& path);
    
    void writeTo(std::ostream& out);
    
};

std::ostream& operator<<(ZipArchive& archive, std::ostream& out);
