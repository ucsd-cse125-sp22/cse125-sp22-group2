# Boost
- Download Boost Version 1.78 from https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/

# Running the Client / Server
- Each client / server should be created in its own Visual Studio project with its own solution file.
## Setting up a Visual Studio Project
- I chose the **Empty C++ Project** option when creating a project in Visual Studio.
- In the **Visual Studio Solution Explorer**"
    - Add the client / server file for your project to **Source Files** (e.g. Client.cpp, Server.cpp) 
    - Right-click the project solution and select **Properties**.
    - In the **General** tab, ensure that the **C++ Language Standard** is set to **Default (ISO C++14 Standard)**
    - In the **C/C++** tab, add the path to the directory containing the Boost folder you downloaded in the **Additional Include Directories** field. 
    - For client programs only: in the **Debugging** tab, put **localhost** for the **Command Arguments**
## Running the Server/Client
- Build the solutions in Visual Studio
-  Run the server program before running a client program.
