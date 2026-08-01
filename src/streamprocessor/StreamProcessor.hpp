#pragma once
#include "reader/PlayerSnapshotReader.hpp"
#include "analytics/SnippetAnalyser.hpp"

using SnapshotListener = std::function<void(const PlayerSnapshot&)>;                                                                            
class StreamProcessor
{
    public:
    StreamProcessor(std::unique_ptr<PlayerSnapshotReader> snapshotReader);
    ~StreamProcessor();
    void Subscribe(SnapshotListener listener); 
    void StreamData();
    private:
    std::string _filename;
    std::unique_ptr<PlayerSnapshotReader> _snapshotReader;
    std::vector<SnapshotListener> _listeners;
};
