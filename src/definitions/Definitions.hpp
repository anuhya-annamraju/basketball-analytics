#pragma once
#include <cmath>
#include <map>

struct Pos
{
    float x_m = 0.0f;
    float y_m = 0.0f;
};

struct PlayerAnalytics {
    int player_id = 0;
    float distance = 0.0f;
    float speed = 0.0f;
    float acceleration = 0.0f;
    int sprint_count = 0;
    Pos prevPos;
    float prevSpeed = 0.0f;
    uint64_t prevTimestamp = 0.0f;
};

struct GroupAnalytics
{
    int group_id;
    float avg_speed = 0.0f;
    float avg_accel = 0.0f;
    Pos baseline;
    std::map<int, PlayerAnalytics> player_analytics;
};

struct SnippetAnalytics
{
  std::map<int, GroupAnalytics> group_analytics;
  float avg_speed = 0.0f;
  float avg_accel = 0.0f;
};
