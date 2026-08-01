#include "reader/PlayerSnapshotReader.hpp"
#include "analytics/SnippetAnalyser.hpp"
#include "streamprocessor/StreamProcessor.hpp"
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

    // PlayerSnapshotReader reader(filename);
    // auto snippet_analyser = std::make_unique<SnippetAnalyser>();

    // while (auto snapshot = reader.readNext()) {
    //     std::cout << "Time: " << snapshot->timestamp_ms
    //     << " | Player: " << snapshot->player_id
    //     << " | Group: " << snapshot->group_id
    //     << " | x: " << snapshot->x_m
    //     << " | y: " << snapshot->y_m << '\n';
    //     if(snapshot.has_value())
    //     {
    //         snippet_analyser->AnalyseSnippet(snapshot.value());
    //     }
    // }

    // snippet_analyser->DisplayAnalytics();

                                                                                                                                              
        // 1. Create reader & StreamProcessor                                                                                                       
        auto reader = std::make_unique<PlayerSnapshotReader>(filename);                                                                             
        StreamProcessor processor(std::move(reader));                                                                                               
                                                                                                                                                    
        // 2. Create SnippetAnalyser                                                                                                                
        SnippetAnalyser snippet_analyser;                                                                                                           
                                                                                                                                                    
        // 3. Subscribe SnippetAnalyser to StreamProcessor                                                                                          
        processor.Subscribe([&snippet_analyser](const PlayerSnapshot& snapshot) {
            snippet_analyser.OnSnapshotReceived(snapshot);
        });
  
        // 4. Stream data (triggers analytics automatically)
        processor.StreamData();
  
        // 5. Output results
        snippet_analyser.DisplayAnalytics();


    return 0;
}
