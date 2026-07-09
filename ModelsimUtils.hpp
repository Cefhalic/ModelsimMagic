// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2026
// Imperial College London Particles community
// -----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "mti.h"
#include <cstdint>
#include <cmath>

float SimTime( const bool& aUpdateDeltaResolution = false ){  

  static float DeltaResolution = 0.0 / 0.0;
  if( aUpdateDeltaResolution ) {
    DeltaResolution = std::pow( 10.0 , mti_GetResolutionLimit() );
  }

  int64_t lBuf;
  mti_NowIndirect( &lBuf );

  return lBuf * DeltaResolution;
}
