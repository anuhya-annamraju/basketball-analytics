#include "playersnapshotreader.hpp"

#include <fstream>
#include <sstream>

std::optional<PlayerSnapshot> PlayerSnapshotReader::readNext() {
  std::string line;

  while (std::getline(file_, line)) {
    if (isFirstLine_) {
      isFirstLine_ = false;
      continue; // Skip header
    }

    std::istringstream ss(line);
    std::string token;
    PlayerSnapshot snapshot;

    if (!std::getline(ss, token, ','))
      return std::nullopt;
    snapshot.timestamp_ms = std::stoull(token);

    if (!std::getline(ss, token, ','))
      return std::nullopt;
    snapshot.player_id = std::stoi(token);

    if (!std::getline(ss, token, ','))
      return std::nullopt;
    snapshot.group_id = std::stoi(token);

    if (!std::getline(ss, token, ','))
      return std::nullopt;
    snapshot.x_m = std::stof(token);

    if (!std::getline(ss, token, ','))
      return std::nullopt;
    snapshot.y_m = std::stof(token);

    return snapshot;
  }

  return std::nullopt; // End of file
}

PlayerSnapshotReader::PlayerSnapshotReader(const std::string &filename)
    : file_(filename), isFirstLine_(true) {
  if (!file_.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
  }
}
