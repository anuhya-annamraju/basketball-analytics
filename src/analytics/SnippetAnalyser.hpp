#pragma once 
#include <map>
#include <iostream>
#include <cmath>
#include "reader/PlayerSnapshotReader.hpp"
#include "definitions/Definitions.hpp"


class SnippetAnalyser {
public:
    SnippetAnalyser(){};
    ~SnippetAnalyser(){};
    void DisplayAnalytics();
    void AnalyseSnippet(PlayerSnapshot snapshot);
    float GetCartesianDistance(float x1, float y1, float x2, float y2);

private:
    PlayerSnapshot _currentSnapshot ;
    SnippetAnalytics _snippetAnalytics;
    void GetOverallAnalytics();
    void CalculateHeatMap(const GroupAnalytics& groupAnalytics);
    void UpdateHeatMapGrid(const Pos& pos, GroupAnalytics& groupAnalytics);
  };
