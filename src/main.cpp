#include <bits/stdc++.h>
using namespace std;

struct Pos {
    int x, y;
};

struct Rule {
    int c, q, A, S;
    char D;
};

const int INF = 1e9;

const int DX[4] = {-1, 1, 0, 0};
const int DY[4] = {0, 0, -1, 1};
const char DIR_CH[4] = {'U', 'D', 'L', 'R'};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, K;
    long long T;
    if (!(cin >> N >> K >> T)) return 0;

    vector<string> v(N), h(N - 1);
    for (int i = 0; i < N; ++i) cin >> v[i];
    for (int i = 0; i < N - 1; ++i) cin >> h[i];

    vector<Pos> targets(K);
    for (int k = 0; k < K; ++k) cin >> targets[k].x >> targets[k].y;

    auto can_move = [&](int i, int j, int dir) -> bool {
        if (dir == 0) {
            if (i == 0) return false;
            return h[i - 1][j] == '0';
        }
        if (dir == 1) {
            if (i == N - 1) return false;
            return h[i][j] == '0';
        }
        if (dir == 2) {
            if (j == 0) return false;
            return v[i][j - 1] == '0';
        }
        if (j == N - 1) return false;
        return v[i][j] == '0';
    };

    auto bfs_dist = [&](Pos s) -> vector<vector<int>> {
        vector<vector<int>> dist(N, vector<int>(N, INF));
        queue<Pos> q;
        dist[s.x][s.y] = 0;
        q.push(s);
        while (!q.empty()) {
            Pos p = q.front();
            q.pop();
            int dcur = dist[p.x][p.y];
            for (int dir = 0; dir < 4; ++dir) {
                if (!can_move(p.x, p.y, dir)) continue;
                int nx = p.x + DX[dir];
                int ny = p.y + DY[dir];
                if (dist[nx][ny] > dcur + 1) {
                    dist[nx][ny] = dcur + 1;
                    q.push({nx, ny});
                }
            }
        }
        return dist;
    };

    vector<vector<int>> g_turn(N, vector<int>(N, 0));
    vector<vector<int>> g_straight(N, vector<int>(N, 0));
    vector<Pos> path;
    int lastDirGlobal = -1;

    auto dir_from_pos = [&](const Pos& a, const Pos& b) -> int {
        if (b.x == a.x - 1 && b.y == a.y) return 0;
        if (b.x == a.x + 1 && b.y == a.y) return 1;
        if (b.x == a.x && b.y == a.y - 1) return 2;
        if (b.x == a.x && b.y == a.y + 1) return 3;
        return -1;
    };

    int N2 = N * N;

    for (int seg = 0; seg + 1 < K; ++seg) {
        Pos S = targets[seg];
        Pos G = targets[seg + 1];
        auto distStart = bfs_dist(S);
        auto distGoal = bfs_dist(G);
        int Lseg = distStart[G.x][G.y];

        if (Lseg >= INF) {
            if (seg == 0) path = {S};
            continue;
        }

        static long long distCost[20][20][5];
        static int prevX[20][20][5];
        static int prevY[20][20][5];
        static int prevD[20][20][5];

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int d = 0; d < 5; ++d) {
                    distCost[i][j][d] = (long long)4e18;
                    prevX[i][j][d] = -1;
                    prevY[i][j][d] = -1;
                    prevD[i][j][d] = -1;
                }
            }
        }

        int startDirIdx = (seg == 0 ? 4 : lastDirGlobal);

        struct Node {
            long long cost;
            int x, y, d;
            bool operator<(const Node& o) const { return cost > o.cost; }
        };

        priority_queue<Node> pq;
        distCost[S.x][S.y][startDirIdx] = 0;
        pq.push({0, S.x, S.y, startDirIdx});

        int bestGoalDir = -1;

        while (!pq.empty()) {
            auto cur = pq.top();
            pq.pop();
            if (cur.cost != distCost[cur.x][cur.y][cur.d]) continue;
            if (cur.x == G.x && cur.y == G.y) {
                bestGoalDir = cur.d;
                break;
            }

            for (int dout = 0; dout < 4; ++dout) {
                if (!can_move(cur.x, cur.y, dout)) continue;
                int nx = cur.x + DX[dout];
                int ny = cur.y + DY[dout];

                if (distStart[cur.x][cur.y] == INF || distGoal[nx][ny] == INF) continue;
                if (distStart[cur.x][cur.y] + 1 + distGoal[nx][ny] != Lseg) continue;
                if (cur.d != 4 && ((cur.d ^ 1) == dout)) continue;

                bool isStraight = cur.d != 4 && dout == cur.d;
                bool isTurn = cur.d != 4 && dout != cur.d;

                long long stepCost = 1;
                if (isStraight) {
                    if (g_turn[cur.x][cur.y] > 0) stepCost += 1'000'000;
                    stepCost += 1;
                }
                if (isTurn) {
                    if (g_straight[cur.x][cur.y] > 0) stepCost += 1'000'000;
                    stepCost += 2;
                }
                stepCost += g_turn[cur.x][cur.y] + g_straight[cur.x][cur.y];

                long long nd = cur.cost + stepCost;
                if (nd < distCost[nx][ny][dout]) {
                    distCost[nx][ny][dout] = nd;
                    prevX[nx][ny][dout] = cur.x;
                    prevY[nx][ny][dout] = cur.y;
                    prevD[nx][ny][dout] = cur.d;
                    pq.push({nd, nx, ny, dout});
                }
            }
        }

        vector<Pos> segPath;
        if (bestGoalDir == -1) {
            vector<vector<int>> pd(N, vector<int>(N, -1));
            queue<Pos> q;
            q.push(S);
            while (!q.empty()) {
                auto p = q.front();
                q.pop();
                if (p.x == G.x && p.y == G.y) break;
                for (int d = 0; d < 4; ++d) {
                    if (!can_move(p.x, p.y, d)) continue;
                    int nx = p.x + DX[d], ny = p.y + DY[d];
                    if (distStart[nx][ny] == distStart[p.x][p.y] + 1 && pd[nx][ny] == -1) {
                        pd[nx][ny] = d;
                        q.push({nx, ny});
                    }
                }
            }
            Pos cur = G;
            segPath.push_back(cur);
            while (!(cur.x == S.x && cur.y == S.y)) {
                for (int d = 0; d < 4; ++d) {
                    int px = cur.x - DX[d], py = cur.y - DY[d];
                    if (px < 0 || px >= N || py < 0 || py >= N) continue;
                    if (distStart[px][py] == distStart[cur.x][cur.y] - 1 && can_move(px, py, d)) {
                        cur = {px, py};
                        segPath.push_back(cur);
                        break;
                    }
                }
            }
            reverse(segPath.begin(), segPath.end());
        } else {
            int gx = G.x, gy = G.y, gd = bestGoalDir;
            segPath.push_back({gx, gy});
            while (true) {
                int px = prevX[gx][gy][gd];
                int py = prevY[gx][gy][gd];
                int pd = prevD[gx][gy][gd];
                if (px == -1) break;
                segPath.push_back({px, py});
                gx = px;
                gy = py;
                gd = pd;
            }
            reverse(segPath.begin(), segPath.end());
        }

        if (seg == 0) {
            for (auto& p : segPath) path.push_back(p);
        } else {
            for (int i = 1; i < (int)segPath.size(); ++i) path.push_back(segPath[i]);
        }

        int m = (int)segPath.size();
        if (m >= 2) {
            vector<int> localDir(m - 1);
            for (int i = 0; i + 1 < m; ++i) localDir[i] = dir_from_pos(segPath[i], segPath[i + 1]);
            int previousSegmentDir = lastDirGlobal;
            lastDirGlobal = localDir.back();

            for (int i = 0; i + 1 < m; ++i) {
                int x = segPath[i].x, y = segPath[i].y;
                int entryDir = -1;
                if (!(seg == 0 && i == 0)) entryDir = (i == 0 ? previousSegmentDir : localDir[i - 1]);
                if (entryDir == -1) continue;
                int exitDir = localDir[i];
                if (exitDir == entryDir) g_straight[x][y]++;
                else g_turn[x][y]++;
            }
        }
    }

    int L = (int)path.size() - 1;
    if (L <= 0) {
        cout << 1 << ' ' << 1 << ' ' << 0 << '\n';
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (j) cout << ' ';
                cout << 0;
            }
            cout << '\n';
        }
        return 0;
    }

    vector<int> stepAbsDir(L);
    for (int t = 0; t < L; ++t) stepAbsDir[t] = dir_from_pos(path[t], path[t + 1]);

    vector<int> eventKind(L, 0);
    const int right_of[4] = {3, 2, 0, 1};
    const int left_of[4] = {2, 3, 1, 0};
    const int back_of[4] = {1, 0, 3, 2};

    for (int t = 0; t < L; ++t) {
        if (t == 0) {
            eventKind[t] = 1;
            continue;
        }
        int oldd = stepAbsDir[t - 1];
        int newd = stepAbsDir[t];
        if (newd == oldd) eventKind[t] = 1;
        else if (newd == left_of[oldd]) eventKind[t] = 2;
        else if (newd == right_of[oldd]) eventKind[t] = 3;
        else if (newd == back_of[oldd]) eventKind[t] = 4;
    }

    vector<vector<vector<int>>> visits(N, vector<vector<int>>(N));
    for (int t = 0; t < L; ++t) visits[path[t].x][path[t].y].push_back(t);

    const int KMAX = 4;
    vector<int> cellType(N2, 0);
    vector<string> cellPattern(N2);
    vector<int> visitCountCell(N2, 0);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            auto& vlist = visits[i][j];
            int vid = i * N + j;
            visitCountCell[vid] = (int)vlist.size();
            if (vlist.empty()) continue;

            int cntStraight = 0, cntTurn = 0, cntBack = 0;
            string pat;
            for (int t : vlist) {
                if (t == 0) continue;
                if (eventKind[t] == 1) cntStraight++;
                else if (eventKind[t] == 2 || eventKind[t] == 3) {
                    cntTurn++;
                    pat.push_back(eventKind[t] == 2 ? 'L' : 'R');
                } else if (eventKind[t] == 4) cntBack++;
            }

            if (cntBack > 0 || (cntTurn > 0 && cntStraight > 0) || cntTurn > KMAX) {
                cellType[vid] = 2;
            } else if (cntTurn > 0) {
                cellType[vid] = 1;
                cellPattern[vid] = pat;
            }
        }
    }

    vector<string> patterns;
    unordered_map<string, int> patId;
    function<void(string)> dfs_pat = [&](string s) {
        if ((int)s.size() > KMAX) return;
        if (!patId.count(s)) {
            patId[s] = (int)patterns.size();
            patterns.push_back(s);
        }
        if ((int)s.size() == KMAX) return;
        dfs_pat(s + 'L');
        dfs_pat(s + 'R');
    };
    dfs_pat("");

    vector<int> colorNext(patterns.size());
    for (int i = 0; i < (int)patterns.size(); ++i) {
        if (patterns[i].empty()) colorNext[i] = i;
        else colorNext[i] = patId[patterns[i].substr(1)];
    }

    int nextColor = (int)patterns.size();
    vector<int> specialBase(N2, -1);
    for (int vid = 0; vid < N2; ++vid) {
        if (cellType[vid] == 2) {
            specialBase[vid] = nextColor;
            nextColor += visitCountCell[vid];
        }
    }

    int C = nextColor;
    int Q = 4;
    int emptyPatId = patId[""];
    vector<vector<int>> init_color(N, vector<int>(N, emptyPatId));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int vid = i * N + j;
            if (cellType[vid] == 1) init_color[i][j] = patId[cellPattern[vid]];
            else if (cellType[vid] == 2) init_color[i][j] = specialBase[vid];
        }
    }

    int firstAbs = stepAbsDir[0];
    int idx2abs[4], abs2idx[4];
    idx2abs[0] = firstAbs;
    idx2abs[1] = (firstAbs + 1) & 3;
    idx2abs[2] = (firstAbs + 2) & 3;
    idx2abs[3] = (firstAbs + 3) & 3;
    for (int i = 0; i < 4; ++i) abs2idx[idx2abs[i]] = i;

    auto rotL = [&](int q) { return (q + 3) & 3; };
    auto rotR = [&](int q) { return (q + 1) & 3; };

    vector<int> curColor(N2);
    for (int i = 0; i < N; ++i) for (int j = 0; j < N; ++j) curColor[i * N + j] = init_color[i][j];
    vector<int> specialVisitIdx(N2, 0);
    unordered_map<long long, int> ruleIndex;
    vector<Rule> rules;

    auto add_rule = [&](int c, int q, int A, int S, char D) {
        long long key = ((long long)c << 32) ^ (unsigned long long)q;
        if (!ruleIndex.count(key)) {
            ruleIndex[key] = (int)rules.size();
            rules.push_back({c, q, A, S, D});
        }
    };

    int q_state = 0;
    for (int t = 0; t < L; ++t) {
        int x = path[t].x, y = path[t].y;
        int vid = x * N + y;
        int c = curColor[vid];
        int q_in = q_state;

        if (cellType[vid] == 2) {
            int idx = specialVisitIdx[vid]++;
            int base = specialBase[vid];
            int m = visitCountCell[vid];
            int c_now = base + idx;
            int c_next = (idx + 1 < m) ? base + idx + 1 : emptyPatId;
            int absDir = stepAbsDir[t];
            int q_out = abs2idx[absDir];
            add_rule(c_now, q_in, c_next, q_out, DIR_CH[absDir]);
            curColor[vid] = c_next;
            q_state = q_out;
        } else if (cellType[vid] == 1) {
            int A = c;
            int q_out = q_in;
            if (t > 0 && (eventKind[t] == 2 || eventKind[t] == 3)) {
                A = colorNext[c];
                q_out = (eventKind[t] == 2 ? rotL(q_in) : rotR(q_in));
            }
            int absDir = idx2abs[q_out];
            add_rule(c, q_in, A, q_out, DIR_CH[absDir]);
            curColor[vid] = A;
            q_state = q_out;
        } else {
            int absDir = (t == 0 ? idx2abs[q_in] : stepAbsDir[t]);
            int q_out = abs2idx[absDir];
            add_rule(c, q_in, c, q_out, DIR_CH[absDir]);
            q_state = q_out;
        }
    }

    cout << C << ' ' << Q << ' ' << rules.size() << '\n';
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (j) cout << ' ';
            cout << init_color[i][j];
        }
        cout << '\n';
    }
    for (const auto& r : rules) cout << r.c << ' ' << r.q << ' ' << r.A << ' ' << r.S << ' ' << r.D << '\n';

    return 0;
}
