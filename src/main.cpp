#include "reader/PlayerSnapshotReader.hpp"
#include "analytics/SnippetAnalyser.hpp"
#include "streamprocessor/StreamProcessor.hpp"
#include "web/AnalyticsServer.hpp"
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

    // 1. Create reader & StreamProcessor
    auto reader = std::make_unique<PlayerSnapshotReader>(filename);
    StreamProcessor processor(std::move(reader));

    // 2. Create SnippetAnalyser
    SnippetAnalyser snippet_analyser;

    // 3. Start Web Server on localhost:12345
    AnalyticsServer webServer(12345);
    webServer.Start();

    // 4. Subscribe Web Server to calculated analytics events
    snippet_analyser.SubscribeAnalytics([&webServer](const PlayerSnapshot& snapshot, const PlayerAnalytics& analytics) {
        webServer.BroadcastAnalytics(snapshot, analytics);
    });

    // 5. Subscribe SnippetAnalyser to StreamProcessor
    processor.Subscribe([&snippet_analyser](const PlayerSnapshot& snapshot) {
        snippet_analyser.OnSnapshotReceived(snapshot);
    });

    std::cout << "\n📊 Streaming analytics data... Open http://localhost:12345 in your browser!\n";

    // 6. Stream data (triggers analytics & web telemetry broadcast automatically)
    processor.StreamData();

    return 0;
}
