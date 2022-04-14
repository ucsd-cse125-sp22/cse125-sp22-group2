This project is currently configured to run on Microsoft Visual Studio 2012 or newer.
We recommend that you download the latest version of Visual Studio Community to run this project (it's free!).

Upon opening the project, a popup may appear stating that the project was built using an old version of Visual Studio and that the build files need to be updated to a newer version. Please accept the prompt so Visual Studio can update your files automatically.

If you encounter an error stating the build tools do not exist, the instructions on how to fix it shows up in the console window at the bottom. Simply go to the project properties and select the build tools version available in your Visual Studio.

If any library is missing from your system, Visual Studio can automatically download and install them for your project. Go to the Tools menu from the top of the window, and go to "NuGet Package Manager". Select "Manage NuGet Packages for Solution," search for any library your computer is missing under the browse tab, then hit Install.