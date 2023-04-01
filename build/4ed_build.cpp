#include <stl.h>
#include <stl.cpp>

#include "../custom/4coder_base_types.h"
#include "../custom/4coder_version.h"
#include "../custom/4coder_base_types.cpp"

enum class Platform
{
    Windows,
    Linux,
    Mac,
    Count
};

enum class Compiler
{
    MSVC,
    GCC,
    Clang,
    Count
};

enum class Architecture
{
    X64,
    X86,
    ARM64,
    ARM32,
    Count
};

#if defined(AK_STL_WIN32_OS)
#define G_ThisOS Platform::Windows
#elif defined(AK_STL_LINUX_OS)
#define G_ThisOS Platform::Linux
#elif defined(AK_STL_MAC_OS)
#define G_ThisOS Platform::Mac
#else
#error Invalid platform
#endif

#if defined(AK_STL_MSVC_COMPILER)
#define This_Compiler Compiler_MSVC
#elif defined(AK_STL_GCC_COMPILER)
#define This_Compiler Compiler_GCC
#elif defined(AK_STL_CLANG_COMPILER)
#define This_Compiler Compiler_Clang
#else
#error Invalid Compiler
#endif

#if defined(AK_STL_ARCH_X86_64)

#if defined(AK_STL_BITNESS_32)
#define G_ThisArch (u32)(Architecture::X86)
#else
#define G_ThisArch (u32)(Architecture::X64)
#endif

#elif defined(AK_STL_ARCH_ARM)

#if defined(AK_STL_BITNESS_32)
#define G_ThisArch (u32)(Architecture::ARM32)
#else
#define G_ThisArch (u32)(Architecture::ARM64)
#endif

#else
#error Invalid architecture
#endif

using namespace ak_stl;

enum class compiler_flags 
{
    Options= 0x1,
    LIBS = 0x2,
    ICON = 0x4,
    Shared = 0x8,
    Debug = 0x10,
    Optimized = 0x20,
    SUPER = 0x40,
    INTERNAL = 0x80,
    SHIP = 0x100,
};

static const str G_CodePath(stringify(CODE_PATH));
static const str G_CustomPath(stringify(CUSTOM_PATH));

#ifdef AK_STL_WIN32_OS
static const str G_BuildExtension("bat");
#else
static const str G_BuildExtension("sh");
#endif

static const str G_PlatformNames[] = {str("win"), str("linux"), str("mac")};
StaticAssert(ArrayCount(G_PlatformNames) == (u32)Platform::Count);

static const str G_ArchNames[] = {str("x64"), str("x86"), str("arm32"), str("arm64")};
StaticAssert(ArrayCount(G_ArchNames) == (u32)Architecture::Count);

#if defined(AK_STL_MSVC_COMPILER)

static const str G_CompilerCommand("cl");
static const str G_CompilerOptions("-W4 -wd4310 -wd4100 -wd4201 -wd4505 -wd4996 " 
                                   "-wd4127 -wd4510 -wd4512 -wd4610 -wd4390 "    
                                   "-wd4611 -wd4189 -WX -GR- -EHa- -nologo -FC");

#endif

#if defined(AK_STL_WIN32_OS)
static const str G_LibraryExtension("lib");
static const str G_LibraryOptions("user32.lib winmm.lib gdi32.lib opengl32.lib comdlg32.lib userenv.lib ");
#endif

void Build(allocator* Allocator, const span<str8>& IncludeFolders, const span<str8>& Defines, uint32_t Flags)
{
    str_list List(Allocator);
    List.Add(G_CompilerCommand);
    
    if(Flags & (uint32_t)compiler_flags::Options)
        List.Add(G_CompilerOptions);
    
    List.Add("-I%.*s", G_CodePath.Count(), G_CodePath.Data());
    for(const str& IncludeFolder : IncludeFolders)
    {
        List.Add("%.*s/%.*s", G_CodePath.Count(), G_CodePath.Data(), IncludeFolder.Count(), IncludeFolder.Data());
    }
    
    List.Add("%.*s/bin/%.*s/freetype.%.*s", G_CodePath.Count(), G_CodePath.Data(), G_ArchNames[G_ThisArch].Count(), G_ArchNames[G_ThisArch].Data(), G_LibraryExtension.Count(), G_LibraryExtension.Data());
    
    if(Flags & (uint32_t)compiler_flags::Debug)
    {
        List.Add("-Zi");
        List.Add("-DDO_CRAZY_EXPENSIVE_ASSERTS");
    }
    
    if(Flags & (uint32_t)compiler_flags::Optimized)
        List.Add("-O2");
    
    if(Flags & (uint32_t)compiler_flags::Shared)
        List.Add("-LD");
    
    for(const str& Define : Defines)
    {
        List.Add("-D%.*s", Define.Count(), Define.Data());
    }
    
    str LineStr = List.Join(Allocator, (uint32_t)str_join::Space);
    system((const char*)LineStr.Data());
}


void Build_Custom(allocator* Allocator)
{
    str BuildScript = Str_Concat(Allocator, G_CustomPath,
                                 str(Allocator, "/build/build.%.*s", G_BuildExtension.Count(), G_BuildExtension.Data())); 
    
    str BuildCommand;
    if(G_ThisOS == Platform::Windows)
    {
        BuildCommand = Str_Concat(Allocator, str("call "), BuildScript);
    }
    else
    {
        BuildCommand= Str_Concat(Allocator, str("./"), BuildScript);
    }
    
    int code = system((const char*)BuildCommand.Data());
}

int main()
{
    os_allocator OSAllocator;
    arena_allocator Allocator(&OSAllocator);
    Build_Custom(&Allocator);
    Build(&Allocator);
    
    return 0;
}