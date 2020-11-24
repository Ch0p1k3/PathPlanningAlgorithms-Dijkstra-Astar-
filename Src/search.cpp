#include "search.h"

#define PI_CONSTANT 3.14159265359
#define C_D std::sqrt(2)
#define C_HV 1

Search::Search()
{
//set defaults here
}

Search::~Search() {}

double Search::Diagonal(Coordinates cur, Coordinates goal) const
{
    // c_hv * |dx - dy| + c_d * min(dx, dy)

    return C_HV * abs(abs(goal.i - cur.i) - abs(goal.j - cur.j)) + C_D * std::min(abs(goal.i - cur.i), abs(goal.j - cur.j));
}

double Search::Manhattan(Coordinates cur, Coordinates goal) const
{
    // c_hv * (dx + dy)

    return C_HV * (abs(cur.i - goal.i) + abs(cur.i - goal.i));
}

double Search::Euclidean(Coordinates cur, Coordinates goal) const
{
    // c_hv * sqrt(dx * dx + dy * dy)

    return C_HV * sqrt((goal.i - cur.i) * (goal.i - cur.i) + (goal.j - cur.j));
}

double Search::Chebyshev(Coordinates cur, Coordinates goal) const
{
    // max(dx, dy)

    return std::max(abs(cur.i - goal.i), abs(cur.j - goal.j));
}

double Search::HeuristicWeight() const
{
    return 1.;
}

double Search::Heuristic(Coordinates cur, Coordinates goal, const EnvironmentOptions &options) const
{
    if (options.metrictype == 0) {
        return Diagonal(cur, goal);
    } else if (options.metrictype == 1) {
        return Manhattan(cur, goal);
    } else if (options.metrictype == 2) {
        return Euclidean(cur, goal);
    } else if (options.metrictype == 3) {
        return Chebyshev(cur, goal);
    }
    return 0;
}

std::optional<Node> Search::GetNeighbours(Node& v, int i, int j, const Map &map, const EnvironmentOptions &options)
{
    if (map.CellOnGridAndIsTraversable(v.i + i, v.j + j)) {
        if (abs(i) - abs(j) == 0) {
            if (options.allowdiagonal) {
                bool f1 = map.CellOnGridAndIsTraversable(v.i, v.j + j);
                bool f2 = map.CellOnGridAndIsTraversable(v.i + i, v.j);
                if ((options.cutcorners && ((!f1 && !f2 && options.allowsqueeze) || (f1 && !f2) || (!f1 && f2))) || (f1 && f2)) {
                    return Node
                        { v.i + i
                        , v.j + j, v.g + C_D + HeuristicWeight() * Heuristic({v.i + i, v.j + j}, map.getGoal(), options)
                        , v.g + C_D
                        , Heuristic({v.i + i, v.j + j}, map.getGoal(), options)
                        , &v};
                } 
            }
        } else {
            return Node
                        { v.i + i
                        , v.j + j, v.g + C_HV + HeuristicWeight() * Heuristic({v.i + i, v.j + j}, map.getGoal(), options)
                        , v.g + C_HV
                        , Heuristic({v.i + i, v.j + j}, map.getGoal(), options)
                        , &v};
        }
    }
    return std::nullopt;
}

SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options)
{
    auto time = std::chrono::steady_clock::now();
    
    int cntClose = 0;

    Coordinates start = map.getStart();
    Coordinates goal = map.getGoal();
    int width = map.getMapWidth();
    int height = map.getMapHeight();

    close.resize(height, std::vector<Node>(width));

    size_t cntSt = 0;

    Node* searchedGoal = nullptr;

    open.push_back(Node
        (start.i
        , start.j
        , HeuristicWeight() * Heuristic(start, goal, options)
        , 0
        , Heuristic(start, goal, options)
        , nullptr)
    );

    while (!open.empty()) {
        ++cntSt;
        auto l = std::min_element(open.begin(), open.end());
        close[l->i][l->j] = *l;
        int inI = l->i, inJ = l->j;
        ++cntClose;
        open.erase(l);
        auto v = &close[inI][inJ];
        if (v->i == goal.i && v->j == goal.j) {
            searchedGoal = &close[v->i][v->j];
            break;
        }
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i == 0 && j == 0) continue;
                if (auto neighbour = GetNeighbours(close[v->i][v->j], i, j, map, options)) {
                    if (close[neighbour->i][neighbour->j].i == -1) {
                        auto it = std::find_if(open.begin(), open.end(), [neighbour](Node el) {
                            return el.i == neighbour->i && el.j == neighbour->j;
                        });
                        if (it != open.end() && it->g > neighbour->g) {
                            *it = neighbour.value();
                        } else if (it == open.end()) {
                            open.push_back(neighbour.value());
                        }
                    }
                }
            }
        }
    }

    sresult.pathfound = (searchedGoal != nullptr);
    sresult.pathlength = sresult.pathfound ? searchedGoal->g : 0;
    sresult.nodescreated =  open.size() + cntClose;
    sresult.numberofsteps = cntSt;
    if (sresult.pathfound) {
        makePrimaryPath(searchedGoal);
        makeSecondaryPath();
    }
    sresult.time = std::chrono::duration<double>(std::chrono::steady_clock::now() - time).count();
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
