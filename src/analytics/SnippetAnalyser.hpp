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
    float _prevPosX = 0.0f;
    float _prevPosY = 0.0f;
    float _prevSpeed = 0.0f;
    uint64_t _prevTimestamp = 0.0f;
};

struct SnippetAnalytics
{
  std::map<std::pair<int, int>, PlayerAnalytics> player_analytics;
};

class SnippetAnalyser {
public:
    SnippetAnalyser();
    ~SnippetAnalyser();
    void DisplayAnalytics();
    void AnalyseSnippet(PlayerSnapshot snapshot);

private:
    PlayerSnapshot _currentSnapshot ;
    SnippetAnalytics _snippetAnalytics;
    float GetCartesianDistance(float x1, float y1, float x2, float y2);
  };
