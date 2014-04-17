#ifndef __qtree__
#define __qtree__

#include <iostream>
#include <stdio.h>

#include "body.h"

class QTree {
  public:
    QTree(point_t const* const points, const lvl_t level = 0,
        double const* const coord = NULL, double width = 1,
        QTree* parent = NULL);

    void insert(size_t const* const idxs, const size_t l);
    void insert(size_t idx);

    midlvl_t toMid(const bool useAnchor = false) const;

    lvl_t lvl() const;

    point_t const* point() const;

    // inorder, depth-first iterator, to be speicific
    class iterator {
      public:
        iterator(QTree* ptr);
        iterator operator++();
        iterator operator++(int junk);
        QTree& operator*();
        QTree* operator->();
        bool operator==(const iterator& rhs);
        bool operator!=(const iterator& rhs);

      private:
        QTree* ptr;
        QTree const* const start;

        QTree* next(QTree const* p) const;
        void advance();
        const size_t idx(QTree const* const p) const;
    };

    iterator begin();
    iterator end();

    friend std::ostream& operator<<(std::ostream& os, const QTree& tree);

  private:
    point_t const* const points;
    const lvl_t level;
    const double width;

    bool isLeaf;
    bool isEmpty;
    QTree* parent;
    QTree* kids[NUM_KIDS];
    // should b const. dont know how to copy-initialize this array
    // 0: lower-left
    // 1: lower-right
    // 2: upper-left
    // 3: upper-right
    double coord[DIM];
    // this is this node's index in the `points` array.  this is only
    // meaningful if `isLeaf == true`
    size_t idx;

    // given a point, return the node in the `kids[]` array where it belongs
    // (based on which quadrant it belongs in)
    QTree* getKidForPoint(const point_t point) const;

    void createKids();

};

#endif

