#include "SnippetAnalyser.hpp"


void SnippetAnalyser::AnalyseSnippet(PlayerSnapshot snapshot){

    auto& groupAnalytics = _snippetAnalytics.group_analytics[snapshot.group_id];

    auto& playerAnalytics = groupAnalytics.player_analytics[snapshot.player_id];
    
    // calculate distance
    float step = PhysicsUtils::GetCartesianDistance(snapshot.x_m,snapshot.y_m,playerAnalytics.prev_pos.x_m,playerAnalytics.prev_pos.y_m);
    playerAnalytics.distance += step;

    // calculate speed = distance/elapsed time
    // calculate acceleration = speed/elapsed time
    auto dt = snapshot.timestamp_ms - playerAnalytics.prev_timestamp;

    float currSpeed = PhysicsUtils::CalculateSpeed(step, dt);
    float accel = PhysicsUtils::CalculateAcceleration(currSpeed,playerAnalytics.speed,dt);
    
    playerAnalytics.prev_pos.x_m = snapshot.x_m;
    playerAnalytics.prev_pos.y_m = snapshot.y_m;
    playerAnalytics.prev_timestamp = snapshot.timestamp_ms;
    playerAnalytics.speed = currSpeed;
    playerAnalytics.acceleration = accel;
    playerAnalytics.player_id = snapshot.player_id;
    groupAnalytics.group_id = snapshot.group_id;

    //From current snapshot calculate heatmap grid index from position
    UpdateHeatMapGrid(playerAnalytics.prev_pos,groupAnalytics);

    // Notify registered analytics listeners (e.g., WebServer telemetry broadcast)
    for (const auto& cb : _analyticsCallbacks) {
        cb(snapshot, playerAnalytics);
    }
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
                          << " Distance: " << player_details.distance  << " (m)"                                                               
                         << " Speed " << player_details.speed << " (m/s)"                                                           
                          "  Accel " << player_details.acceleration << " (m/s2)" << 
                          std::endl;    
        }
    CalculateHeatMap(group.second);
    }
}

void SnippetAnalyser::GetOverallAnalytics(){}

void SnippetAnalyser::CalculateHeatMap(const GroupAnalytics& groupAnalytics){
    // if total no of samples is 100% - then calculate value of each heatmap grid in % 
    // normalise each value to be {0-1} value
    if(groupAnalytics.heatmap.total_count)
    {
        for(int i=0;i<GRID_ROWS;i++)
        {
             for(int j=0;j<GRID_COLS;j++)
             {
                auto val = (float)groupAnalytics.heatmap.position_distributuion[i][j]/groupAnalytics.heatmap.total_count;
               if(val>=0) std::cout << val << ",";
             }
             std::cout << std::endl;
        }
    }
}

void SnippetAnalyser::UpdateHeatMapGrid(const Pos& pos,GroupAnalytics& groupAnalytics)
{
    groupAnalytics.heatmap.total_count += 1;
    
   int i = (pos.x_m>0)? std::ceil(pos.x_m): std::floor(pos.x_m);
   int j = (pos.y_m>0)? std::ceil(pos.y_m): std::floor(pos.y_m);

   i += GRID_COLS/2;
   j += GRID_ROWS/2;
                                                                           
   // std::cout << " pos is " << pos.x_m << "," << pos.y_m << " grid is " << i << "," << j << std::endl;
    // 2. Check bounds                                                                                                          
    if (i >= 0 && i < GRID_COLS && j >= 0 && j < GRID_ROWS) {                                                           
        groupAnalytics.heatmap.position_distributuion[i][j] += 1;                                                                                               
    }                                                                                                                                               
}