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
    float _prevTimestamp = 0.0f;
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

void DisplayAnalytics()
{
    for (auto player: _snippetAnalytics.player_analytics)
    {
        std::cout << "[Group: " << player.first.first                                                                        
                          << ", Player: " << player.first.second << "]"                                                              
                          << " Distance: " << player.second.distance                                                                 
                         << std::endl;    
    }
}

void AnalyseSnippet(PlayerSnapshot snapshot){

    int grpId = snapshot.group_id;
    int playerId = snapshot.player_id;

    auto& playerAnalytics = _snippetAnalytics.player_analytics[{snapshot.group_id,snapshot.player_id}];
    
    // calculate distance
   playerAnalytics.distance += GetCartesianDistance(snapshot.x_m,snapshot.y_m,playerAnalytics._prevPosX,playerAnalytics._prevPosY);
    
    // calculate speed = distance/elapsed time

    
    // calculate acceleration = speed/elapsed time
    playerAnalytics._prevPosX=snapshot.x_m;
    playerAnalytics._prevPosY=snapshot.y_m;
}
private:
    PlayerSnapshot _currentSnapshot ;
    SnippetAnalytics _snippetAnalytics;
     void CalculateDistance();
     float GetCartesianDistance(float x1, float y1, float x2, float y2);
     float GetCartesianSpeed(float distance, float dt);
  };
