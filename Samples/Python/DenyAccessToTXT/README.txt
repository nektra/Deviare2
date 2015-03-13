This demo requires you to:

1. Install Python 2.7.3
2. Register DeviareCOM.dll and DeviareCOM64.dll (if running under an x64 platform).
3. Download and install Python Win32 Extensions

===================================================================================

1. You can download a Python 2.7.3 installer from here:

	http://www.python.org/download/

===================================================================================

2. To register Deviare .dll files follow these steps:

	- Open a command prompt with administrator rights.
	- Navigate to the folder in which the .dll files are stored.
	- Execute "regsvr32 DeviareCOM.dll" and "regsvr32 DeviareCOM64.dll".
	
===================================================================================

3. You can download a Python Win32 Extensions installer from here:

	http://sourceforge.net/projects/pywin32/files/pywin32

NOTE: Be careful to download the appropriate version for your platform and Python version.

NOTE 2: If you get a message stating that you haven't installed Python in your system yet (but you did and you are completely sure that you downloaded the correct Python Win32 Extensions installer) you can copy the registry folder HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Python to HKEY_LOCAL_MACHINE\SOFTWARE\Python (or viceversa). To do this, you should:

- Open the registry editor (start -> run... -> "regedit").
- Find the folder you want to copy.
- Right click on the folder and select "Export". This will allow you to save a file with the registry folder content.
- Edit the file you just created and change all the paths in the file accordingly to where you want to move the folder.
- Save the file and then double-click it. A message box should appear prompting if you want to add information to the registry. Click "yes" and you are done.

Once you've done this, the Python Win32 Extensions installer should recognize your Python installation.

NOTE 3: If you still have problems to install this demo, please visit our forum to get further assistance. http://forum.nektra.com/forum/