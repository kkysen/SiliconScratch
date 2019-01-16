#pragma once

#include "detail/EndOfCentralDirectoryBlock.h"

#include "ZipArchiveEntry.h"

#include <istream>
#include <vector>
#include <algorithm>
#include <atomic>
#include <memory>

/**
 * \brief Represents a package of compressed files in the zip archive format.
 */
class ZipArchive {
    
    friend class ZipFile;
    
    friend class ZipArchiveEntry;

public:
    
    typedef std::shared_ptr<ZipArchive> Ptr;

private:
    
    enum class SeekDirection {
        Forward,
        Backward
    };
    
    detail::EndOfCentralDirectoryBlock _endOfCentralDirectoryBlock;
    std::vector<ZipArchiveEntry::Ptr> _entries;
    std::istream* _zipStream;
    bool _owningStream;
    
public:
    
    ZipArchive();
    
    /**
     * \brief Destructor.
     */
    ~ZipArchive();
    
    /**
     * \brief Move assignment operator.
     *
     * \param other The ZipArchive instance to move.
     *
     * \return  A shallow copy of this object.
     */
    ZipArchive& operator=(ZipArchive&& other) noexcept;
    
    const std::vector<ZipArchiveEntry::Ptr>& entries() const noexcept;
    
    std::vector<ZipArchiveEntry::Ptr>& entries() noexcept;
    
    const std::string& comment() const noexcept;
    
    std::string& comment() noexcept;
    
    /**
     * \brief Default constructor.
     */
    static ZipArchive::Ptr Create();
    
    /**
     * \brief Move constructor.
     *
     * \param other The ZipArchive instance to move.
     */
    static ZipArchive::Ptr Create(ZipArchive::Ptr&& other);
    
    /**
     * \brief Constructor.
     *
     * \param stream The input stream of the zip archive content. Must be seekable.
     */
    static ZipArchive::Ptr Create(std::istream& stream);
    
    /**
     * \brief Constructor. It optionally allows to simultaneously destroy and dealloc the input stream
     *        with the ZipArchive.
     *
     * \param stream                The input stream of the zip archive content. Must be seekable.
     * \param takeOwnership         If true, it calls "delete stream" in the ZipArchive destructor.
     */
    static ZipArchive::Ptr Create(std::istream* stream, bool takeOwnership);
    
    /**
     * \brief Creates an zip entry with given file name.
     *
     * \param fileName  Filename of the file.
     *
     * \return  nullptr if it fails, else the new entry.
     */
    ZipArchiveEntry::Ptr CreateEntry(const std::string& fileName);
    
    /**
     * \brief Writes the zip archive content to the stream. It must be seekable.
     *
     * \param stream The stream to write in.
     */
    void WriteToStream(std::ostream& stream);
    
    /**
     * \brief Swaps this instance of ZipArchive with another instance.
     *
     * \param other The instance to swap with.
     */
    void Swap(ZipArchive::Ptr other);

private:
    
    bool EnsureCentralDirectoryRead();
    
    bool ReadEndOfCentralDirectory();
    
    bool SeekToSignature(uint32_t signature, SeekDirection direction);
    
    void InternalDestroy();
    
};
