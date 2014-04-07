#ifndef __qtree__
#define __qtree__

class QTreeNode {
  private:
    point_t const* const points;

    bool isLeaf;
    bool isEmpty;

    // this is only meaningful if `isLeaf == true`
    size_t idx;

  public:
    QTreeNode(point_t const* const points) :
      points(points),
      isLeaf(true),
      isEmpty(true) {}

    void insert(size_t const* const idxs, const size_t n);
}

#endif

