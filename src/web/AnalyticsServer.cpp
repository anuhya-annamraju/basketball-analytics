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
       << "\"acceleration\":" << analytics.acceleration << ","
       << "\"distance\":" << analytics.distance
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
            margin-bottom: 12px;
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
        .player-distance-bar {
            display: flex;
            flex-wrap: wrap;
            gap: 10px;
            margin-bottom: 14px;
            padding: 10px 14px;
            background: rgba(15, 23, 42, 0.6);
            border: 1px solid #334155;
            border-radius: 8px;
        }
        .player-dist-chip {
            display: inline-flex;
            align-items: center;
            gap: 6px;
            font-size: 0.85rem;
            font-weight: 500;
            color: #94a3b8;
            background: #1e293b;
            padding: 4px 10px;
            border-radius: 6px;
            border: 1px solid #334155;
        }
        .player-dist-dot {
            width: 10px;
            height: 10px;
            border-radius: 50%;
        }
        .player-dist-val {
            color: #38bdf8;
            font-weight: 700;
        }
        .chart-container {
            position: relative;
            height: 360px;
            width: 100%;
        }
        .court-container {
            position: relative;
            width: 100%;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        canvas#courtCanvas {
            background: #0f172a;
            border-radius: 8px;
            border: 1px solid #334155;
            box-shadow: inset 0 0 20px rgba(0, 0, 0, 0.5);
            max-width: 100%;
            height: auto;
        }
        .court-legend {
            display: flex;
            gap: 20px;
            margin-top: 12px;
            font-size: 0.85rem;
            color: #94a3b8;
        }
        .legend-item {
            display: flex;
            align-items: center;
            gap: 6px;
        }
        .legend-color {
            width: 12px;
            height: 12px;
            border-radius: 3px;
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

    <!-- TOP SECTION: LIVE 2D ACCELERATION PLOTS BY GROUP WITH PLAYER DISTANCES -->
    <div id="groupChartsGrid" class="charts-grid">
        <!-- Dynamic Group Chart Cards with live player distance telemetry are inserted here -->
    </div>

    <!-- BOTTOM SECTION: LIVE 2D BASKETBALL COURT & HEATMAP POSITION DISTRIBUTION -->
    <div class="section-card">
        <div class="section-header">
            <h2>🏀 2D Basketball Court & Position Heatmap (Real-Time)</h2>
            <span class="group-badge">Center (0,0) | X: [-14.35m, +14.35m] | Y: [-7.6m, +7.6m]</span>
        </div>
        <div class="court-container">
            <canvas id="courtCanvas" width="960" height="520"></canvas>
            <div class="court-legend">
                <div class="legend-item">
                    <div class="legend-color" style="background: rgba(251, 191, 36, 0.4); border: 1px solid #fbbf24;"></div>
                    <span>Position Heatmap Density</span>
                </div>
                <div class="legend-item">
                    <div class="legend-color" style="background: #38bdf8; border-radius: 50%;"></div>
                    <span>Live Player Positions</span>
                </div>
                <div class="legend-item">
                    <div class="legend-color" style="background: #ef4444;"></div>
                    <span>Center Origin (0,0)</span>
                </div>
            </div>
        </div>
    </div>

    )rawhtml"
    R"rawhtml(
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

        const groupCharts = {};
        let packetTotal = 0;
        const playerSet = new Set();
        const groupSet = new Set();

        // Heatmap Grid (28 cols x 15 rows)
        const GRID_COLS = 28;
        const GRID_ROWS = 15;
        const heatmapGrid = Array.from({ length: GRID_COLS }, () => Array(GRID_ROWS).fill(0));
        let maxHeatCount = 1;

        // Position Trails & Live Positions
        const positionDots = [];
        const livePlayerPositions = {};

        function createGroupChart(groupId) {
            const gridContainer = document.getElementById('groupChartsGrid');

            const card = document.createElement('div');
            card.className = 'section-card';
            card.id = `group_card_${groupId}`;

            card.innerHTML = `
                <div class="section-header">
                    <h2>📈 Group ${groupId} - Player Accelerations (m/s²)</h2>
                    <span class="group-badge">Group ${groupId}</span>
                </div>
                <div class="player-distance-bar" id="playerDistances_group_${groupId}">
                    <!-- Live Player Distance Telemetry Chips -->
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
                        tooltip: { mode: 'index', intersect: false }
                    }
                }
            });

            groupCharts[groupId] = chart;
            return chart;
        }

        // --- 2D BASKETBALL COURT RENDERER ---
        const courtCanvas = document.getElementById('courtCanvas');
        const courtCtx = courtCanvas.getContext('2d');

        const COURT_LENGTH_M = 28.7;
        const COURT_WIDTH_M = 15.2;

        function metersToCanvas(x_m, y_m) {
            const marginX = 50;
            const marginY = 40;
            const drawWidth = courtCanvas.width - 2 * marginX;
            const drawHeight = courtCanvas.height - 2 * marginY;

            const canvasX = courtCanvas.width / 2 + (x_m / (COURT_LENGTH_M / 2)) * (drawWidth / 2);
            const canvasY = courtCanvas.height / 2 - (y_m / (COURT_WIDTH_M / 2)) * (drawHeight / 2);

            return { x: canvasX, y: canvasY };
        }

        function drawCourt() {
            const w = courtCanvas.width;
            const h = courtCanvas.height;

            courtCtx.fillStyle = '#0f172a';
            courtCtx.fillRect(0, 0, w, h);

            const marginX = 50;
            const marginY = 40;
            const drawW = w - 2 * marginX;
            const drawH = h - 2 * marginY;

            // 1. Draw Heatmap Grid Density
            const cellW = drawW / GRID_COLS;
            const cellH = drawH / GRID_ROWS;

            for (let c = 0; c < GRID_COLS; c++) {
                for (let r = 0; r < GRID_ROWS; r++) {
                    const count = heatmapGrid[c][r];
                    if (count > 0) {
                        const alpha = Math.min(0.85, (count / maxHeatCount) * 0.8 + 0.1);
                        const cellX = marginX + c * cellW;
                        const cellY = marginY + (GRID_ROWS - 1 - r) * cellH;

                        const ratio = count / maxHeatCount;
                        const red = 255;
                        const green = Math.floor(255 * (1 - ratio * 0.7));
                        const blue = Math.floor(50 * (1 - ratio));

                        courtCtx.fillStyle = `rgba(${red}, ${green}, ${blue}, ${alpha})`;
                        courtCtx.fillRect(cellX, cellY, cellW, cellH);
                    }
                }
            }

            // 2. Draw Court Boundary & Grid Lines
            courtCtx.strokeStyle = '#334155';
            courtCtx.lineWidth = 1;

            for (let c = 0; c <= GRID_COLS; c++) {
                const gx = marginX + c * cellW;
                courtCtx.beginPath();
                courtCtx.moveTo(gx, marginY);
                courtCtx.lineTo(gx, marginY + drawH);
                courtCtx.stroke();
            }
            for (let r = 0; r <= GRID_ROWS; r++) {
                const gy = marginY + r * cellH;
                courtCtx.beginPath();
                courtCtx.moveTo(marginX, gy);
                courtCtx.lineTo(marginX + drawW, gy);
                courtCtx.stroke();
            }

            // Outer Boundary
            courtCtx.strokeStyle = '#94a3b8';
            courtCtx.lineWidth = 3;
            courtCtx.strokeRect(marginX, marginY, drawW, drawH);

            // Half-Court Line
            const centerX = w / 2;
            courtCtx.beginPath();
            courtCtx.moveTo(centerX, marginY);
            courtCtx.lineTo(centerX, marginY + drawH);
            courtCtx.stroke();

            // Center Circle at (0,0)
            const centerPos = metersToCanvas(0, 0);
            const circleRadius = (1.8 / (COURT_LENGTH_M / 2)) * (drawW / 2);

            courtCtx.beginPath();
            courtCtx.arc(centerPos.x, centerPos.y, circleRadius, 0, 2 * Math.PI);
            courtCtx.stroke();

            // Center Origin Crosshair (0,0)
            courtCtx.strokeStyle = '#ef4444';
            courtCtx.lineWidth = 2;
            courtCtx.beginPath();
            courtCtx.arc(centerPos.x, centerPos.y, 4, 0, 2 * Math.PI);
            courtCtx.fillStyle = '#ef4444';
            courtCtx.fill();

            // Key Areas
            const keyW = (5.8 / COURT_LENGTH_M) * drawW;
            const keyH = (4.9 / COURT_WIDTH_M) * drawH;
            const keyY = marginY + (drawH - keyH) / 2;

            courtCtx.strokeStyle = '#94a3b8';
            courtCtx.lineWidth = 2;
            courtCtx.strokeRect(marginX, keyY, keyW, keyH);
            courtCtx.strokeRect(marginX + drawW - keyW, keyY, keyW, keyH);

            // Axis Labels
            courtCtx.fillStyle = '#64748b';
            courtCtx.font = '11px Inter, sans-serif';
            courtCtx.textAlign = 'center';
            courtCtx.fillText('-14.35m', marginX, marginY - 10);
            courtCtx.fillText('0 (Center)', centerX, marginY - 10);
            courtCtx.fillText('+14.35m', marginX + drawW, marginY - 10);

            courtCtx.textAlign = 'right';
            courtCtx.fillText('+7.6m', marginX - 10, marginY + 12);
            courtCtx.fillText('0', marginX - 10, centerPos.y + 4);
            courtCtx.fillText('-7.6m', marginX - 10, marginY + drawH);

            // 3. Position Trail Dots
            for (let i = 0; i < positionDots.length; i++) {
                const dot = positionDots[i];
                const pt = metersToCanvas(dot.x, dot.y);
                const alpha = (i / positionDots.length) * 0.6 + 0.2;

                courtCtx.beginPath();
                courtCtx.arc(pt.x, pt.y, 3, 0, 2 * Math.PI);
                courtCtx.fillStyle = dot.color;
                courtCtx.globalAlpha = alpha;
                courtCtx.fill();
            }
            courtCtx.globalAlpha = 1.0;

            // 4. Live Moving Player Markers with Total Distance Label
            for (const pid in livePlayerPositions) {
                const p = livePlayerPositions[pid];
                const pt = metersToCanvas(p.x, p.y);

                courtCtx.beginPath();
                courtCtx.arc(pt.x, pt.y, 10, 0, 2 * Math.PI);
                courtCtx.fillStyle = p.color;
                courtCtx.globalAlpha = 0.3;
                courtCtx.fill();

                courtCtx.globalAlpha = 1.0;
                courtCtx.beginPath();
                courtCtx.arc(pt.x, pt.y, 6, 0, 2 * Math.PI);
                courtCtx.fillStyle = p.color;
                courtCtx.strokeStyle = '#ffffff';
                courtCtx.lineWidth = 2;
                courtCtx.fill();
                courtCtx.stroke();

                // Player ID & Live Distance Label
                courtCtx.fillStyle = '#ffffff';
                courtCtx.font = 'bold 11px Inter, sans-serif';
                courtCtx.textAlign = 'center';
                const distStr = p.distance ? ` (${p.distance.toFixed(1)}m)` : '';
                courtCtx.fillText(`P${pid}${distStr}`, pt.x, pt.y - 14);
            }
        }

        drawCourt();

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

            // 1. Update Group Acceleration Line Charts (Top Half)
            let chart = groupCharts[data.group_id];
            if (!chart) {
                chart = createGroupChart(data.group_id);
            }

            const distValStr = (data.distance !== undefined) ? data.distance.toFixed(1) + 'm' : '0.0m';

            // Update or create Live Player Distance Chip in the Group Header Bar
            let distBar = document.getElementById(`playerDistances_group_${data.group_id}`);
            if (distBar) {
                let chip = document.getElementById(`distChip_${data.group_id}_${data.player_id}`);
                if (!chip) {
                    chip = document.createElement('div');
                    chip.className = 'player-dist-chip';
                    chip.id = `distChip_${data.group_id}_${data.player_id}`;
                    distBar.appendChild(chip);
                }
                const color = getPlayerColor(data.player_id);
                chip.innerHTML = `<span class="player-dist-dot" style="background:${color};"></span>` +
                                 `<span>Player ${data.player_id}:</span>` +
                                 `<span class="player-dist-val">${distValStr}</span>`;
            }

            // Update Chart Dataset with live distance label
            let dataset = chart.data.datasets.find(ds => ds.playerId === data.player_id);
            if (!dataset) {
                const color = getPlayerColor(data.player_id);
                dataset = {
                    label: `Player ${data.player_id} (${distValStr})`,
                    playerId: data.player_id,
                    data: [],
                    borderColor: color,
                    backgroundColor: color,
                    borderWidth: 2,
                    pointRadius: 2,
                    tension: 0.2
                };
                chart.data.datasets.push(dataset);
            } else {
                dataset.label = `Player ${data.player_id} (${distValStr})`;
            }

            dataset.data.push({ x: parseFloat(timeSec), y: parseFloat(data.acceleration.toFixed(2)) });
            if (dataset.data.length > 60) dataset.data.shift();
            chart.update('none');

            // 2. Update 2D Court & Heatmap Position Distribution (Bottom Half)
            const playerColor = getPlayerColor(data.player_id);

            livePlayerPositions[data.player_id] = {
                x: data.x_m,
                y: data.y_m,
                group: data.group_id,
                color: playerColor,
                distance: data.distance || 0.0
            };

            const col = Math.min(GRID_COLS - 1, Math.max(0, Math.floor(((data.x_m + COURT_LENGTH_M / 2) / COURT_LENGTH_M) * GRID_COLS)));
            const row = Math.min(GRID_ROWS - 1, Math.max(0, Math.floor(((data.y_m + COURT_WIDTH_M / 2) / COURT_WIDTH_M) * GRID_ROWS)));
            
            heatmapGrid[col][row]++;
            if (heatmapGrid[col][row] > maxHeatCount) {
                maxHeatCount = heatmapGrid[col][row];
            }

            positionDots.push({ x: data.x_m, y: data.y_m, color: playerColor });
            if (positionDots.length > 300) positionDots.shift();

            drawCourt();
        };
    </script>
</body>
</html>
    )rawhtml";
}
