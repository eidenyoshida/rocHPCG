
//@HEADER
// ***************************************************
//
// HPCG: High Performance Conjugate Gradient Benchmark
//
// Contact:
// Michael A. Heroux ( maherou@sandia.gov)
// Jack Dongarra     (dongarra@eecs.utk.edu)
// Piotr Luszczek    (luszczek@eecs.utk.edu)
//
// ***************************************************
//@HEADER

/*!
 @file ComputeSPMV.cpp

 HPCG routine
 */

#include "ComputeSPMV.hpp"

#include <hip/hip_runtime.h>

__global__ void kernel_spmv_ell(local_int_t m,
                                local_int_t n,
                                local_int_t ell_width,
                                const local_int_t* ell_col_ind,
                                const double* ell_val,
                                const double* x,
                                double* y)
{
    local_int_t row = blockIdx.x * blockDim.x + threadIdx.x;

    if(row >= m)
    {
        return;
    }

    double sum = 0.0;

    for(local_int_t p = 0; p < ell_width; ++p)
    {
        local_int_t idx = p * m + row;
        local_int_t col = ell_col_ind[idx];

        if(col >= 0 && col < n)
        {
            sum = fma(ell_val[idx], x[col], sum);
        }
    }

    y[row] = sum;
}

/*!
  Routine to compute sparse matrix vector product y = Ax where:
  Precondition: First call exchange_externals to get off-processor values of x

  This routine calls the reference SpMV implementation by default, but
  can be replaced by a custom, optimized routine suited for
  the target system.

  @param[in]  A the known system matrix
  @param[in]  x the known vector
  @param[out] y the On exit contains the result: Ax.

  @return returns 0 upon success and non-zero otherwise

  @see ComputeSPMV_ref
*/
int ComputeSPMV(const SparseMatrix& A, const Vector& x, Vector& y)
{
    assert(x.localLength >= A.localNumberOfColumns);
    assert(y.localLength >= A.localNumberOfRows);

#ifndef HPCG_NO_MPI
//    ExchangeHalo(A, x); TODO
#endif

    hipLaunchKernelGGL((kernel_spmv_ell),
                       dim3((A.localNumberOfRows - 1) / 128 + 1),
                       dim3(128),
                       0,
                       0,
                       A.localNumberOfRows,
                       A.localNumberOfColumns,
                       A.ell_width,
                       A.ell_col_ind,
                       A.ell_val,
                       x.d_values,
                       y.d_values);

    return 0;
}
