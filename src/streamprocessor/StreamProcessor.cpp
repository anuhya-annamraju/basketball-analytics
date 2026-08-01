#include "StreamProcessor.hpp"
#include <filesystem>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

StreamProcessor::StreamProcessor(std::unique_ptr<PlayerSnapshotReader> snapshotReader):_snapshotReader(std::move(snapshotReader)){}

StreamProcessor::~StreamProcessor(){};

void StreamProcessor::Subscribe(SnapshotListener listener)
{
    _listeners.push_back(listener);
}

void StreamProcessor::StreamData()
{                                                                                                                                      
    while (auto snapshot = _snapshotReader->readNext()) {                                                                                       
        if (snapshot.has_value()) {                                                                                                             
            // Notify subscribers automatically                                                                                            
            for (const auto& listener : _listeners) {                                                                                           
                listener(snapshot.value());                                                                                                     
            }                                                                                                                                   
            // Pacing delay (40ms) to simulate real-time playback streaming
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }                                                                                                                                       
    }    
}