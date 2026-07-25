#pragma once
#include <cstdint>
#include <fstream>
#include <string>
#include <optional>

struct PlayerSnapshot {
    uint64_t timestamp_ms;
    int player_id;
    int group_id;
    float x_m;
    float y_m;
};

class PlayerSnapshotReader {
public:
  explicit PlayerSnapshotReader(const std::string &filename);

  std::optional<PlayerSnapshot> readNext();

private:
  std::ifstream file_;
  bool isFirstLine_;
};