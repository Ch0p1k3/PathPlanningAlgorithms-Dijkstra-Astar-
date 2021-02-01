#include "search.h"

#define PI_CONSTANT 3.14159265359
#define C_D CN_SQRT_TWO
#define C_HV 1

Search::Search() {}

Search::~Search() {}

double Search::Diagonal(Coordinates cur, Coordinates goal) const
{
    // c_hv * |dx - dy| + c_d * min(dx, dy)

    return C_HV * abs(abs(goal.i - cur.i) - abs(goal.j - cur.j)) + C_D * std::min(abs(goal.i - cur.i), abs(goal.j - cur.j));
}

double Search::Manhattan(Coordinates cur, Coordinates goal) const
{
    // c_hv * (dx + dy)

    return C_HV * (abs(cur.i - goal.i) + abs(cur.j - goal.j));
}

double Search::Euclidean(Coordinates cur, Coordinates goal) const
{
    // c_hv * sqrt(dx * dx + dy * dy)

    return C_HV * std::sqrt((goal.i - cur.i) * (goal.i - cur.i) + (goal.j - cur.j) * (goal.j - cur.j));
}

double Search::Chebyshev(Coordinates cur, Coordinates goal) const
{
    // max(dx, dy)

    return std::max(abs(cur.i - goal.i), abs(cur.j - goal.j));
}

double Search::HeuristicWeight(const EnvironmentOptions &options) const
{
    return options.hweight;
}

double Search::Heuristic(Coordinates cur, Coordinates goal, const EnvironmentOptions &options) const
{
    if (options.searchtype == CN_SP_ST_DIJK) {
        return 0;
    } else if (options.searchtype == CN_SP_ST_ASTAR) {
        if (options.metrictype == CN_SP_MT_DIAG) {
            return Diagonal(cur, goal);
        } else if (options.metrictype == CN_SP_MT_MANH) {
            return Manhattan(cur, goal);
        } else if (options.metrictype == CN_SP_MT_EUCL) {
            return Euclidean(cur, goal);
        } else if (options.metrictype == CN_SP_MT_CHEB) {
            return Chebyshev(cur, goal);
        }
    }
}

std::optional<Node> Search::GetNeighbours(Node& v, int i, int j, const Map &map, const EnvironmentOptions &options)
{
    if (!map.CellOnGrid(v.i + i, v.j + j) || map.CellIsObstacle(v.i + i, v.j + j)) {
        return std::nullopt;
    }
    if (abs(i) - abs(j) == 0) {
        if (!options.allowdiagonal) {
            return std::nullopt;
        } else if (!options.cutcorners) {
            if (map.CellIsObstacle(v.i, v.j + j) || map.CellIsObstacle(v.i + i, v.j)) {
                return std::nullopt;
            }
        } else if (!options.allowsqueeze) {
            if (map.CellIsObstacle(v.i, v.j + j) && map.CellIsObstacle(v.i + i, v.j)) {
                return std::nullopt;
            }
        }
        return Node
                { v.i + i
                , v.j + j
                , v.g + C_D + HeuristicWeight(options) * Heuristic({v.i + i, v.j + j}, map.getGoal(), options)
                , v.g + C_D
                , Heuristic({v.i + i, v.j + j}, map.getGoal(), options)
                , &v};
    }
    return Node
                { v.i + i
                , v.j + j
                , v.g + C_HV + HeuristicWeight(options) * Heuristic({v.i + i, v.j + j}, map.getGoal(), options)
                , v.g + C_HV
                , Heuristic({v.i + i, v.j + j}, map.getGoal(), options)
                , &v};
}

SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options)
{
    struct hashPair {
        size_t operator()(const std::pair<int, int>& e) const
        {
            return ((size_t)(e.first + e.second) * (e.first + e.second + 1) / 2 + e.second);
        }
    };

    auto cmpForOpen = [&options](const Node& left, const Node& right) {
        if (options.breakingties == CN_SP_BT_GMIN) {
            return std::tie(left.F, right.g, left.i, left.j) < std::tie(right.F, left.g, right.i, right.j);
        }
        return std::tie(left.F, left.g, left.i, left.j) < std::tie(right.F, right.g, right.i, right.j);
    };

    std::unordered_map<std::pair<int, int>, Node, hashPair> close;
    std::set<Node, decltype(cmpForOpen)> open(cmpForOpen);
    std::unordered_map<std::pair<int, int>, std::set<Node>::iterator, hashPair> auxiliary_map;

    auto time = std::chrono::steady_clock::now();

    const Coordinates start = map.getStart();
    const Coordinates goal = map.getGoal();

    size_t cntSt = 0;

    Node* searchedGoal = nullptr;
    
    open.clear();
    close.clear();
    auxiliary_map.clear();

    open.insert(Node
        ( start.i
        , start.j
        , HeuristicWeight(options) * Heuristic(start, goal, options)
        , 0
        , Heuristic(start, goal, options)
        , nullptr)
    );

    auxiliary_map[{start.i, start.j}] = open.begin();

    while (!open.empty()) {
        ++cntSt;
        auto l = *open.begin();
        open.erase(open.begin());
        auxiliary_map.erase({l.i, l.j});
        close[{l.i, l.j}] = l;
        auto v = &close[{l.i, l.j}];
        if (v->i == goal.i && v->j == goal.j) {
            searchedGoal = v;
            break;
        }
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i == 0 && j == 0) continue;
                if (auto neighbour = GetNeighbours(*v, i, j, map, options)) {
                    if (close.find({neighbour->i, neighbour->j}) == close.end()) {
                        auto it = auxiliary_map.find({neighbour->i, neighbour->j});
                        if (it != auxiliary_map.end() && it->second->g > neighbour->g) {
                            open.erase(it->second);
                            auto new_it = open.insert(neighbour.value());
                            auxiliary_map.erase(it);
                            auxiliary_map[{neighbour->i, neighbour->j}] = new_it.first;
                        } else if (it == auxiliary_map.end()) {
                            auto new_it = open.insert(neighbour.value());
                            auxiliary_map[{neighbour->i, neighbour->j}] = new_it.first;
                        }
                    }
                }
            }
        }
    }

    sresult.pathfound = (searchedGoal != nullptr);
    sresult.pathlength = sresult.pathfound ? searchedGoal->g : 0;
    sresult.nodescreated =  open.size() + close.size();
    sresult.numberofsteps = cntSt;
    if (sresult.pathfound) {
        makePrimaryPath(searchedGoal);
    }
    sresult.time = std::chrono::duration<double>(std::chrono::steady_clock::now() - time).count();
    if (sresult.pathfound) {
        makeSecondaryPath();
    }
    sresult.hppath = &hppath;
    sresult.lppath = &lppath;
    return sresult;
}

void Search::makePrimaryPath(Node* curNode)
{
    while (curNode) {
        lppath.push_front(*curNode);
        curNode = curNode->parent;
    }
}

void Search::makeSecondaryPath()
{
    if (!lppath.empty()) {
        hppath.push_front(lppath.front());
        if (lppath.size() != 1) {
            auto cur = std::next(lppath.begin());
            auto prev = lppath.begin();
            for (auto it = next(lppath.begin(), 2); it != lppath.end(); it = next(it)) {
                if (!(it->i - cur->i == cur->i - prev->i && it->j - cur->j == cur->j - prev->j)) {
                    hppath.push_back(*cur);
                }
                prev = cur;
                cur = it;
            }
        }
    }
}
