# API-Obfuscator
This drop-in header will obfuscate function calls to make them difficult to resolve and incredibly annoying to label

# Background
Part of my professional job is performing malware RE. Usually malware and their associated crypters/packers/etc will utilize anti-debugging techniques, but ultimately through a combination of static and dynamic analysis you can "figure it out".
This repo is a technique I came up with as a "Man it would be really annoying if they did this..."

API Obfuscator is effectively a technique that "attacks" the debugging tools, such as Ghidra and IDA. It does make the inital RE more difficult, but more importantly, once the analyst has "figured it out", API Obfuscator simply makes the disassembly a pain in rear to label.

# Screenshots

This is the result of running 10 API calls through the obfuscator. This is a drop-in header, so using it is dead simple.

The source is found in `sample.c` in this repo, and this is what it looks like in Ghidra compiled as `cl.exe /O2 sample.c`:

![image](https://github.com/user-attachments/assets/973def11-c355-4564-a010-8f6d7d55b8c6)

Here is the gem: You will notice that all the memory addresses and function calls are identical, even though there are two different API calls happening. The noticable difference is the `8` and `0xc` contained in the math.

If you are a reverse engineer or a user of RE tools, you can already tell how obnoxious this will be to label. Even once you figure out that `8` correlates to LoadLibrary, the best technique I am aware of would be writing a script to insert comments. Less than ideal, terribly annoying, and the marked-up source will never look nice. Ugh. You're welcome world.
