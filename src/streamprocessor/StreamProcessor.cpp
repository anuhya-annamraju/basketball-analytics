#include "StreamProcessor.hpp"
#include <filesystem>

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
            // Notify all subscribers automatically!                                                                                            
            for (const auto& listener : _listeners) {                                                                                           
                listener(snapshot.value());                                                                                                     
            }                                                                                                                                   
        }                                                                                                                                       
    }    
}