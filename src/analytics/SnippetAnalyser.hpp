#pragma once 
#include <map>
#include <iostream>
#include <cmath>
#include "reader/playersnapshotreader.hpp"

struct Pos
{
    float x_m = 0.0f;
    float y_m = 0.0f;
};

struct PlayerAnalytics {
    float distance = 0.0f;
    float speed = 0.0f;
    float acceleration = 0.0f;
    int sprint_count = 0;
    Pos prevPos;
    float prevSpeed = 0.0f;
    uint64_t prevTimestamp = 0.0f;
};

struct SnippetAnalytics
{
  std::map<std::pair<int, int>, PlayerAnalytics> player_analytics;
  float avg_speed = 0.0f;
  float avg_accel = 0.0f;
};

struct GroupAnalytics
{
    int group_id;
    float avg_speed = 0.0f;
    float avg_accel = 0.0f;
    Pos baseline;
};

class SnippetAnalyser {
public:
    SnippetAnalyser(){};
    ~SnippetAnalyser(){};
    void DisplayAnalytics();
    void AnalyseSnippet(PlayerSnapshot snapshot);

private:
    PlayerSnapshot _currentSnapshot ;
    SnippetAnalytics _snippetAnalytics;
    GroupAnalytics _groupAnalytics;
    void GetOverallAnalytics();
    float GetCartesianDistance(float x1, float y1, float x2, float y2);
  };
