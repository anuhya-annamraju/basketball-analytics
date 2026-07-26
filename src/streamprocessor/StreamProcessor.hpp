#pragma once
#include "reader/playersnapshotreader.hpp"

class StreamProcessor
{
    public:
    StreamProcessor(std::string filename);
    ~StreamProcessor();
    PlayerSnapshot StreamData();
    private:
    std::string _filenmae;
    //PlayerSnapshotReader _snapshotReader;
};
