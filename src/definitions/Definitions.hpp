#pragma once
#include <cmath>
#include <map>

constexpr float COURT_LENGTH = 28.7f;                                                                                           
constexpr float COURT_WIDTH  = 15.2f;                                                                                           
                                                                                                                                    
constexpr int GRID_COLS = (int)(COURT_LENGTH); // 28                                                      
constexpr int GRID_ROWS = (int)(COURT_WIDTH);  // 15     

struct Pos
{
    float x_m = 0.0f;
    float y_m = 0.0f;
};

struct HeatMap
{
    int total_count = 0;
    int position_distributuion[GRID_COLS][GRID_ROWS]={0};
};

struct PlayerAnalytics {
    int player_id = 0;
    float distance = 0.0f;
    float speed = 0.0f;
    float acceleration = 0.0f;
    int sprint_count = 0;
    Pos prev_pos;
    float prev_speed = 0.0f;
    uint64_t prev_timestamp = 0.0f;
};

struct GroupAnalytics
{
    int group_id;
    float avg_speed = 0.0f;
    float avg_accel = 0.0f;
    Pos baseline;
    std::map<int, PlayerAnalytics> player_analytics;
    HeatMap heatmap;
};

struct SnippetAnalytics
{
  std::map<int, GroupAnalytics> group_analytics;
  float avg_speed = 0.0f;
  float avg_accel = 0.0f;
};
