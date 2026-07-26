#include "SnippetAnalyser.hpp"

float SnippetAnalyser:: GetCartesianDistance(float x1, float y1, float x2, float y2)
{                                      
    float dx = x2 - x1;                                                                                              
    float dy = y2 - y1;                                                                                              
    float distance = std::hypot(dx, dy);  
    return distance;
}

void SnippetAnalyser::AnalyseSnippet(PlayerSnapshot snapshot){

    auto& groupAnalytics = _snippetAnalytics.group_analytics[snapshot.group_id];

    auto& playerAnalytics = groupAnalytics.player_analytics[snapshot.player_id];
    
    // calculate distance
    float step = GetCartesianDistance(snapshot.x_m,snapshot.y_m,playerAnalytics.prevPos.x_m,playerAnalytics.prevPos.y_m);
    playerAnalytics.distance += step;

    // calculate speed = distance/elapsed time
    // calculate acceleration = speed/elapsed time
    auto dt = snapshot.timestamp_ms - playerAnalytics.prevTimestamp;
    float currSpeed=0.0f;
    float accel= 0.0f;
    if(dt>0 && dt<=50)
    {
         currSpeed = step/(dt*1e-3);
         accel = currSpeed - playerAnalytics.speed/(dt*1e-3); 
    }

    playerAnalytics.prevPos.x_m = snapshot.x_m;
    playerAnalytics.prevPos.y_m = snapshot.y_m;
    playerAnalytics.prevTimestamp = snapshot.timestamp_ms;
    playerAnalytics.speed = currSpeed;
    playerAnalytics.acceleration = accel;
    playerAnalytics.player_id = snapshot.player_id;
    groupAnalytics.group_id = snapshot.group_id;
}

void SnippetAnalyser::DisplayAnalytics()
 {

    for(auto group : _snippetAnalytics.group_analytics)
    {
        std::cout << " group " << group.first << std::endl;
        int grpId = group.first;

        for(auto player : group.second.player_analytics)
        {
            auto player_details = player.second;
             std::cout << "[Group: " << grpId                                                                        
                          << ", Player Id : " << player.first << "]"                                                              
                          << " Distance: " << player_details.distance                                                                 
                         << " Speed " << player_details.speed <<                                                            
                          "  Accel " << player_details.acceleration <<
                          std::endl;    
        }
    }
    // for (auto player: _snippetAnalytics.player_analytics)
    // {
    //     std::cout << "[Group: " << player.first.first                                                                        
    //                       << ", Player: " << player.first.second << "]"                                                              
    //                       << " Distance: " << player.second.distance                                                                 
    //                      << " Speed " << player.second.speed <<                                                            
    //                       "  Accel " << player.second.acceleration <<
    //                       std::endl;    
    // }
}

void SnippetAnalyser::GetOverallAnalytics()
{
   
}