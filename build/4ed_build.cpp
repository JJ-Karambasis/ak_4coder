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

char *platform_names[] = {
    "win",
    "linux",
    "mac",
};

StaticAssert(ArrayCount(platform_names) == (u32)Platform::Count);

enum class Compiler
{
    MSVC,
    GCC,
    Clang,
    Count
};

char *compiler_names[] = {
    "cl",
    "gcc",
    "clang",
};

StaticAssert(ArrayCount(compiler_names) == (u32)Compiler::Count);

enum class Architecture
{
    X64,
    X86,
    ARM64,
    ARM32,
    Count
};

char *arch_names[] = {
    "x64",
    "x86",
    "arm64",
    "arm32"
};

StaticAssert(ArrayCount(arch_names) == (u32)Architecture::Count);

#if defined(AK_STL_WIN32_OS)
#define This_OS Platform_Windows
#elif defined(AK_STL_LINUX_OS)
#define This_OS Platform_Linux
#elif defined(AK_STL_MAC_OS)
#define This_OS Platform_Mac
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
#define This_Arch Arch_X86
#else
#define This_Arch Arch_X64
#endif

#elif defined(AK_STL_ARCH_ARM)

#if defined(AK_STL_BITNESS_32)
#define This_Arch Arch_ARM32
#else
#define This_Arch Arch_ARM64
#endif

#else
#error Invalid architecture
#endif

int main()
{
    return 0;
}