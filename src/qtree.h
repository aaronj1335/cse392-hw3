#ifndef __qtree__
#define __qtree__

#include <iostream>
#include <stdio.h>

#include "body.h"

class QTree {
  public:
    QTree(point_t const* const points, const size_t level = 0,
        double const* const coord = NULL, double width = 1,
        QTree* parent = NULL);

    void insert(size_t const* const idxs, const size_t n);
    void insert(size_t idx);

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
    const size_t level;
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
    // this is only meaningful if `isLeaf == true`
    size_t idx;

    QTree* getKidForPoint(const point_t point) const;

    void createKids();

};

#endif

