#pragma once
#include <httplib.h>
#include <string>
#include <vector>
#include <mutex>
#include <set>
#include <thread>
#include <atomic>
#include "reader/PlayerSnapshotReader.hpp"
#include "definitions/Definitions.hpp"

class AnalyticsServer {
public:
    explicit AnalyticsServer(int port = 12345);
    ~AnalyticsServer();

    void Start();
    void Stop();

    void BroadcastAnalytics(const PlayerSnapshot& snapshot, const PlayerAnalytics& analytics);

private:
    int _port;
    httplib::Server _svr;
    std::thread _serverThread;
    std::atomic<bool> _running{false};

    std::mutex _mutex;
    std::set<httplib::DataSink*> _sinks;

    static const char* GetIndexHtml();
};
