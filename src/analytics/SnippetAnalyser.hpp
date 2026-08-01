#pragma once 
#include <map>
#include <iostream>
#include <cmath>
#include <functional>
#include <vector>
#include "reader/PlayerSnapshotReader.hpp"
#include "definitions/Definitions.hpp"
#include "utils/PhysicsUtils.hpp"
#include "reader/ISnapshotObserver.hpp"

using AnalyticsCallback = std::function<void(const PlayerSnapshot& snapshot, const PlayerAnalytics& analytics)>;

class SnippetAnalyser : public ISnapshotObserver {
public:
    SnippetAnalyser(){};
    ~SnippetAnalyser(){};
    void DisplayAnalytics();
    
    void SubscribeAnalytics(AnalyticsCallback callback) {
        _analyticsCallbacks.push_back(callback);
    }

    void AnalyseSnippet(PlayerSnapshot snapshot);   
    void OnSnapshotReceived(const PlayerSnapshot& snapshot) override {                                                                          
        AnalyseSnippet(snapshot);                                                                                                               
    } 
    void UpdateHeatMapGrid(const Pos& pos, GroupAnalytics& groupAnalytics);

private:
    PlayerSnapshot _currentSnapshot ;
    SnippetAnalytics _snippetAnalytics; 
    std::vector<AnalyticsCallback> _analyticsCallbacks;
    void GetOverallAnalytics();
    void CalculateHeatMap(const GroupAnalytics& groupAnalytics);
  };
