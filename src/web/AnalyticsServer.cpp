#include "AnalyticsServer.hpp"
#include <iostream>
#include <sstream>

AnalyticsServer::AnalyticsServer(int port)
    : _port(port)
{
    // Serve HTML Dashboard
    _svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(GetIndexHtml(), "text/html");
    });

    // Server-Sent Events (SSE) streaming endpoint
    _svr.Get("/events", [this](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Cache-Control", "no-cache");
        res.set_header("Connection", "keep-alive");

        res.set_chunked_content_provider(
            "text/event-stream",
            [this](size_t offset, httplib::DataSink &sink) {
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    _sinks.insert(&sink);
                }
                
                // Keep streaming open while client is connected
                while (_running && sink.is_writable()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }

                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    _sinks.erase(&sink);
                }
                return true;
            }
        );
    });
}

AnalyticsServer::~AnalyticsServer()
{
    Stop();
}

void AnalyticsServer::Start()
{
    if (_running) return;
    _running = true;

    _serverThread = std::thread([this]() {
        std::cout << "\n======================================================\n";
        std::cout << " 🚀 Live Analytics Web Server running at:\n";
        std::cout << " 👉 http://localhost:" << _port << '\n';
        std::cout << "======================================================\n\n";
        _svr.listen("0.0.0.0", _port);
    });
}

void AnalyticsServer::Stop()
{
    if (!_running) return;
    _running = false;
    _svr.stop();
    if (_serverThread.joinable()) {
        _serverThread.join();
    }
}

void AnalyticsServer::BroadcastAnalytics(const PlayerSnapshot& snapshot, const PlayerAnalytics& analytics)
{
    if (!_running) return;

    std::ostringstream ss;
    ss << "data: {"
       << "\"timestamp_ms\":" << snapshot.timestamp_ms << ","
       << "\"player_id\":" << snapshot.player_id << ","
       << "\"group_id\":" << snapshot.group_id << ","
       << "\"x_m\":" << snapshot.x_m << ","
       << "\"y_m\":" << snapshot.y_m << ","
       << "\"speed\":" << analytics.speed << ","
       << "\"acceleration\":" << analytics.acceleration
       << "}\n\n";

    std::string message = ss.str();

    std::lock_guard<std::mutex> lock(_mutex);
    for (auto it = _sinks.begin(); it != _sinks.end(); ) {
        if ((*it)->is_writable()) {
            (*it)->write(message.data(), message.size());
            ++it;
        } else {
            it = _sinks.erase(it);
        }
    }
}

const char* AnalyticsServer::GetIndexHtml()
{
    return R"rawhtml(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Basketball Analytics - Realtime Telemetry</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap" rel="stylesheet">
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Inter', system-ui, -apple-system, sans-serif;
        }
        body {
            background-color: #0b0f19;
            color: #f1f5f9;
            min-height: 100vh;
            padding: 24px;
            display: flex;
            flex-direction: column;
            gap: 24px;
        }
        header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            background: #1e293b;
            padding: 16px 24px;
            border-radius: 12px;
            border: 1px solid #334155;
            box-shadow: 0 10px 25px -5px rgba(0, 0, 0, 0.3);
        }
        .title-group {
            display: flex;
            align-items: center;
            gap: 12px;
        }
        .title-group h1 {
            font-size: 1.35rem;
            font-weight: 700;
            color: #ffffff;
            letter-spacing: -0.02em;
        }
        .status-badge {
            display: inline-flex;
            align-items: center;
            gap: 8px;
            padding: 6px 14px;
            border-radius: 20px;
            font-size: 0.85rem;
            font-weight: 600;
            background: rgba(16, 185, 129, 0.15);
            color: #10b981;
            border: 1px solid rgba(16, 185, 129, 0.3);
        }
        .status-dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background-color: #10b981;
            box-shadow: 0 0 10px #10b981;
            animation: pulse 1.5s infinite;
        }
        @keyframes pulse {
            0% { opacity: 1; transform: scale(1); }
            50% { opacity: 0.4; transform: scale(1.2); }
            100% { opacity: 1; transform: scale(1); }
        }
        .metrics-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 16px;
        }
        .metric-card {
            background: #1e293b;
            padding: 16px 20px;
            border-radius: 12px;
            border: 1px solid #334155;
        }
        .metric-card p {
            font-size: 0.8rem;
            color: #94a3b8;
            text-transform: uppercase;
            letter-spacing: 0.05em;
            margin-bottom: 6px;
        }
        .metric-card h2 {
            font-size: 1.6rem;
            font-weight: 700;
            color: #38bdf8;
        }
        .charts-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(500px, 1fr));
            gap: 20px;
        }
        .section-card {
            background: #1e293b;
            border-radius: 12px;
            border: 1px solid #334155;
            padding: 20px;
            box-shadow: 0 10px 25px -5px rgba(0, 0, 0, 0.3);
            display: flex;
            flex-direction: column;
        }
        .section-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 16px;
        }
        .section-header h2 {
            font-size: 1.1rem;
            font-weight: 600;
            color: #f8fafc;
        }
        .group-badge {
            background: #334155;
            color: #f1f5f9;
            padding: 4px 10px;
            border-radius: 6px;
            font-size: 0.8rem;
            font-weight: 600;
        }
        .chart-container {
            position: relative;
            height: 360px;
            width: 100%;
        }
        .placeholder-card {
            border: 2px dashed #334155;
            border-radius: 12px;
            padding: 32px;
            text-align: center;
            color: #64748b;
            background: rgba(30, 41, 59, 0.4);
        }
        .placeholder-card h3 {
            color: #94a3b8;
            font-size: 1.1rem;
            margin-bottom: 8px;
        }
    </style>
</head>
<body>

    <header>
        <div class="title-group">
            <span style="font-size: 1.6rem;">🏀</span>
            <h1>Basketball Analytics Engine Telemetry</h1>
        </div>
        <div class="status-badge" id="statusBadge">
            <span class="status-dot"></span>
            <span id="statusText">CONNECTING...</span>
        </div>
    </header>

    <div class="metrics-grid">
        <div class="metric-card">
            <p>Active Groups</p>
            <h2 id="activeGroupsCount">0</h2>
        </div>
        <div class="metric-card">
            <p>Active Players</p>
            <h2 id="activePlayersCount">0</h2>
        </div>
        <div class="metric-card">
            <p>Data Packets Streamed</p>
            <h2 id="packetCount">0</h2>
        </div>
        <div class="metric-card">
            <p>Latest Timestamp</p>
            <h2 id="latestTime">0.0s</h2>
        </div>
    </div>

    <!-- TOP SECTION: LIVE 2D ACCELERATION PLOTS BY GROUP -->
    <div id="groupChartsGrid" class="charts-grid">
        <!-- Dynamic Group Chart Cards are inserted here -->
    </div>

    <!-- BOTTOM SECTION PLACEHOLDER FOR STEP 2 (FIELD & HEATMAP) -->
    <div class="placeholder-card">
        <h3>🏀 2D Basketball Field & Speed Heatmap</h3>
        <p>Step 2 visualization target: Live player positions on court grid & heatmap density.</p>
    </div>

    <script>
        const COLOR_PALETTE = [
            '#38bdf8', '#f43f5e', '#10b981', '#fbbf24', 
            '#a855f7', '#ec4899', '#3b82f6', '#f97316'
        ];

        const playerColorMap = {};
        let colorIdx = 0;

        function getPlayerColor(playerId) {
            if (!playerColorMap[playerId]) {
                playerColorMap[playerId] = COLOR_PALETTE[colorIdx % COLOR_PALETTE.length];
                colorIdx++;
            }
            return playerColorMap[playerId];
        }

        const groupCharts = {}; // Map of groupId -> Chart instance
        let packetTotal = 0;
        const playerSet = new Set();
        const groupSet = new Set();

        function createGroupChart(groupId) {
            const gridContainer = document.getElementById('groupChartsGrid');

            // Create Section Card
            const card = document.createElement('div');
            card.className = 'section-card';
            card.id = `group_card_${groupId}`;

            card.innerHTML = `
                <div class="section-header">
                    <h2>📈 Group ${groupId} - Player Accelerations (m/s²)</h2>
                    <span class="group-badge">Group ${groupId}</span>
                </div>
                <div class="chart-container">
                    <canvas id="accelChart_group_${groupId}"></canvas>
                </div>
            `;

            gridContainer.appendChild(card);

            const ctx = document.getElementById(`accelChart_group_${groupId}`).getContext('2d');
            const chart = new Chart(ctx, {
                type: 'line',
                data: { datasets: [] },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    animation: false,
                    scales: {
                        x: {
                            type: 'linear',
                            title: { display: true, text: 'Time (seconds)', color: '#94a3b8' },
                            grid: { color: '#334155' },
                            ticks: { color: '#94a3b8' }
                        },
                        y: {
                            title: { display: true, text: 'Acceleration (m/s²)', color: '#94a3b8' },
                            grid: { color: '#334155' },
                            ticks: { color: '#94a3b8' },
                            suggestedMin: -5,
                            suggestedMax: 10
                        }
                    },
                    plugins: {
                        legend: {
                            labels: { color: '#f8fafc', font: { family: 'Inter', size: 12 } }
                        },
                        tooltip: {
                            mode: 'index',
                            intersect: false
                        }
                    }
                }
            });

            groupCharts[groupId] = chart;
            return chart;
        }

        const eventSource = new EventSource('/events');

        eventSource.onopen = () => {
            document.getElementById('statusText').innerText = 'LIVE STREAMING';
            document.getElementById('statusBadge').style.borderColor = 'rgba(16, 185, 129, 0.3)';
        };

        eventSource.onerror = () => {
            document.getElementById('statusText').innerText = 'DISCONNECTED';
            document.getElementById('statusBadge').style.background = 'rgba(239, 68, 68, 0.15)';
            document.getElementById('statusBadge').style.color = '#ef4444';
        };

        eventSource.onmessage = (event) => {
            const data = JSON.parse(event.data);
            packetTotal++;
            playerSet.add(data.player_id);
            groupSet.add(data.group_id);

            const timeSec = (data.timestamp_ms / 1000.0).toFixed(2);
            
            document.getElementById('packetCount').innerText = packetTotal;
            document.getElementById('activePlayersCount').innerText = playerSet.size;
            document.getElementById('activeGroupsCount').innerText = groupSet.size;
            document.getElementById('latestTime').innerText = timeSec + 's';

            // Get or create chart for this group
            let chart = groupCharts[data.group_id];
            if (!chart) {
                chart = createGroupChart(data.group_id);
            }

            // Find or create dataset for player in this group's chart
            let dataset = chart.data.datasets.find(ds => ds.label === `Player ${data.player_id}`);
            if (!dataset) {
                const color = getPlayerColor(data.player_id);
                dataset = {
                    label: `Player ${data.player_id}`,
                    data: [],
                    borderColor: color,
                    backgroundColor: color,
                    borderWidth: 2,
                    pointRadius: 2,
                    tension: 0.2
                };
                chart.data.datasets.push(dataset);
            }

            // Append new acceleration point
            dataset.data.push({ x: parseFloat(timeSec), y: parseFloat(data.acceleration.toFixed(2)) });

            // Sliding window: keep last 60 points per player
            if (dataset.data.length > 60) {
                dataset.data.shift();
            }

            chart.update('none');
        };
    </script>
</body>
</html>
    )rawhtml";
}
