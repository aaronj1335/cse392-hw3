#include <assert.h>

#include "qtree.h"

QTree::QTree(point_t const* const points, const size_t level,
    double const* const coord, double width, QTree* parent) :
  points(points),
  level(level),
  width(width),
  isLeaf(true),
  isEmpty(true),
  parent (parent) {

  for (size_t i = 0; i < DIM; i++)
    this->coord[i] = coord == NULL? 0 : coord[i];
}

void QTree::createKids() {
  size_t kidLevel = level + 1;
  double kidWidth = width / 2.;

  for (size_t i = 0; i < NUM_KIDS; i++) {
    double kidCoord[DIM];

    for (size_t j = 0; j < DIM; j++) {
      double offset = (1 << j) & i? 1.0 : 0.0;
      kidCoord[j] = coord[j] + (offset * kidWidth);
    }

    kids[i] = new QTree(points, kidLevel, kidCoord, kidWidth, this);
  }

  isLeaf = false;
}

QTree* QTree::getKidForPoint(const point_t point) const {
  // this is shady... but it's because our point_t struct isn't
  // dimension-generic, but this friggin tree is. that was dumb.
  double const* const pointAsPointer = (double const* const) &point;
  double half = width / 2;
  size_t which = 0;

  for (size_t i = 0; i < DIM; i++)
    if (pointAsPointer[i] > coord[i] + half)
      which += (1 << i);

  return kids[which];
}

void QTree::insert(size_t const* const idxs, const size_t n) {
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

    if (level >= MAX_LEVEL) {
      fprintf(stderr, "could not split point at (%f, %f) to insert point at ("
          "%f, %f). ignoring it\n", points[idx].x, points[idx].y,
          points[idxs[0]].x, points[idxs[0]].y);
      return;
    }

    createKids();
    insert(idx);
  }

  for (size_t i = 0; i < n; i++) {
    QTree* kid = getKidForPoint(points[idxs[i]]);
    kid->insert(idxs[i]);
  }
}

void QTree::insert(const size_t idx) {
  return insert(&idx, 1);
}

QTree::iterator::iterator(QTree* ptr) : ptr(ptr), start(ptr) { }

QTree::iterator QTree::iterator::operator++() {
  iterator i = *this;
  advance();
  return i;
}

QTree::iterator QTree::iterator::operator++(int junk) {
  advance();
  return *this;
}

QTree& QTree::iterator::operator*() {
  return *ptr;
}

QTree* QTree::iterator::operator->() {
  return ptr;
}

bool QTree::iterator::operator==(const iterator& rhs) {
  return ptr == rhs.ptr;
}

bool QTree::iterator::operator!=(const iterator& rhs) {
  return ptr != rhs.ptr;
}

const size_t QTree::iterator::idx(QTree const* const p) const {
  for (size_t i = 0; i < NUM_KIDS; i++)
    if (p->parent->kids[i] == p)
      return i;

  // this node is pointing to a parent that does not have this child
  assert(false);
}

// the next two functions are hideous. i don't even know. whatever.
QTree* QTree::iterator::next(QTree const* p) const {
  if (p) {
    if (p->isLeaf) {
      while (p != start && idx(p) == NUM_KIDS - 1)
        p = p->parent;
      return p->parent? p->parent->kids[idx(p) + 1] : NULL;
    } else {
      return p->kids[0];
    }
  }

  return NULL;
}

void QTree::iterator::advance() {
  if (ptr == start) {
    ptr = ptr->isLeaf? NULL : next(ptr);
    return;
  }

  QTree* prev = ptr;
  ptr = next(ptr);

  while (ptr && ptr != start && ptr == prev->parent) {
    prev = ptr;
    ptr = next(ptr);
  }

  if (ptr == start)
    ptr = NULL;
}

QTree::iterator QTree::begin() {
  return iterator(this);
}

QTree::iterator QTree::end() {
  return NULL;
}

std::ostream& operator<<(std::ostream& os, const QTree& tree) {
  os << "QTree<" << &tree << ", " << tree.level << ", ";

  if (tree.isLeaf)
    if (tree.isEmpty)
      os << "empty leaf";
    else
      os << "(" << tree.points[tree.idx].x << ", "
        << tree.points[tree.idx].y << ")";
  else
    os << "parent";

  os << ">";

  return os;
}

