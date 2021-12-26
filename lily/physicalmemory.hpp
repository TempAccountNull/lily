#pragma once
#include <windows.h>
#include <functional>

class PhysicalAddress {
private:
    //Maximum ram 1TB
    constexpr static uintptr_t MAXPHYADDRMASK = ~- 0x10000000000;
    /*
    INITIALIZER(
        int Info[4];
        __cpuid(Info, 0x80000008);
        BYTE MAXPHYADDR = Info[0] & 0xff;
        MAXPHYADDRMASK = -1;
        MAXPHYADDRMASK = MAXPHYADDRMASK >> MAXPHYADDR;
        MAXPHYADDRMASK = ~(MAXPHYADDRMASK << MAXPHYADDR);
    )
    */
public:
    uintptr_t PA;
    constexpr uintptr_t Get() const { return PA & MAXPHYADDRMASK; }
    constexpr operator uintptr_t() const { return Get(); }
    constexpr PhysicalAddress& operator =(uintptr_t NewPA) {
        PA = NewPA & MAXPHYADDRMASK;
        return *this;
    }
    constexpr PhysicalAddress(uintptr_t NewPA = 0) { *this = NewPA; }
};

typedef struct _PTE
{
    union
    {
        struct
        {
            uintptr_t Present : 1;              // Must be 1, region invalid if 0.
            uintptr_t ReadWrite : 1;            // If 0, writes not allowed.
            uintptr_t UserSupervisor : 1;       // If 0, user-mode accesses not allowed.
            uintptr_t PageWriteThrough : 1;     // Determines the memory type used to access the memory.
            uintptr_t PageCacheDisable : 1;     // Determines the memory type used to access the memory.
            uintptr_t Accessed : 1;             // If 0, this entry has not been used for translation.
            uintptr_t Dirty : 1;                // If 0, the memory backing this page has not been written to.
            uintptr_t PageAccessType : 1;       // Determines the memory type used to access the memory.
            uintptr_t Global : 1;                // If 1 and the PGE bit of CR4 is set, translations are global.
            uintptr_t Ignored2 : 3;
            uintptr_t PageFrameNumber : 36;     // The page frame number of the backing physical page.
            uintptr_t Reserved : 4;
            uintptr_t Ignored3 : 7;
            uintptr_t ProtectionKey : 4;         // If the PKE bit of CR4 is set, determines the protection key.
            uintptr_t ExecuteDisable : 1;       // If 1, instruction fetches not allowed.
        };
        uintptr_t Value;
    };
} PTE, * PPTE;
static_assert(sizeof(PTE) == sizeof(PVOID), "Size mismatch, only 64-bit supported.");

typedef struct _PDE
{
    union
    {
        struct
        {
            uintptr_t Present : 1;              // Must be 1, region invalid if 0.
            uintptr_t ReadWrite : 1;            // If 0, writes not allowed.
            uintptr_t UserSupervisor : 1;       // If 0, user-mode accesses not allowed.
            uintptr_t PageWriteThrough : 1;     // Determines the memory type used to access PT.
            uintptr_t PageCacheDisable : 1;     // Determines the memory type used to access PT.
            uintptr_t Accessed : 1;             // If 0, this entry has not been used for translation.
            uintptr_t Ignored1 : 1;
            uintptr_t PageSize : 1;             // If 1, this entry maps a 2MB page.
            uintptr_t Ignored2 : 4;
            uintptr_t PageFrameNumber : 36;     // The page frame number of the PT of this PDE.
            uintptr_t Reserved : 4;
            uintptr_t Ignored3 : 11;
            uintptr_t ExecuteDisable : 1;       // If 1, instruction fetches not allowed.
        };
        uintptr_t Value;
    };
} PDE, * PPDE;
static_assert(sizeof(PDE) == sizeof(PVOID), "Size mismatch, only 64-bit supported.");

typedef struct _PDPTE
{
    union
    {
        struct
        {
            uintptr_t Present : 1;              // Must be 1, region invalid if 0.
            uintptr_t ReadWrite : 1;            // If 0, writes not allowed.
            uintptr_t UserSupervisor : 1;       // If 0, user-mode accesses not allowed.
            uintptr_t PageWriteThrough : 1;     // Determines the memory type used to access PD.
            uintptr_t PageCacheDisable : 1;     // Determines the memory type used to access PD.
            uintptr_t Accessed : 1;             // If 0, this entry has not been used for translation.
            uintptr_t Ignored1 : 1;
            uintptr_t PageSize : 1;             // If 1, this entry maps a 1GB page.
            uintptr_t Ignored2 : 4;
            uintptr_t PageFrameNumber : 36;     // The page frame number of the PD of this PDPTE.
            uintptr_t Reserved : 4;
            uintptr_t Ignored3 : 11;
            uintptr_t ExecuteDisable : 1;       // If 1, instruction fetches not allowed.
        };
        uintptr_t Value;
    };
} PDPTE, * PPDPTE;
static_assert(sizeof(PDPTE) == sizeof(PVOID), "Size mismatch, only 64-bit supported.");

typedef struct _PML4E
{
    union
    {
        struct
        {
            uintptr_t Present : 1;              // Must be 1, region invalid if 0.
            uintptr_t ReadWrite : 1;            // If 0, writes not allowed.
            uintptr_t UserSupervisor : 1;       // If 0, user-mode accesses not allowed.
            uintptr_t PageWriteThrough : 1;     // Determines the memory type used to access PDPT.
            uintptr_t PageCacheDisable : 1;     // Determines the memory type used to access PDPT.
            uintptr_t Accessed : 1;             // If 0, this entry has not been used for translation.
            uintptr_t Ignored1 : 1;
            uintptr_t PageSize : 1;             // Must be 0 for PML4E.
            uintptr_t Ignored2 : 4;
            uintptr_t PageFrameNumber : 36;     // The page frame number of the PDPT of this PML4E.
            uintptr_t Reserved : 4;
            uintptr_t Ignored3 : 11;
            uintptr_t ExecuteDisable : 1;       // If 1, instruction fetches not allowed.
        };
        uintptr_t Value;
    };
} PML4E, * PPML4E;
static_assert(sizeof(PML4E) == sizeof(PVOID), "Size mismatch, only 64-bit supported.");

typedef struct _CR3
{
    union
    {
        struct
        {
            uintptr_t Ignored : 3;
            uintptr_t PWT : 1;
            uintptr_t PCD : 1;
            uintptr_t Ignored2 : 7;
            uintptr_t PageFrameNumber : 36;
            uintptr_t Reserved : 16;
        };
        uintptr_t Value;
    };
} CR3, *PCR3;
static_assert(sizeof(_CR3) == sizeof(PVOID), "Size mismatch, only 64-bit supported.");

using TypeWritePhysicalMemoryFunc = std::function<bool(PhysicalAddress PA, const void* Buffer, size_t Size)>;
using TypeReadPhysicalMemoryFunc = std::function<bool(PhysicalAddress PA, void* Buffer, size_t Size)>;

static PhysicalAddress GetPhysicalAddressByPhysicalMemoryAccess(uintptr_t VirtualAddress, CR3 cr3, TypeReadPhysicalMemoryFunc ReadPhysicalMemory) {
    const uintptr_t Address = VirtualAddress;
    const uintptr_t IndexPML4 = (Address >> 39) & 0x1FF;
    const uintptr_t IndexPageDirPtr = (Address >> 30) & 0x1FF;
    const uintptr_t IndexPageDir = (Address >> 21) & 0x1FF;
    const uintptr_t IndexPageTable = (Address >> 12) & 0x1FF;

    PML4E EntryPML4;
    PDPTE EntryPageDirPtr;
    PDE EntryPageDir;
    PTE EntryPageTable;

    //does not check "Present" bit
    if (!ReadPhysicalMemory(cr3.PageFrameNumber * 0x1000 + IndexPML4 * 8, &EntryPML4, 8))
        return 0;

    if (!ReadPhysicalMemory(EntryPML4.PageFrameNumber * 0x1000 + IndexPageDirPtr * 8, &EntryPageDirPtr, 8))
        return 0;

    if (EntryPageDirPtr.PageSize)
        return EntryPageDirPtr.PageFrameNumber * 0x1000 + (Address & 0x3FFFFFFF);

    if (!ReadPhysicalMemory(EntryPageDirPtr.PageFrameNumber * 0x1000 + IndexPageDir * 8, &EntryPageDir, 8))
        return 0;

    if (EntryPageDir.PageSize)
        return EntryPageDir.PageFrameNumber * 0x1000 + (Address & 0x1FFFFF);

    if (!ReadPhysicalMemory(EntryPageDir.PageFrameNumber * 0x1000 + IndexPageTable * 8, &EntryPageTable, 8))
        return 0;

    return EntryPageTable.PageFrameNumber * 0x1000 + (Address & 0xFFF);
}

static bool ReadProcessMemoryByPhysicalMemoryAccess(uintptr_t Address, void* Buffer, size_t Size, CR3 cr3,
    TypeReadPhysicalMemoryFunc ReadPhysicalMemory) {
    while (Size > 0) {
        size_t BlockSize = 0x1000 - (Address & 0xFFF);
        if (BlockSize > Size)
            BlockSize = Size;

        PhysicalAddress PA = GetPhysicalAddressByPhysicalMemoryAccess(Address, cr3, ReadPhysicalMemory);
        if (!PA)
            return false;

        if (!ReadPhysicalMemory(PA, Buffer, BlockSize))
            return false;

        Address += BlockSize;
        Size -= BlockSize;
        Buffer = (void*)(uintptr_t(Buffer) + BlockSize);
    }

    return true;
}

static bool WriteProcessMemoryByPhysicalMemoryAccess(uintptr_t Address, const void* Buffer, size_t Size, CR3 cr3, 
    TypeReadPhysicalMemoryFunc ReadPhysicalMemory, TypeWritePhysicalMemoryFunc WritePhysicalMemory)  {
    while (Size > 0) {
        size_t BlockSize = 0x1000 - (Address & 0xFFF);
        if (BlockSize > Size)
            BlockSize = Size;

        PhysicalAddress PA = GetPhysicalAddressByPhysicalMemoryAccess(Address, cr3, ReadPhysicalMemory);
        if (!PA)
            return false;

        if (!WritePhysicalMemory(PA, Buffer, BlockSize))
            return false;

        Address += BlockSize;
        Size -= BlockSize;
        Buffer = (const void*)(uintptr_t(Buffer) + BlockSize);
    }

    return true;
}