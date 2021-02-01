#ifndef NODE_H
#define NODE_H
#include <memory>
#include <tuple>
//That's the data structure for storing a single search node.
//You MUST store all the intermediate computations occuring during the search
//incapsulated to Nodes (so NO separate arrays of g-values etc.)

struct Node
{
    int     i, j; //grid cell coordinates
    double  F, g, H; //f-, g- and h-values of the search node
    Node* parent; //backpointer to the predecessor node (e.g. the node which g-value was used to set the g-velue of the current node)

    Node()
    : i(-1)
    , j(-1)
    , F(0)
    , g(0)
    , H(0)
    , parent(nullptr) {}

    Node(int ni, int nj, double nF, double ng, double nH, Node* nparent)
    : i(ni)
    , j(nj)
    , F(nF)
    , g(ng)
    , H(nH)
    , parent(nparent) {}
};

#endif
