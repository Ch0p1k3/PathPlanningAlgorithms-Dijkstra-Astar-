#include "search.h"

#define PI_CONSTANT 3.14159265359
#define C_D std::sqrt(2)
#define C_HV 1

Search::Search()
{
//set defaults here
}

Search::~Search() {}

int Search::Diagonal(Coordinates cur, Coordinates goal) const
{
    // c_hv * |dx - dy| + c_d * min(dx, dy)

    return C_HV * abs(abs(goal.i - cur.i) - abs(goal.j - cur.j)) + C_D * std::min(abs(goal.i - cur.i), abs(goal.j - cur.j));
}

int Search::Manhattan(Coordinates cur, Coordinates goal) const
{
    // c_hv * (dx + dy)

    return C_HV * (abs(cur.i - goal.i) + abs(cur.i - goal.i));
}

int Search::Euclidean(Coordinates cur, Coordinates goal) const
{
    // c_hv * sqrt(dx * dx + dy * dy)

    return C_HV * sqrt((goal.i - cur.i) * (goal.i - cur.i) + (goal.j - cur.j));
}

int Search::Chebyshev(Coordinates cur, Coordinates goal) const
{
    // max(dx, dy)

    return std::max(abs(cur.i - goal.i), abs(cur.j - goal.j));
}

int Search::HeuristicWeight() const
{
    return 1;
}

int Search::Heuristic(Coordinates cur, Coordinates goal, const EnvironmentOptions &options) const
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
                    return Node{v.i + i, v.j + j, 0, v.g + C_D, 0, &v};
                } 
            }
        } else {
            return Node{v.i + i, v.j + j, 0, v.g + C_HV, 0, &v};
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

    open.push_back(Node(start.i, start.j, 0, 0, 0, nullptr));
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
        while (searchedGoal) {
            lppath.push_front(*searchedGoal);
            searchedGoal = searchedGoal->parent;
        }
        
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
    sresult.time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time).count();
    sresult.hppath = &hppath;
    sresult.lppath = &lppath;
    return sresult;
}
/*void Search::makePrimaryPath(Node curNode)
{
    //need to implement
}*/

/*void Search::makeSecondaryPath()
{
    //need to implement
}*/
