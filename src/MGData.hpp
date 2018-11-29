
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
 @file MGData.hpp

 HPCG data structure
 */

#ifndef MGDATA_HPP
#define MGDATA_HPP

#include <cassert>
#include <hip/hip_runtime_api.h>

#include "utils.hpp"
#include "SparseMatrix.hpp"
#include "Vector.hpp"

struct MGData_STRUCT {
  int numberOfPresmootherSteps; // Call ComputeSYMGS this many times prior to coarsening
  int numberOfPostsmootherSteps; // Call ComputeSYMGS this many times after coarsening
  local_int_t * f2cOperator; //!< 1D array containing the fine operator local IDs that will be injected into coarse space.
  Vector * rc; // coarse grid residual vector
  Vector * xc; // coarse grid solution vector
  Vector * Axf; // fine grid residual vector
  /*!
   This is for storing optimized data structres created in OptimizeProblem and
   used inside optimized ComputeSPMV().
   */
  void * optimizationData;

  local_int_t* d_f2cOperator; //!< f2cOperator on device
};
typedef struct MGData_STRUCT MGData;

/*!
 Constructor for the data structure of CG vectors.

 @param[in] Ac - Fully-formed coarse matrix
 @param[in] f2cOperator -
 @param[out] data the data structure for CG vectors that will be allocated to get it ready for use in CG iterations
 */
inline void InitializeMGData(local_int_t * f2cOperator, MGData & data) {
  data.f2cOperator = f2cOperator; // Space for injection operator
  return;
}

inline void HIPInitializeMGData(local_int_t* f2cOperator, Vector* rc, Vector* xc, Vector* Axf, MGData& data)
{
    data.numberOfPresmootherSteps = 1;
    data.numberOfPostsmootherSteps = 1;
    data.d_f2cOperator = f2cOperator;
    data.rc = rc;
    data.xc = xc;
    data.Axf = Axf;
}

/*!
 Destructor for the CG vectors data.

 @param[inout] data the MG data structure whose storage is deallocated
 */
inline void DeleteMGData(MGData & data) {

  delete [] data.f2cOperator;
  DeleteVector(*data.Axf);
  DeleteVector(*data.rc);
  DeleteVector(*data.xc);
  HIPDeleteVector(*data.rc);
  HIPDeleteVector(*data.xc);
  delete data.Axf;
  delete data.rc;
  delete data.xc;

  HIP_CHECK(hipFree(data.d_f2cOperator));

  return;
}

#endif // MGDATA_HPP

