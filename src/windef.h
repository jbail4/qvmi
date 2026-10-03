#pragma once
#include <stdint.h>

typedef struct _IMAGE_DATA_DIRECTORY
{
    uint32_t VirtualAddress;
    uint32_t Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_IMPORT_DESCRIPTOR
{
    union {
        uint32_t Characteristics;
        uint32_t OriginalFirstThunk;
    } u1;
    uint32_t TimeDateStamp;
    uint32_t ForwarderChain;
    uint32_t Name;
    uint32_t FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR;

typedef struct IMAGE_EXPORT_DIRECTORY
{
    uint32_t Characteristics;
    uint32_t TimeDateStamp;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint32_t Name; // RVA to module name
    uint32_t Base;
    uint32_t NumberOfFunctions;     // total functions (may include gaps)
    uint32_t NumberOfNames;         // number of named functions
    uint32_t AddressOfFunctions;    // RVA to DWORD array of function addresses
    uint32_t AddressOfNames;        // RVA to DWORD array of function name RVAs
    uint32_t AddressOfNameOrdinals; // RVA to WORD array of ordinals
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

typedef struct IMAGE_THUNK_DATA64
{
    union {
        uint64_t ForwarderString;
        uint64_t Function;
        uint64_t Ordinal;
        uint64_t AddressOfData;
    } u1;
} IMAGE_THUNK_DATA64;

typedef struct _IMAGE_IMPORT_BY_NAME
{
    uint16_t Hint;
    char Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;

typedef struct virt_mem_descriptor
{
    uint64_t base;
    uint64_t cr3;
} virt_mem_descriptor;

typedef struct LIST_ENTRY
{
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

typedef struct UNICODE_STRING
{
    uint16_t Length;
    uint16_t MaximumLength;
    uint16_t *Buffer; // wchar_t is 4 byte on linux, 2 byte on windows.
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct RTL_USER_PROCESS_PARAMETERS
{
    uint8_t pad1[0x60];
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
} PRTL_USER_PROCESS_PARAMETERS, RTL_USER_PROCESS_PARAMETERS;

typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    uint64_t DllBase;
    uint64_t EntryPoint;
    uint64_t SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
} LDR_DATA_TABLE_ENTRY;

typedef struct _PEB_LDR_DATA
{
    uint32_t Length;
    uint8_t Initialized;
    uint8_t _padding1[3];
    uint64_t SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    uint64_t EntryInProgress;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct PEB
{
    uint8_t pad[0x10];
    uint64_t ImageBaseAddress;
    PEB_LDR_DATA *Ldr;
    RTL_USER_PROCESS_PARAMETERS *ProcessParameters;
} PEB;

typedef struct KPROCESS
{
    uint8_t pad1[0x28];
    uint64_t DirectoryTableBase;
    uint8_t pad2[0x198];
} KPROCESS;

typedef struct EPROCESS
{
    KPROCESS Pcb;
    uint8_t pad1[0x8];
    uint64_t UniqueProcessId;
    LIST_ENTRY ActiveProcessLinks;
    uint8_t pad2[0xF8];
    PEB *Peb;
    uint8_t pad3[0x50];
    char ImageFileName[15];
} EPROCESS;