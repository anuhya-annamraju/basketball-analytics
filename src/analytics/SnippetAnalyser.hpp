#pragma once 
#include <map>
#include <iostream>
#include <cmath>
#include "reader/PlayerSnapshotReader.hpp"
#include "definitions/Definitions.hpp"
#include "utils/PhysicsUtils.hpp"
#include "reader/ISnapshotObserver.hpp"


class SnippetAnalyser : public ISnapshotObserver {
public:
    SnippetAnalyser(){};
    ~SnippetAnalyser(){};
    void DisplayAnalytics();
    
    void AnalyseSnippet(PlayerSnapshot snapshot);   
    void OnSnapshotReceived(const PlayerSnapshot& snapshot) override {                                                                          
        AnalyseSnippet(snapshot);                                                                                                               
    } 
private:
    PlayerSnapshot _currentSnapshot ;
    SnippetAnalytics _snippetAnalytics; 
    void GetOverallAnalytics();
    void CalculateHeatMap(const GroupAnalytics& groupAnalytics);
    void UpdateHeatMapGrid(const Pos& pos, GroupAnalytics& groupAnalytics);
  };
