#pragma once 
#include <map>
#include <iostream>
#include <cmath>
#include "reader/playersnapshotreader.hpp"


struct PlayerAnalytics {
    float distance = 0.0f;
    float speed = 0.0f;
    float acceleration = 0.0f;

    float intermediateDistance = 0.0f;
};

struct SnippetAnalytics
{
  std::map<std::pair<int, int>, PlayerAnalytics> player_analytics;
};

class SnippetAnalyser {
public:
SnippetAnalyser(){
    std::cout << "SnippetAnalyser object created." << std::endl;
}
~SnippetAnalyser(){}

void AnalyseSnippet(PlayerSnapshot snapshot){
    _currentSnapshot = snapshot;
    CalculateDistance();
}

private:
    PlayerSnapshot _currentSnapshot;
    SnippetAnalytics _snippetAnalytics;
     void CalculateDistance();
     float GetCartesianDistance(float x1, float y1, float x2, float y2);
  };
