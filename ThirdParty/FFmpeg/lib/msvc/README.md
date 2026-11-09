# MSVC import libraries

`x86/` contains MSVC-format import libraries generated from the exports of the existing DLLs in `bin/x86/`. FFmpeg DLLs and the original import libraries remain unchanged.

The original GNU import libraries cause MSVC x86 Release linking to fail with LNK2026/LNK1281 when SAFESEH is enabled. Regenerating import libraries fixes the linker compatibility without disabling SAFESEH, ASLR or DEP. This does not change the security properties of the existing third-party DLLs.

After replacing the bundled DLLs, regenerate the import libraries using Visual Studio `lib.exe /def:<name>.def /machine:x86 /out:<name>.lib`. The matching export definition files are included in `x86/`; update them if the DLL exports change.
