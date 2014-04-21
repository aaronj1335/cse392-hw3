#ifndef __euler__
#define __euler__

#include <omp.h>
#include <tbb/parallel_scan.h>
#include <tbb/blocked_range.h>

#include "morton.h"

void eulerTour(midlvl_t const* const mids, const size_t l, size_t *const inIdxs,
    size_t *const outIdxs);

template<typename T>
class PrefixSum {
  T sum;
  T* const y;
  const T* const z;
  const T id;

  public:
    PrefixSum(T y_[], const T z_[], const T id) :
      sum(id),
      y(y_),
      z(z_),
      id(id) {}

    T get_sum() const {
      return sum;
    }

    template<typename Tag>
    void operator()(const tbb::blocked_range<int>& r, Tag) {
      T temp = sum;

      for (int i = r.begin(); i < r.end(); ++i) {
        temp = temp + z[i];

        if (Tag::is_final_scan())
          y[i] = temp;
      }

      sum = temp;
    }

    PrefixSum(PrefixSum& b, tbb::split) : sum(b.id), y(b.y), z(b.z), id(id) {}

    void reverse_join(PrefixSum& a) {
      sum = a.sum + sum;
    }

    void assign(PrefixSum& b) {
      sum = b.sum;
    }
};

template<typename T>
void parallelPrefixSum(T* data, const size_t l, const T id) {

  PrefixSum<T> body(data, data, id);
  tbb::parallel_scan(tbb::blocked_range<int>(0, l), body);

  body.get_sum();

  return;
}

template<typename T>
void treePrefixSum(T const* const weights, size_t const* const inIdxs,
    size_t const* const outIdxs, const size_t l, const T id, T* result) {

  T* s = new T[2 * l];

  #pragma omp parallel for
    for (size_t i = 0; i < l * 2; i++)
      s[i] = id;

  #pragma omp parallel for
    for (size_t i = 0; i < l; i++)
      s[inIdxs[i]] = weights[i];

  parallelPrefixSum(s, l * 2, id);

  #pragma omp parallel for
    for (size_t i = 0; i < l; i++)
      result[i] = s[outIdxs[i]] - s[inIdxs[i]] + weights[i];

  delete[] s;
}

float potential(midlvl_t const* const mids, QTree const* const* const nodes,
    point_t const* const points, size_t const* const idxs, const size_t l,
    const size_t idx, const point_t& p);

#endif
