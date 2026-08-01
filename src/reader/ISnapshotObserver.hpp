#pragma once
#include "PlayerSnapshotReader.hpp"


class ISnapshotObserver
{
    public:
        virtual ~ISnapshotObserver() = default; 
        virtual void OnSnapshotReceived(const PlayerSnapshot& snapshot) = 0;
};