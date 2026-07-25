#include "reader/playersnapshotreader.hpp"
#include "analytics/SnippetAnalyser.hpp"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }
    const std::string filename = argv[1];
    if (!std::filesystem::exists(filename)) {
        std::cerr << "Error: File does not exist: " << filename << '\n';
        return 1;
    }

    PlayerSnapshotReader reader(filename);

    while (auto snapshot = reader.readNext()) {
        std::cout << "Time: " << snapshot->timestamp_ms
                    << " | Player: " << snapshot->player_id
                    << " | Group: " << snapshot->group_id
                    << " | x: " << snapshot->x_m
                    << " | y: " << snapshot->y_m << '\n';
    }

    auto snippet_analyser = std::make_unique<SnippetAnalyser>();

    snippet_analyser->AnalyseSnippet();

    return 0;
}
