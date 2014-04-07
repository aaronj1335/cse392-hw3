#ifndef __qtree__
#define __qtree__

#include <stdio.h>

#include "body.h"

class QTree {
  private:
    /* static bool kidDimsMap[NC][D]; */
    /* static bool kidDimsMapInitialized; */

    point_t const* const points;
    const size_t level;
    const double width;

    bool isLeaf;
    bool isEmpty;
    QTree* kids[NC];
    // should b const. dont know how to copy-initialize this array
    // 0: lower-left
    // 1: lower-right
    // 2: upper-left
    // 3: upper-right
    double coord[D];
    // this is only meaningful if `isLeaf == true`
    size_t idx;

    static bool** getKidDimMap();

    QTree* getKidForPoint(const point_t point) const;

    void createKids();

  public:
    QTree(point_t const* const points, const size_t level = 0,
        double const* const coord = NULL, double width = 1);

    void insert(size_t const* const idxs, const size_t n);
    void insert(size_t idx);
};

#endif

