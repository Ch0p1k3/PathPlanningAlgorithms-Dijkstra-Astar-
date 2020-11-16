#include "search.h"

#define PI_CONSTANT 3.14159265359
#define SQRT_TWO    1.41421356237

Search::Search()
{
//set defaults here
}

Search::~Search() {}

std::optional<Node> Search::GetNeighbours(Node& v, int i, int j, const Map &map, const EnvironmentOptions &options)
{
    if (map.CellOnGridAndIsTraversable(v.i + i, v.j + j)) {
        if (abs(i - j) == 0) {
            if (options.allowdiagonal) {
                bool f1 = map.CellOnGridAndIsTraversable(v.i, v.j + j);
                bool f2 = map.CellOnGridAndIsTraversable(v.i + i, v.j);
                if ((options.cutcorners && ((!f1 && !f2 && options.allowsqueeze) || (f1 && !f2) || (!f1 && f2))) || (f1 && f2)) {
                    return Node{v.i + i, v.j + j, v.g + SQRT_TWO, 0, &v};
                } 
            }
        } else {
            return Node{v.i + i, v.j + j, v.g + 1, 0, &v};
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

    open.push_back(Node(start.i, start.j, 0, 0, nullptr));
    while (!open.empty()) {
        ++cntSt;
        auto v = std::min_element(open.begin(), open.end());
        close[v->i][v->j] = *v;
        ++cntClose;
        open.erase(v);
        if (v->i == goal.i && v->j == goal.j) {
            searchedGoal = &close[v->i][v->j];
            break;
        }
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
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

    sresult.pathfound = searchedGoal == nullptr;
    sresult.pathlength = sresult.pathfound ? searchedGoal->g : 0;
    sresult.nodescreated =  open.size() + cntClose;
    sresult.numberofsteps = cntSt;

    while (searchedGoal) {
        lppath.push_front(*searchedGoal);
        searchedGoal = searchedGoal->parent;
    }
    
    if (!lppath.empty()) {
        hppath.push_front(lppath.front());
        if (lppath.size() != 1) {
            auto cur = std::next(lppath.begin());
            auto prev = lppath.begin();
            for (auto it = next(lppath.begin(), 2); it != lppath.end(); next(it)) {
                if (!(it->i - cur->i == cur->i - prev->i && it->j - cur->j == cur->j - prev->j)) {
                    hppath.push_back(*cur);
                }
                prev = cur;
                cur = it;
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
