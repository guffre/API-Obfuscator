# API-Obfuscator
API Obfuscator is a technique that "attacks" debugging tools, such as Ghidra and IDA. It makes the inital RE more difficult, but more importantly, once the analyst has "figured it out", API Obfuscator simply makes the disassembly a real pain to label.
Within its implementation, I make use of some PRNG techniques to also make dynamic analysis a tad more annoying.
This technique falls underneath the umbrellas of binary obfuscation, anti-debugging, and a touch of anti-disassembly.

# How it works
This is for Windows C programs. It obfuscates any function call by passing it through the "ApiResolver" function. It avoids 1:1 memory addresses by making use of arrays, pointer math, random values, and indexing everything from the start of the structure at compile time.

```
PVOID ApiResolver(unsigned int API);
#define VirtualProtect ((VIRTUALPROTECT)ApiResolver(((UINT*)&apiresolverstruct)[APILENGTH*ApiVirtualProtect + (ApiResolverCounter++%APILENGTH)]))
```
To frustrate dynamic analysis, it makes use of random values. `apiresolverstruct` is actually arrays of run-time random numbers that are guaranteed to be unique. They ultimately map to the real API offset, but using random values makes the assembly more complex (read: mathy) and can frustrate automated tools. This will really shine when obfuscating statically linked code.

To frustrate static analysis, a picture is worth a thousand words. This is the result of running 10 API calls through the obfuscator. The source is found in `sample.c` and compiled with `cl.exe /O2 sample.c`:

![image](https://github.com/user-attachments/assets/973def11-c355-4564-a010-8f6d7d55b8c6)

You will notice that all the memory addresses and function calls are identical, even though there are two different API calls happening. The noticable difference is the `8` and `0xc` contained in the math.

If you are a reverse engineer or a user of RE tools, you can already tell how obnoxious this will be to label. Even once you figure out that `8` correlates to LoadLibrary, the best technique I am aware of would be writing a script to insert comments. Less than ideal, terribly annoying, and the marked-up source will never look nice. Ugh.

# How-To
Its a drop-in header, but it does need to be initialized.

```
#include "apiresolver.h"

// Somewhere:
InitApiResolver();
```
Once included, it will obfuscate function calls to make them difficult to resolve and incredibly annoying to label.
Of note: It uses `#define <API>` to find/replace functions. This is important, for example, with calls like `LoadLibrary` that are already #defined to other calls, like `LoadLibraryA` or `LoadLibraryW`.
This use of define makes it dead-simple to use though, and you can even insert it into library code and I would expect it to work.

It's not limited to Windows API either. You can typedef your own functions and just have all functions go through `ApiResolver` if you wish.

# Why?
Part of my professional job is performing malware RE. Usually malware and their associated crypters/packers/etc will utilize anti-debugging techniques, but ultimately through a combination of static and dynamic analysis you can "figure it out".
This repo is a technique I came up with as a "You know what would be really annoying" thought.
