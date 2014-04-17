#include <omp.h>
#include <tbb/parallel_scan.h>
#include <tbb/blocked_range.h>

#include "euler.h"

using namespace tbb;

void eulerTour(midlvl_t const* const mids, const size_t l, size_t *const inIdxs,
    size_t *const outIdxs) {

  #pragma omp parallel for
    for (size_t i = 0; i < l; i++) {
      lvl_t lv = lvl(mids[i]);
      size_t inIdx = i * 2 - (size_t) lv;
      size_t subtree = subtreeSize(mids, l, i);
      size_t outIdx = inIdx + 2 * subtree + 1;
      inIdxs[i] = inIdx;
      outIdxs[i] = outIdx;
    }
}
template<typename T>
class PrefixSum {
  T sum;
  T* const y;
  const T* const z;

  public:
    PrefixSum( T y_[], const T z_[] ) : sum(0), y(y_), z(z_) {}

    T get_sum() const {
      return sum;
    }

    template<typename Tag>
    void operator()(const blocked_range<int>& r, Tag) {
      T temp = sum;

      for (int i=r.begin(); i<r.end(); ++i) {
        temp = temp + z[i];

        if (Tag::is_final_scan())
          y[i] = temp;
      }

      sum = temp;
    }

    PrefixSum(PrefixSum& b, split) : sum(0), y(b.y), z(b.z) {}

    void reverse_join(PrefixSum& a) {
      sum = a.sum + sum;
    }

    void assign(PrefixSum& b) {
      sum = b.sum;
    }
};

void parallelPrefixSum(double* data, const size_t l) {

  PrefixSum<double> body(data, data);
  parallel_scan(blocked_range<int>(0, l), body);

  body.get_sum();

  return;
}

void treePrefixSum(double const* const weights, size_t const* const inIdxs,
    size_t const* const outIdxs, const size_t l, double* result) {

  double* s = new double[2 * l];

  #pragma omp parallel for
    for (size_t i = 0; i < l * 2; i++)
      s[i] = 0;

  #pragma omp parallel for
    for (size_t i = 0; i < l; i++)
      s[inIdxs[i]] = weights[i];

  parallelPrefixSum(s, l * 2);

  #pragma omp parallel for
    for (size_t i = 0; i < l; i++)
      result[i] = s[outIdxs[i]] - s[inIdxs[i]] + weights[i];

  delete [] s;
}

