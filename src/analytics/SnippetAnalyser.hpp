#pragma once 
#include <map>
#include <iostream>
#include <cmath>
#include "reader/PlayerSnapshotReader.hpp"
#include "definitions/Definitions.hpp"
#include "utils/PhysicsUtils.hpp"


class SnippetAnalyser {
public:
    SnippetAnalyser(){};
    ~SnippetAnalyser(){};
    void DisplayAnalytics();
    void AnalyseSnippet(PlayerSnapshot snapshot);   

private:
    PlayerSnapshot _currentSnapshot ;
    SnippetAnalytics _snippetAnalytics; 
    void GetOverallAnalytics();
    void CalculateHeatMap(const GroupAnalytics& groupAnalytics);
    void UpdateHeatMapGrid(const Pos& pos, GroupAnalytics& groupAnalytics);
  };
