#include "SnippetAnalyser.hpp"


void SnippetAnalyser::AnalyseSnippet(PlayerSnapshot snapshot){

    auto& groupAnalytics = _snippetAnalytics.group_analytics[snapshot.group_id];

    auto& playerAnalytics = groupAnalytics.player_analytics[snapshot.player_id];
    
    // calculate distance
    float step = PhysicsUtils::GetCartesianDistance(snapshot.x_m,snapshot.y_m,playerAnalytics.prev_pos.x_m,playerAnalytics.prev_pos.y_m);
    playerAnalytics.distance += step;

    // calculate speed = step_distance/elapsed time
    // calculate acceleration = diff(speed)/elapsed time
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

void SnippetAnalyser::UpdateHeatMapGrid(const Pos& pos, GroupAnalytics& groupAnalytics)
{
    groupAnalytics.heatmap.total_count += 1;
    
    int col = static_cast<int>(((pos.x_m + COURT_LENGTH / 2.0f) / COURT_LENGTH) * GRID_COLS);
    int row = static_cast<int>(((pos.y_m + COURT_WIDTH / 2.0f) / COURT_WIDTH) * GRID_ROWS);

    // Clamp bounds safely
    col = std::max(0, std::min(GRID_COLS - 1, col));
    row = std::max(0, std::min(GRID_ROWS - 1, row));

    groupAnalytics.heatmap.position_distributuion[col][row] += 1;
}