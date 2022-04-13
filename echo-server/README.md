# About
This README describes what the steps to set up the echo server. The Boost example is more complete: it allows you to enter input as the client
and have the server echo your response. The Winsock example just initiates one client-server connection.

# Visual Studio
- I used Visual Studio 2022 for the echo server. You can download Visual Studio from here: https://visualstudio.microsoft.com/downloads/

# Winsock
- You'll need the Windows SDK to use Winsock. You can download the SDK and run the installer from here: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
- The client and server programs are taken from these examples:
    - https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code
    - https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code

# Boost/Asio
- You'll need to download Boost and Asio from https://www.boost.org/users/download/

# Running the Echo Server
- Each client / server should be created in its own Visual Studio project.
## Setting up a Visual Studio Project
- I chose the **Empty C++ Project** option when creating a project in Visual Studio.
- In the **Visual Studio Solution Explorer**"
    - Add the client / server file for your project to **Source Files** 
    - Right-click the project solution and select **Properties**.
    - In the **General** tab, ensure that the **C++ Language Standard** is set to **Default (ISO C++14 Standard)**
    - In the **C/C++** tab, add the path to the Boost folder you downloaded in the **Additional Include Directories** field. For example,
    the path might be .\boost_1_78_0 if the directory is in the same level as your project. You need to include this option
    so that the compiler can find Boost when building the programs.
    - For client programs only: in the **Debugging** tab, put **localhost** for the **Command Arguments**
## Running the Server/Client
- Build the solutions in Visual Studio and run the client and server programs in seperate terminals / windows using Ctrl + F5.
