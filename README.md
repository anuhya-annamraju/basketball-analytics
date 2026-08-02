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

Definitions.hpp - defines all the structs used in this app to represent Player, Group and Snipper Analytics
PlayerSnapshotReader - reads each line in the csv file, parses it into PlayerSnapshot
StreamProcessor - holds all the listeners that want to subscribe to the stream of data
ISnapshotObserver - is an interface that any Observer can implement in order to re notified of any incoming stream of data, here that would be SnippetAnalyser.cpp but it can also be implemented by another class in furture if needed. 
SnippetAnalyser - subscribes to snapshot data and performs the analytics algorithms described above 
AnalyticsServer - starts a server on localhost::12345 and visualizes all the analytics
tests/ - holds the unit tests defined to test the functions used for analytics algorithms


## How to run it 
- On Linux/WSL

- On Windows

- On Mac



