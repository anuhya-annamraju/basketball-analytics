## Basketball Analytics App
This app performs analytics on data samples of basketball players provided in ./snippets/ fodler. The analytics algorithm calculates 

- the instantaneous step distances of a player between two smaples & the total distance travelled by each player. 
- the instantaneous speed of a player between two smaples.
- the instantaneous acceleration of a player between two smaples. 
- the heatmap representing the how often a grid was visited in the basketball court. Here a grid is considered a 1mx1m region.

The instantaneous calculations happening in real time as the data arrives, is visualized in a web-server that is hosted on localhost::12345

![alt text](plots.png)
![alt text](heatmap.png)

## Software Architecture
The app is designed in modular way that focuses on Single Responsibility Pronciple. Each class is responsible for one specific function and they are all located inside their own dedicated folder for reader's convenience.

**Definitions.hpp** - defines all the structs used in this app to represent Player, Group and Snipper Analytics <br> 
**PlayerSnapshotReader** - reads each line in the csv file, parses it into PlayerSnapshot <br> 
**StreamProcessor** - holds all the listeners that want to subscribe to the stream of data <br> 
**ISnapshotObserver** - is an interface that any Observer can implement in order to re notified of any incoming stream of data, here that would be SnippetAnalyser.cpp but it can also be implemented by another class in furture if needed. <br> 
**SnippetAnalyser** - subscribes to snapshot data and performs the analytics algorithms described above <br>  
**AnalyticsServer** - starts a server on localhost::12345 and visualizes all the analytics <br> 
**tests/** - holds the unit tests defined to test the functions used for analytics algorithms <br> 


## How to run it 
- **On Linux/WSL && Mac** - uses makefiles and gcc with cmake
`cmake --preset release` <br>
`cmake --build --preset release`                                                                                                  


After successfully building, navigate to the build folder and launch 
`./basketball-app ../snippets/scippet1.csv`
and launch  http://localhost:12345

**For Unit tests**
`./unit_tests`

- **On Windows**
    
In powershell type 
`cmake --preset msvc-release` <br>
`cmake --build --preset msvc-release`                                                                                                  
                                                                                                             
After successfully building, navigate to the build folder and run
`./basketball-app.exe ../snippets/scippet1.csv`
and launch  http://localhost:12345

**For Unit tests**
`./unit_tests.exe`    

Use Ctrl+c to stop the app