Copyright © 2013-2015 NVIDIA Corporation. All rights reserved.

The GFNLink SDK package includes the following:

### GFNLinkSDK\dll:            
* Win32 (x86) and x64 GFN.dlls The dll needs to be placed next to your game executable in order to enable communication with the GFN backend.

### GFNLinkSDK\include         
* C (.h) and C++ (.hpp) header files your application will need to include in order to access the GFNLink API.

### GFNLinkSDK\lib             
* GFNLink static library files, built with various build configurations. 
* GFNLinkMD.lib - Uses MultiThreadedDll CRT linkage
* GFNLinkMT.lib - Uses MultiThreaded CRT linkage

### GFNLinkSDK\props
* GFNLink props files to include in your application; Adding this will automatically set up include and lib paths.
* Use GFNLinkSDK.props if you're using MultiThreadedDll CRT linkage
* Use GFNLinkSDKMT.props if you're using MultiThreaded CRT linkage

### GFNLinkSDK\stubs
* Stub files you can add to your application in order to meet the minimal integration requirements.
* You should fill in these stubs with calls into your application in order to implement the required functionality.
* Use C Stubs for C style calls
* Use C++ Stubs for C++ style calls
* Use .NET Stubs for C#/.NET

### Simulator
* The simulator acts as the server for GFN applications (e.g. Samples). It simulates the GFN environment and responds to the request made by GFN.dll. This also includes a sample IDM server for Account Federation Samples. 

### Samples 
* SampleCApplication - illustrates a minimal C implementation
* SampleCPPApplication - illustrates a minimal console-based C++ implementation
* SampleCPPGUIApplication - illustrates a C++ GUI-based implementation with Account Federation 

See https://github.com/NVIDIA/GFN-Link/wiki for more details.
