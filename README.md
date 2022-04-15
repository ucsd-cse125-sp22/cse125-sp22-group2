# Team Big Skrrt Studios
Our game is Skrrt Skirt: a 3-versus-1 game of reverse tag in which one player possesses a crown and three other players try to steal that crown for themselves. 
The goal of the game is to possess the crown for the most amount of time.

[Link](https://cse125.ucsd.edu/2022/cse125g2/wordpress/index.php/home/) to our website

# Library Downloads
This section lists all of the libraries required to build the project.
- [GLM](https://sourceforge.net/projects/glm.mirror/)
- [Boost](https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/)

# Visual Studio Configuration
## Additional Include Directories
In the Solution Explorer of your Visual Studio project, add the following directories
as **Additional Include Directories** (the specific filesystem path depends on where your library is
relative to your Visual Studio project):
- glm
- boost_1_78_0

One way you can include all of the directories easily is to put all of the libraries within a single container
folder. Then you can specify the name of this container folder for the **Additional Include Directories** and you
should be able to include all of the libraries as if they were contained in the same directory as your project 
(i.e. you can do **include <glm/glm.hpp>**) 
