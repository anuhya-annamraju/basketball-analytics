#include <doctest/doctest.h>
#include "analytics/SnippetAnalyser.hpp"
#include <iostream>
#include <iomanip>

static void PrintHeatMapGrid(const GroupAnalytics& groupAnalytics, const std::string& title) {
    std::cout << "\n========================================================================================\n";
    std::cout << " 📊 " << title << " (Total Visits: " << groupAnalytics.heatmap.total_count << ")\n";
    std::cout << "========================================================================================\n";
    std::cout << " Row (Y) |  Cols 0..13 (Left Half)        |  Cols 14..27 (Right Half)\n";
    std::cout << " --------+--------------------------------+---------------------------------------------\n";
    
    // Print from top row (14) down to bottom row (0)
    for (int r = GRID_ROWS - 1; r >= 0; --r) {
        std::cout << "   " << std::setw(2) << r << "    | ";
        for (int c = 0; c < GRID_COLS; ++c) {
            if (c == GRID_COLS / 2) {
                std::cout << "| "; // Center Y-axis separator line
            }
            int count = groupAnalytics.heatmap.position_distributuion[c][r];
            if (count > 0) {
                std::cout << "1 "; // Visited cell
            } else {
                std::cout << ". "; // Unvisited cell
            }
        }
        std::cout << '\n';
        if (r == GRID_ROWS / 2) {
            std::cout << " --------+--------------------------------+---------------------------------------------\n";
        }
    }
    std::cout << "========================================================================================\n\n";
}

TEST_SUITE("HeatMap Grid Analytics") {

    TEST_CASE("Scenario 1: All grids visited once -> values are identical across 2D array") {
        SnippetAnalyser analyser;
        GroupAnalytics groupAnalytics{};

        const int totalCells = GRID_COLS * GRID_ROWS; // 420

        for (int col = 0; col < GRID_COLS; ++col) {
            for (int row = 0; row < GRID_ROWS; ++row) {
                float x_m = ((col + 0.5f) / GRID_COLS) * COURT_LENGTH - (COURT_LENGTH / 2.0f);
                float y_m = ((row + 0.5f) / GRID_ROWS) * COURT_WIDTH - (COURT_WIDTH / 2.0f);
                
                Pos pos{x_m, y_m};
                analyser.UpdateHeatMapGrid(pos, groupAnalytics);
            }
        }

        PrintHeatMapGrid(groupAnalytics, "SCENARIO 1: ALL GRIDS VISITED ONCE (Uniform Grid)");

        CHECK(groupAnalytics.heatmap.total_count == totalCells);

        const float expectedNormalizedVal = 1.0f / static_cast<float>(totalCells);

        for (int col = 0; col < GRID_COLS; ++col) {
            for (int row = 0; row < GRID_ROWS; ++row) {
                int rawCount = groupAnalytics.heatmap.position_distributuion[col][row];
                CHECK(rawCount == 1);

                float normalizedVal = static_cast<float>(rawCount) / groupAnalytics.heatmap.total_count;
                CHECK(normalizedVal == doctest::Approx(expectedNormalizedVal));
            }
        }
    }

    TEST_CASE("Scenario 2: Only Top-Right Quadrant visited once -> remaining quadrants 0, Top-Right values all equal") {
        SnippetAnalyser analyser;
        GroupAnalytics groupAnalytics{};

        const int startCol = GRID_COLS / 2; // 14
        const int startRow = GRID_ROWS / 2; // 7

        int expectedVisits = 0;

        for (int col = startCol; col < GRID_COLS; ++col) {
            for (int row = startRow; row < GRID_ROWS; ++row) {
                float x_m = ((col + 0.5f) / GRID_COLS) * COURT_LENGTH - (COURT_LENGTH / 2.0f);
                float y_m = ((row + 0.5f) / GRID_ROWS) * COURT_WIDTH - (COURT_WIDTH / 2.0f);

                Pos pos{x_m, y_m};
                analyser.UpdateHeatMapGrid(pos, groupAnalytics);
                expectedVisits++;
            }
        }

        PrintHeatMapGrid(groupAnalytics, "SCENARIO 2: ONLY TOP-RIGHT QUADRANT VISITED ONCE");

        CHECK(groupAnalytics.heatmap.total_count == expectedVisits);

        const float expectedNormVal = 1.0f / static_cast<float>(expectedVisits);

        for (int col = 0; col < GRID_COLS; ++col) {
            for (int row = 0; row < GRID_ROWS; ++row) {
                int rawCount = groupAnalytics.heatmap.position_distributuion[col][row];
                float normalizedVal = static_cast<float>(rawCount) / groupAnalytics.heatmap.total_count;

                if (col >= startCol && row >= startRow) {
                    CHECK(rawCount == 1);
                    CHECK(normalizedVal == doctest::Approx(expectedNormVal));
                } else {
                    CHECK(rawCount == 0);
                    CHECK(normalizedVal == doctest::Approx(0.0f));
                }
            }
        }
    }

    TEST_CASE("Scenario 3: Only Top-Left Quadrant visited once -> remaining quadrants 0, Top-Left values all equal") {
        SnippetAnalyser analyser;
        GroupAnalytics groupAnalytics{};

        const int endCol = GRID_COLS / 2;   // 14
        const int startRow = GRID_ROWS / 2; // 7

        int expectedVisits = 0;

        for (int col = 0; col < endCol; ++col) {
            for (int row = startRow; row < GRID_ROWS; ++row) {
                float x_m = ((col + 0.5f) / GRID_COLS) * COURT_LENGTH - (COURT_LENGTH / 2.0f);
                float y_m = ((row + 0.5f) / GRID_ROWS) * COURT_WIDTH - (COURT_WIDTH / 2.0f);

                Pos pos{x_m, y_m};
                analyser.UpdateHeatMapGrid(pos, groupAnalytics);
                expectedVisits++;
            }
        }

        PrintHeatMapGrid(groupAnalytics, "SCENARIO 3: ONLY TOP-LEFT QUADRANT VISITED ONCE");

        CHECK(groupAnalytics.heatmap.total_count == expectedVisits);
        const float expectedNormVal = 1.0f / static_cast<float>(expectedVisits);

        for (int col = 0; col < GRID_COLS; ++col) {
            for (int row = 0; row < GRID_ROWS; ++row) {
                int rawCount = groupAnalytics.heatmap.position_distributuion[col][row];
                float normalizedVal = static_cast<float>(rawCount) / groupAnalytics.heatmap.total_count;

                if (col < endCol && row >= startRow) {
                    CHECK(rawCount == 1);
                    CHECK(normalizedVal == doctest::Approx(expectedNormVal));
                } else {
                    CHECK(rawCount == 0);
                    CHECK(normalizedVal == doctest::Approx(0.0f));
                }
            }
        }
    }

    TEST_CASE("Scenario 4: Only Bottom-Left Quadrant visited once -> remaining quadrants 0, Bottom-Left values all equal") {
        SnippetAnalyser analyser;
        GroupAnalytics groupAnalytics{};

        const int endCol = GRID_COLS / 2; // 14
        const int endRow = GRID_ROWS / 2; // 7

        int expectedVisits = 0;

        for (int col = 0; col < endCol; ++col) {
            for (int row = 0; row < endRow; ++row) {
                float x_m = ((col + 0.5f) / GRID_COLS) * COURT_LENGTH - (COURT_LENGTH / 2.0f);
                float y_m = ((row + 0.5f) / GRID_ROWS) * COURT_WIDTH - (COURT_WIDTH / 2.0f);

                Pos pos{x_m, y_m};
                analyser.UpdateHeatMapGrid(pos, groupAnalytics);
                expectedVisits++;
            }
        }

        PrintHeatMapGrid(groupAnalytics, "SCENARIO 4: ONLY BOTTOM-LEFT QUADRANT VISITED ONCE");

        CHECK(groupAnalytics.heatmap.total_count == expectedVisits);
        const float expectedNormVal = 1.0f / static_cast<float>(expectedVisits);

        for (int col = 0; col < GRID_COLS; ++col) {
            for (int row = 0; row < GRID_ROWS; ++row) {
                int rawCount = groupAnalytics.heatmap.position_distributuion[col][row];
                float normalizedVal = static_cast<float>(rawCount) / groupAnalytics.heatmap.total_count;

                if (col < endCol && row < endRow) {
                    CHECK(rawCount == 1);
                    CHECK(normalizedVal == doctest::Approx(expectedNormVal));
                } else {
                    CHECK(rawCount == 0);
                    CHECK(normalizedVal == doctest::Approx(0.0f));
                }
            }
        }
    }

    TEST_CASE("Scenario 5: Only Bottom-Right Quadrant visited once -> remaining quadrants 0, Bottom-Right values all equal") {
        SnippetAnalyser analyser;
        GroupAnalytics groupAnalytics{};

        const int startCol = GRID_COLS / 2; // 14
        const int endRow = GRID_ROWS / 2;   // 7

        int expectedVisits = 0;

        for (int col = startCol; col < GRID_COLS; ++col) {
            for (int row = 0; row < endRow; ++row) {
                float x_m = ((col + 0.5f) / GRID_COLS) * COURT_LENGTH - (COURT_LENGTH / 2.0f);
                float y_m = ((row + 0.5f) / GRID_ROWS) * COURT_WIDTH - (COURT_WIDTH / 2.0f);

                Pos pos{x_m, y_m};
                analyser.UpdateHeatMapGrid(pos, groupAnalytics);
                expectedVisits++;
            }
        }

        PrintHeatMapGrid(groupAnalytics, "SCENARIO 5: ONLY BOTTOM-RIGHT QUADRANT VISITED ONCE");

        CHECK(groupAnalytics.heatmap.total_count == expectedVisits);
        const float expectedNormVal = 1.0f / static_cast<float>(expectedVisits);

        for (int col = 0; col < GRID_COLS; ++col) {
            for (int row = 0; row < GRID_ROWS; ++row) {
                int rawCount = groupAnalytics.heatmap.position_distributuion[col][row];
                float normalizedVal = static_cast<float>(rawCount) / groupAnalytics.heatmap.total_count;

                if (col >= startCol && row < endRow) {
                    CHECK(rawCount == 1);
                    CHECK(normalizedVal == doctest::Approx(expectedNormVal));
                } else {
                    CHECK(rawCount == 0);
                    CHECK(normalizedVal == doctest::Approx(0.0f));
                }
            }
        }
    }

    TEST_CASE("SCENARIO 6: Extreme Court Corner Coordinates Clamping") {
        SnippetAnalyser analyser;
        GroupAnalytics groupAnalytics{};

        // Top-Left Corner (-14.35m, +7.6m) -> Col 0, Row 14
        analyser.UpdateHeatMapGrid({-14.35f, 7.6f}, groupAnalytics);
        CHECK(groupAnalytics.heatmap.position_distributuion[0][14] == 1);

        // Top-Right Corner (+14.35m, +7.6m) -> Col 27, Row 14
        analyser.UpdateHeatMapGrid({14.35f, 7.6f}, groupAnalytics);
        CHECK(groupAnalytics.heatmap.position_distributuion[27][14] == 1);

        // Bottom-Left Corner (-14.35m, -7.6m) -> Col 0, Row 0
        analyser.UpdateHeatMapGrid({-14.35f, -7.6f}, groupAnalytics);
        CHECK(groupAnalytics.heatmap.position_distributuion[0][0] == 1);

        // Bottom-Right Corner (+14.35m, -7.6m) -> Col 27, Row 0
        analyser.UpdateHeatMapGrid({14.35f, -7.6f}, groupAnalytics);
        CHECK(groupAnalytics.heatmap.position_distributuion[27][0] == 1);

        CHECK(groupAnalytics.heatmap.total_count == 4);
    }

    TEST_CASE("SCENARIO 7: Out-of-Bounds Clamping Safety") {
        SnippetAnalyser analyser;
        GroupAnalytics groupAnalytics{};

        // Far top-right off court (+100m, +50m) should safely clamp to max cell (27, 14)
        analyser.UpdateHeatMapGrid({100.0f, 50.0f}, groupAnalytics);
        CHECK(groupAnalytics.heatmap.position_distributuion[27][14] == 1);

        // Far bottom-left off court (-500m, -300m) should safely clamp to min cell (0, 0)
        analyser.UpdateHeatMapGrid({-500.0f, -300.0f}, groupAnalytics);
        CHECK(groupAnalytics.heatmap.position_distributuion[0][0] == 1);

        CHECK(groupAnalytics.heatmap.total_count == 2);
    }

    TEST_CASE("Hotspot Intensity Weighting") {
        SnippetAnalyser analyser;
        GroupAnalytics groupAnalytics{};

        // 90 visits to Paint Area cell (0,0) -> (Col 14, Row 7)
        for (int i = 0; i < 90; ++i) {
            analyser.UpdateHeatMapGrid({0.0f, 0.0f}, groupAnalytics);
        }
        // 10 visits to 3-Point Line cell (10m, 5m) -> (Col 23, Row 12)
        for (int i = 0; i < 10; ++i) {
            analyser.UpdateHeatMapGrid({10.0f, 5.0f}, groupAnalytics);
        }

        CHECK(groupAnalytics.heatmap.total_count == 100);

        float paintDensity = (float)groupAnalytics.heatmap.position_distributuion[14][7] / groupAnalytics.heatmap.total_count;
        CHECK(paintDensity == doctest::Approx(0.90f));

        float perimeterDensity = (float)groupAnalytics.heatmap.position_distributuion[23][12] / groupAnalytics.heatmap.total_count;
        CHECK(perimeterDensity == doctest::Approx(0.10f));
    }

    TEST_CASE("Multi-Group HeatMap Independence") {
        SnippetAnalyser analyser;
        SnippetAnalytics snippetAnalytics{};

        GroupAnalytics& group1 = snippetAnalytics.group_analytics[1];
        GroupAnalytics& group2 = snippetAnalytics.group_analytics[2];

        analyser.UpdateHeatMapGrid({5.0f, 2.0f}, group1);

        CHECK(group1.heatmap.total_count == 1);
        CHECK(group2.heatmap.total_count == 0);
    }
}
