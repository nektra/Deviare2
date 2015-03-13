Deviare is a professional hooking engine for instrumenting arbitrary Win32
functions, COM objects, and functions which symbols are located in program
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


----------------------
# MINIMUM REQUIREMENTS

Deviare runtime runs on any computer with Microsoft Windows XP or later OS
installed.

To compile Deviare and/or build samples you need:

- Visual Studio 2012 with Update 4 or later.
- .NET Framework 4.
- Windows SDK 8 with all .NET Framework components installed.


**NOTE**: Some of the files you'll find in the *BIN* folder are **NOT**
generated during compilation, such as the database (.db) files. Deviare
needs these files to work. It you create new databases, don't forget to
move them to the *BIN* folder.


-------------
# BUG REPORTS

If you experience something you think might be a bug in Deviare, please
report it in GitHub repository or write us in <http://www.nektra.com/contact/>.

Describe what you did, what happened, what kind of computer you have,
which operating system you're using, and anything else you think might
be relevant.


-----------------------
# LICENSING INFORMATION

This library has a dual license, a commercial one suitable for closed source
projects and a GPL license that can be used in open source software.

Depending on your needs, you must choose one of them and follow its policies.
A detail of the policies and agreements for each license type are available in
the *LICENSE.COMMERCIAL* and *LICENSE.GPL* files.

For further information please refer to <http://www.nektra.com/licensing/> or
contact Nektra here <http://www.nektra.com/contact/>.

This library uses a portion of UDis86 project <http://udis86.sourceforge.net/>,
authored, copyrighted and maintained by Vivek Thampi. UDis86 is licensed under
the terms of BSD License. For any questions referring to UDis86 contact the
author at <vivek[at]sig9[dot]com>.
