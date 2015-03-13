Database generator for Deviare 2.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Installation
---------------

Create an empty folder and unzip the contents of the Deviare2_DbGenerator.zip

After decompression you will find four header files named preprocessed##X.h, where

    ## is 32 or 64 depeding on the target platform
    X is "A" or "W" meaning Ansi and Wide (unicode) versions

Most structures in Windows headers are well defined and the corresponding Ansi and Unicode versions are included.
For e.g.: WNDCLASSEXA and WNDCLASSEXW but a few ones, like INTERNET_CERTIFICATE_INFO has problems. Some of its
members are of type LPTSTR so if the ansi version, it is processed as LPSTR, else as LPWSTR.

Because most applications are unicode we use the "W" version of the files by default but you can change it by
editing the build_db32.bat and build_db64.bat files.


2. Adding your API
------------------

In order to add your structures and api's, simply edit the preprocessed32W.h and the preprocessed64W.h (or the
ansi version if you want to use them) and add the definitions at the end of the file.

For e.g.:

    int __stdcall MyApi(DWORD *param1, int param2);

NOTE: The header file is already preprocessed so you may find that there are no "#define" directives. Most of the
      Windows types like DWORD are typedefs so they are available for usage in your api's but if you find that a
      type definition is not available, you can add it to the header file.


3. Rebuilding the database
--------------------------

After editing the header files execute the build_db32.bat batch file to build the 32-bit database and use the
build_db64.bat batch file to build the 64.bit version.

The whole process may take several minutes to complete. The database and a log file will be created. If compilation
failed for any reason, please review the edited header file and inspect the .log file to detect where is the error.

NOTE: The 64-bit builder only runs on a x64 compatible computer with an x64 operating system installed.

In 64-bit o.s., the x64 version of Deviare is capable of hook 32 bit processes too. Remember to update both databases
if changes are made.

After a succesfull compilation, copy the ".db" files to the folder where you installed Deviare.
