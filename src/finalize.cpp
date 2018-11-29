
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

#include <fstream>
#include <hip/hip_runtime_api.h>
#include <hiprand/hiprand.h>

#include "utils.hpp"
#include "hpcg.hpp"

/*!
  Closes the I/O stream used for logging information throughout the HPCG run.

  @return returns 0 upon success and non-zero otherwise

  @see HPCG_Init
*/
int
HPCG_Finalize(void) {
  HPCG_fout.close();

  // Free workspace
  HIP_CHECK(hipFree(workspace));

  // Free RNG
  hiprandDestroyGenerator(rng);

  // Reset HIP device
  hipDeviceReset();

  return 0;
}
