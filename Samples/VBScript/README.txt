These demos require you to register DeviareCOM.dll and DeviareCOM64.dll (if running under an x64 platform).
Normally, these are generated when building Deviare. Another way to get them is to download the latest release on Github.
To build Deviare, the build script in "Deviare2/Source" can be invoked in a development command prompt for Visual Studio 2015.
Alternatively, opening the solution in that folder and building from there works too.
The built binaries can be found in "Deviare2/bin".

To register the Deviare dlls, you must:

1. Open a command prompt with administrator rights.
2. Navigate to the folder in which the .dll files are stored.
3. Execute "regsvr32 DeviareCOM.dll" and "regsvr32 DeviareCOM64.dll".