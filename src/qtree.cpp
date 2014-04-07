#include "qtree.h"

QTreeNode::QTreeNode(point_t const* const points, const size_t level,
    double const* const coord, double width) :
  points(points),
  level(level),
  width(width),
  isLeaf(true),
  isEmpty(true) {

  for (size_t i = 0; i < D; i++)
    this->coord[i] = coord == NULL? 0 : coord[i];
}

void QTreeNode::createKids() {
  size_t kidLevel = level + 1;
  double kidWidth = width / 2.;

  for (size_t i = 0; i < NC; i++) {
    double kidCoord[D];

    for (size_t j = 0; j < D; j++) {
      double offset = (1 << j) & i? 1.0 : 0.0;
      kidCoord[j] = coord[j] + (offset * kidWidth);
    }

    kids[i] = new QTreeNode(points, kidLevel, kidCoord, kidWidth);
  }

  isLeaf = false;
}

QTreeNode* QTreeNode::getKidForPoint(const point_t point) const {
  // this is shady... but it's because our point class isn't dimension-generic,
  // but this friggin tree is. that was dumb.
  double const* const pointAsPointer = (double const* const) &point;
  double half = width / 2;
  size_t which = 0;

  for (size_t i = 0; i < D; i++)
    if (pointAsPointer[i] > coord[i] + half)
      which += (1 << i);

  return kids[which];
}

void QTreeNode::insert(size_t const* const idxs, const size_t n) {
  if (!n)
    return;

  if (isLeaf) {
    // if the following holds, we're just inserting a single point into this
    // node, so we can do that and return
    if (n == 1 && (isEmpty || idxs[0] == idx)) {
      isEmpty = false;
      idx = idxs[0];
      return;
    }

    isEmpty = false;

    createKids();
    insert(idx);
  }

  for (size_t i = 0; i < n; i++) {
    QTreeNode* kid = getKidForPoint(points[idxs[i]]);
    kid->insert(idxs[i]);
  }
}

void QTreeNode::insert(const size_t idx) {
  return insert(&idx, 1);
}
