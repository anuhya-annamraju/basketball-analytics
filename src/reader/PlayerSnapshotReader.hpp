#pragma once
#include <cstdint>
#include <fstream>
#include <string>
#include <optional>
#include "definitions/Definitions.hpp"



class PlayerSnapshotReader {
public:
  PlayerSnapshotReader();
  explicit PlayerSnapshotReader(const std::string &filename);
  std::optional<PlayerSnapshot> readNext();

private:
  std::ifstream file_;
  bool isFirstLine_;
};