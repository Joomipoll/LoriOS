#pragma once
#include <stddef.h>
#include <stdint.h>
#include <List.h>
#include <Error.h>
#include <Lock.h>
#include <RefPtr.h>
#include <String.h>
#include <Types.h>
#include <Obj/KObject.h>
#include <abi-bits/fcntl.h>
#include <abi-bits/uid_t.h>

#define FD_SETSIZE 1024
#define PATH_MAX 4096
#define NAME_MAX 255

#define S_IFMT 0xF000
#define S_IFBLK 0x6000
#define S_IFCHR 0x2000
#define S_IFIFO 0x1000
#define S_IFREG 0x8000
#define S_IFDIR 0x4000
#define S_IFLNK 0xA000
#define S_IFSOCK 0xC000
#define POLLIN 0x01
#define POLLOUT 0x02
#define POLLPRI 0x04
#define POLLHUP 0x08
#define POLLERR 0x10
#define POLLRDHUP 0x20
#define POLLNVAL 0x40
#define POLLWRNORM 0x80

#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12
#define DT_WHT 14

#define FS_NODE_TYPE 0xF000
#define FS_NODE_FILE S_IFREG
#define FS_NODE_DIRECTORY S_IFDIR 
#define FS_NODE_MOUNTPOINT S_IFDIR
#define FS_NODE_BLKDEVICE S_IFBLK 
#define FS_NODE_SYMLINK S_IFLNK   
#define FS_NODE_CHARDEVICE S_IFCHR
#define FS_NODE_SOCKET S_IFSOCK   

#define O_ACCESS 7
#define O_EXEC 1
#define O_RDONLY 2
#define O_RDWR 3
#define O_SEARCH 4
#define O_WRONLY 5

#define POLLIN 0x01
#define POLLOUT 0x02
#define POLLPRI 0x04
#define POLLHUP 0x08
#define POLLERR 0x10
#define POLLRDHUP 0x20
#define POLLNVAL 0x40
#define POLLWRNORM 0x80

#define AT_EMPTY_PATH 1
#define AT_SYMLINK_FOLLOW 2
#define AT_SYMLINK_NOFOLLOW 4
#define AT_REMOVEDIR 8
#define AT_EACCESS 512

#define MAXIMUM_SYMLINK_AMOUNT 10

typedef int64_t ino_t;
typedef uint64_t dev_t;
typedef int64_t off_t;
typedef int32_t mode_t;
typedef int32_t nlink_t;
typedef int64_t volume_id_t;

typedef struct
{
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    uid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    int64_t st_blksize;
    int64_t st_blocks;
} stat_t;

class FileSystemNode;

struct pollfd
{
    int fd;
    short events;
    short revents;
};
typedef struct {
    char fds_bits[128];
} fd_set_t;

static inline void FD_CLR(int fd, fd_set_t* fds)
{
    assert(fd < FD_SETSIZE);
    fds->fds_bits[fd / 8] &= ~(1 << (fd % 8));
}

static inline int FD_ISSET(int fd, fd_set_t* fds)
{
    assert(fd < FD_SETSIZE);
    return fds->fds_bits[fd / 8] & (1 << (fd % 8));
}

static inline void FD_SET(int fd, fd_set_t* fds)
{
    assert(fd < FD_SETSIZE);
    fds->fds_bits[fd / 8] |= 1 << (fd % 8);
}

class FilesystemWatcher;
class DirectoryEntry;

class UNIXOpenFile : public KernelObject
{
    DECLARE_KOBJECT(UNIXOpenFile);
public:
    ~UNIXOpenFile();

    lock_t dataLock = 0;

    class FileSystemNode* node = nullptr;
    off_t pos = 0;
    mode_t mode = 0;
};

class FileSystemNode
{
    friend class FilesystemBlocker;

public:
    lock_t blockedLock = 0;
    FastList<class FilesystemBlocker*> blocked;

    uint32_t flags = 0;       // Flags
    uint32_t pmask = 0;       // Permission mask
    uid_t uid = 0;            // User id
    ino_t inode = 0;          // Inode number
    size_t size = 0;          // Node size
    int nlink = 0;            // Amount of references/hard links
    unsigned handleCount = 0; // Amount of file handles that point to this node
    volume_id_t volumeID;

    int error = 0;

    virtual ~FileSystemNode();
    
    virtual ssize_t Read(size_t off, size_t size, uint8_t* buffer); // Read Data

    virtual ssize_t Write(size_t off, size_t size, uint8_t* buffer); // Write Data

    virtual ErrorOr<UNIXOpenFile*> Open(size_t flags); // Open
    virtual void Close();                           // Close

    virtual int ReadDir(DirectoryEntry*, uint32_t); // Read Directory
    virtual FileSystemNode* FindDir(const char* name);            // Find in directory

    virtual int Create(DirectoryEntry* ent, uint32_t mode);
    virtual int CreateDirectory(DirectoryEntry* ent, uint32_t mode);

    virtual ssize_t ReadLink(char* pathBuffer, size_t bufSize);
    virtual int Link(FileSystemNode*, DirectoryEntry*);
    virtual int Unlink(DirectoryEntry*, bool unlinkDirectories = false);

    virtual int Truncate(off_t length);

    virtual int Ioctl(uint64_t cmd, uint64_t arg); // I/O Control
    virtual void Sync();                           // Sync node to device

    virtual bool CanRead() { return true; }
    virtual bool CanWrite() { return true; }

    virtual void Watch(FilesystemWatcher& watcher, int events);
    virtual void Unwatch(FilesystemWatcher& watcher);

    virtual inline bool IsFile() { return (flags & FS_NODE_TYPE) == FS_NODE_FILE; }
    virtual inline bool IsDirectory() { return (flags & FS_NODE_TYPE) == FS_NODE_DIRECTORY; }
    virtual inline bool IsBlockDevice() { return (flags & FS_NODE_TYPE) == FS_NODE_BLKDEVICE; }
    virtual inline bool IsSymlink() { return (flags & FS_NODE_TYPE) == FS_NODE_SYMLINK; }
    virtual inline bool IsCharDevice() { return (flags & FS_NODE_TYPE) == FS_NODE_CHARDEVICE; }
    virtual inline bool IsSocket() { return (flags & FS_NODE_TYPE) == FS_NODE_SOCKET; }
    virtual inline bool IsEPoll() const { return false; }

    void UnblockAll();

    FileSystemNode* link;
    FileSystemNode* parent;

    FilesystemLock nodeLock;
};

class DirectoryEntry
{
public:
    char name[NAME_MAX];

    FileSystemNode* node = nullptr;
    uint32_t inode = 0;

    DirectoryEntry* parent = nullptr;

    mode_t flags = 0;

    DirectoryEntry(FileSystemNode* node, const char* name);
    DirectoryEntry() {}

    static mode_t FileToDirentFlags(mode_t flags) {
        switch (flags & FS_NODE_TYPE) {
        case FS_NODE_FILE:
            flags = DT_REG;
            break;
        case FS_NODE_DIRECTORY:
            flags = DT_DIR;
            break;
        case FS_NODE_CHARDEVICE:
            flags = DT_CHR;
            break;
        case FS_NODE_BLKDEVICE:
            flags = DT_BLK;
            break;
        case FS_NODE_SOCKET:
            flags = DT_SOCK;
            break;
        case FS_NODE_SYMLINK:
            flags = DT_LNK;
            break;
        default:
            assert(!"Invalid file flags!");
        }
        return flags;
    }
};

/*
 * FilesystemWatcher is a semaphore initialized to 0.
 * A thread can wait on it like any semaphore,
 * and when a file is ready it will signal and waiting thread(s) will get woken
 */
class FilesystemWatcher : public Semaphore
{
    List<UNIXOpenFile*> watching;

public:
    FilesystemWatcher() : Semaphore(0) {}

    inline void WatchNode(FileSystemNode* node, int events)
    {
        ErrorOr<UNIXOpenFile*> desc = node->Open(0);
        assert(!desc.HasError() && desc.Value());

        UNIXOpenFile* f = desc.Value();
        f->node->Watch(*this, events);

        watching.add_back(f);
    }

    ~FilesystemWatcher()
    {
        for (auto& fd : watching)
        {
            fd->node->Unwatch(*this);

            delete fd;
        }
    }
};

class FilesystemBlocker : public ThreadBlocker
{
    friend FileSystemNode;
    friend FastList<FilesystemBlocker*>;

public:
    enum BlockType
    {
        BlockRead,
        BlockWrite,
    };

protected:
    FileSystemNode* node = nullptr;

    FilesystemBlocker* next;
    FilesystemBlocker* prev;

    int blockType = BlockType::BlockRead;

    size_t requestedLength = 1;
public:
    FilesystemBlocker(FileSystemNode* _node) : node(_node)
    {
        acquireLock(&lock);

        acquireLock(&node->blockedLock);
        node->blocked.add_back(this);
        releaseLock(&node->blockedLock);

        releaseLock(&lock);
    }

    FilesystemBlocker(FileSystemNode* _node, size_t len) : node(_node), requestedLength(len)
    {
        acquireLock(&lock);

        acquireLock(&node->blockedLock);
        node->blocked.add_back(this);
        releaseLock(&node->blockedLock);

        releaseLock(&lock);
    }

    void Interrupt();

    inline void Unblock()
    {
        shouldBlock = false;

        acquireLock(&lock);
        if (node && !removed)
        {
            node->blocked.remove(this);

            removed = true;
        }
        node = nullptr;

        if (thread)
            thread->Unblock();

        releaseLock(&lock);
    }

    inline size_t RequestedLength() { return requestedLength; }

    ~FilesystemBlocker();
};

typedef struct fs_dirent
{
    uint32_t inode;
    uint32_t type;
    char name[NAME_MAX]; // Filename
} fs_dirent_t;
namespace FileSystem
{
class FsVolume;

class FsDriver
{
public:
    virtual ~FsDriver() = default;
    
    virtual FsVolume* Mount(FileSystemNode* device, const char* name) = 0;
    virtual FsVolume* Unmount(FsVolume* volume) = 0;

    virtual int Identify(FileSystemNode* device) = 0;
    virtual const char* ID() const = 0;
};

extern List<FsDriver*> drivers;

void Initialize();

FileSystemNode* GetRoot();

void RegisterDriver(FsDriver* driver);
void UnregisterDriver(FsDriver* driver);

FsDriver* IdentifyFilesystem(FileSystemNode* node);

FileSystemNode* FollowLink(FileSystemNode* link, FileSystemNode* workingDir);

FileSystemNode* ResolvePath(const String& path, const char* workingDir = nullptr, bool followSymlinks = true);

FileSystemNode* ResolvePath(const String& path, FileSystemNode* workingDir, bool followSymlinks = true);

FileSystemNode* ResolveParent(const char* path, FileSystemNode* workingDir = nullptr);
String CanonicalizePath(const char* path, char* workingDir);
String BaseName(const char* path);

ssize_t Read(FileSystemNode* node, size_t offset, size_t size, void* buffer);

ssize_t Write(FileSystemNode* node, size_t offset, size_t size, void* buffer);
ErrorOr<UNIXOpenFile*> Open(FsNode* node, uint32_t flags = 0);
void Close(FileSystemNode* node);
void Close(UNIXOpenFile* openFile);
int ReadDir(FileSystemNode* node, DirectoryEntry* dirent, uint32_t index);
FileSystemNode* FindDir(FileSystemNode* node, const char* name);

ssize_t Read(const FancyRefPtr<UNIXOpenFile>& handle, size_t size, uint8_t* buffer);
ssize_t Write(const FancyRefPtr<UNIXOpenFile>& handle, size_t size, uint8_t* buffer);
int ReadDir(const FancyRefPtr<UNIXOpenFile>& handle, DirectoryEntry* dirent, uint32_t index);
FileSystemNode* FindDir(const FancyRefPtr<UNIXOpenFile>& handle, const char* name);

int Link(FileSystemNode*, FileSystemNode*, DirectoryEntry*);
int Unlink(FileSystemNode*, DirectoryEntry*, bool unlinkDirectories = false);

int Ioctl(const FancyRefPtr<UNIXOpenFile>& handle, uint64_t cmd, uint64_t arg);

int Rename(FileSystemNode* olddir, const char* oldpath, FileSystemNode* newdir, const char* newpath);
} // namespace fs

ALWAYS_INLINE UNIXOpenFile::~UNIXOpenFile() { FileSystem::Close(this); }
