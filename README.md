Deviare is a professional hooking engine for instrumenting arbitrary Win32
functions, COM objects, and functions whose symbols are located in program
databases (PDBs). It can intercept unmanaged code in 32-bit and 64-bit
applications. It is implemented as a COM component, so it can be integrated
with all the programming languages which support COM, such as C/C++, VB, C#,
Delphi, and Python.

Several Fortune 500 companies are using Deviare technology for application
virtualization, packaging, and troubleshooting, and for computer security.
Computer science researchers are also using Deviare to conduct malware and
reverse engineering studies. Our blog articles contain a vast quantity of
code samples to get you started easily.

Deviare offers a unique *programmer friendly API* which resolves the
complexities associated with binary instrumentation so that even software
engineers without expertise in the field can use it. Deviare takes care of
code injection, parameter marshalling, and inter-process communication. We
created Deviare API in 2007 and continually improve it. Intercepting
applications is a complex task. We test multiple application environments to
ensure that the end user has a trouble-free experience. Deviare also has a
focus on performance handling thousands of hooks with little footprint.

Code instrumentation is used in several other areas like: tracing and
debugging, sandboxing and browser security, malware analysis, video
conference recording, and gaming.

Download the latest binaries and source code from
[here](https://github.com/nektra/Deviare2/releases).


----------------------
# MINIMUM REQUIREMENTS

Deviare runtime runs on any computer with Microsoft Windows XP or later OS
installed.

To compile Deviare and/or build samples you need:

- Visual Studio 2017.
- .NET Framework 4.
- Windows SDK 8 with all .NET Framework components installed.


**NOTE**: The database (.db) files you'll find in the *BIN* folder are **NOT**
generated when building Deviare. Deviare needs these files to work. If you
create new databases, don't forget to move them to the *BIN* folder.
To create them you must first build the corresponding tools that are
included in the solution. Refer to [these instructions](Source/Database/Generator/README.txt) for further details.


-------------
# BEFORE BUILDING

Since Deviare depends directly on [Deviare-InProc][Deviare InProc github], it is
included as a submodule in this repository. To clone this repository do
`git clone --recursive`. If you already cloned without the `--recursive` option,
you can complete the repository initialization with `git submodule update --init --recursive`.

We recommend always executing `git submodule update --init --recursive` after pulling from this repository.
This ensures the Deviare-InProc submodule is up to date with the HEAD of the Deviare2 branch.

If you downloaded the zipped source package from Github instead, you need to satisfy this dependency manually.
To do so, place the contents of Deviare-InProc inside `Externals/DeviareInProc`.


-------------
# BUILDING DEVIARE

To build Deviare you can either execute the build script (build.bat) in a VS2017
 command prompt or open the solution and build it from within Visual Studio.
 When doing the latter, make sure to build for both x86 and x64 target platforms.
 

-------------
# BUG REPORTS

If you experience something you think might be a bug in Deviare, please
report it in the GitHub repository or write to us in [our contact page][Nektra contact].

Describe what you did, what happened, what kind of computer you have,
which operating system you're using and anything else you think might
be relevant.


-----------------------
# LICENSING INFORMATION

This library has a dual license, a commercial one suitable for closed source
projects and a GPL license that can be used in open source software.

Depending on your needs, you must choose one of them and follow its policies.
A detail of the policies and agreements for each license type are available in
the *LICENSE.COMMERCIAL* and *LICENSE.GPL* files.

For further information please refer to [our licensing options][Deviare homepage] or
contact [Nektra here][Nektra contact].

This library uses a portion of [UDis86 project][UDis86 link], authored, 
copyrighted and maintained by Vivek Thampi. UDis86 is licensed under
the terms of BSD License. For any questions referring to UDis86 contact the
author at vivek[at]sig9[dot]com.

[UDis86 link]: http://udis86.sourceforge.net/
[Nektra contact]: http://www.nektra.com/contact/
[Deviare homepage]: http://www.nektra.com/products/deviare-api-hook-windows/
[Deviare InProc github]: https://github.com/nektra/Deviare-InProc