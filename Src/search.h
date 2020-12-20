#ifndef SEARCH_H
#define SEARCH_H
#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include <list>
#include <vector>
#include <math.h>
#include <limits>
#include <chrono>
#include <set>
#include <optional>
#include <memory>
#include <typeinfo>
#include <algorithm>
#include <cmath>
#include <unordered_map>

class Search
{
    public:
        Search();
        ~Search(void);
        SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options);

    protected:
        SearchResult                    sresult; //This will store the search result
        std::list<Node>                 lppath, hppath; //

        std::unordered_map<int, Node> close;
        std::list<Node> open;

        double Heuristic(Coordinates, Coordinates, const EnvironmentOptions &) const;

        double Euclidean(Coordinates, Coordinates) const;

        double Manhattan(Coordinates, Coordinates) const;

        double Chebyshev(Coordinates, Coordinates) const;

        double Diagonal(Coordinates, Coordinates) const;

        double HeuristicWeight(const EnvironmentOptions &) const;

        void makePrimaryPath(Node*);

        void makeSecondaryPath();

        std::optional<Node> GetNeighbours(Node&, int, int, const Map &, const EnvironmentOptions &);
};
#endif
