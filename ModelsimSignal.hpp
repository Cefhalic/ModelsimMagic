// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2026
// Imperial College London Particles community
// -----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "mti.h"
#include <iostream>
#include <iomanip>
#include <array>
#include <format>

#include "Magic.hpp"

// ===============================================================================================
// ------------------------------------------------------------------------------
typedef enum {
  STD_LOGIC_U,
  STD_LOGIC_X,
  STD_LOGIC_0,
  STD_LOGIC_1,
  STD_LOGIC_Z,
  STD_LOGIC_W,
  STD_LOGIC_L,
  STD_LOGIC_H,
  STD_LOGIC_D
} standardLogicType;
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
void mti_get( mtiSignalIdT& aMtiId , uint64_t& aWord )
{
  auto lType = mti_GetTypeKind( mti_GetSignalType( aMtiId ) );
  
  if( lType == MTI_TYPE_SCALAR ) {
    aWord = mti_GetSignalValue(aMtiId);  
  } else {        
    auto Size = mti_TickLength( mti_GetSignalType( aMtiId ) );

    mtiSignalIdT *lBuf;
    lBuf = mti_GetSignalSubelements( aMtiId , 0 );

    uint64_t lInt(0);
    uint64_t lMask(0x8000000000000000);
    for ( int i(0); i != Size; ++i ) {
      if( mti_GetSignalValue(lBuf[i])==STD_LOGIC_1 ) lInt |= lMask;
      lMask >>= 1;
    }

    aWord = lInt >> (64-Size);
    mti_VsimFree( lBuf );  
  }
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
void mti_get( mtiSignalIdT& aMtiId , int64_t& aWord )
{
  auto lType = mti_GetTypeKind( mti_GetSignalType( aMtiId ) );
  
  if( lType == MTI_TYPE_SCALAR ) {
    aWord = mti_GetSignalValue(aMtiId);  
  } else {        
    auto Size = mti_TickLength( mti_GetSignalType( aMtiId ) );

    mtiSignalIdT *lBuf;
    lBuf = mti_GetSignalSubelements( aMtiId , 0 );

    int64_t lInt(0);
    uint64_t lMask(0x8000000000000000);
    for ( int i(0); i != Size; ++i ) {
      if( mti_GetSignalValue(lBuf[i])==STD_LOGIC_1 ) lInt |= lMask;
      lMask >>= 1;
    }

    aWord = lInt >> (64-Size);
    mti_VsimFree( lBuf );  
  }
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
void mti_get( mtiSignalIdT& aMtiId , bool& aBool )
{
  if( mti_TickLength( mti_GetSignalType( aMtiId ) ) == 2 ) aBool =   mti_GetSignalValue(aMtiId);
  else                                                     aBool = ( mti_GetSignalValue(aMtiId)==STD_LOGIC_1 );
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
template< typename T , std::size_t Size >
void mti_get( mtiSignalIdT& aMtiId , std::array< T , Size >& aParallel )
{
  auto FliSize = mti_TickLength( mti_GetSignalType( aMtiId ) );
  if( FliSize != Size )
  {
    mti_PrintFormatted( "FLI size (%i) != C++ size (%i)" , FliSize , Size );
    mti_FatalError();
  }
  
  mtiSignalIdT *lBuf;  
  lBuf = mti_GetSignalSubelements( aMtiId , 0 );
  mtiTypeIdT lType = mti_GetSignalType( aMtiId );

  if( mti_TickLeft( lType ) < mti_TickRight( lType ) ) {
    for ( int i(0); i != Size; ++i ) mti_get( lBuf[i] , aParallel[i] );
  } else {    
    for ( int i(0); i != Size; ++i ) mti_get( lBuf[i] , aParallel[(Size-1)-i] );
  }

  mti_VsimFree( lBuf );
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
void mti_get_magic ( mtiSignalIdT* aBuf )
{}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
template< typename S , typename... T >
void mti_get_magic ( mtiSignalIdT* aBuf , S& aArg , T&&... aRest )
{
  // char* lName = mti_GetSignalNameIndirect( *aBuf , NULL , 0 );
  // std::cout << "MTI " << lName << std::endl;
  mti_get( *aBuf , aArg );
  mti_get_magic ( ++aBuf , aRest... );
  // mti_VsimFree( lName );
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
template< typename T >
void mti_get( mtiSignalIdT& aMtiId , magic< T > & aArg )
{
  // std::cout << "C++ " << aArg.MagicFields() << std::endl;  
  mtiSignalIdT *lBuf;
  lBuf = mti_GetSignalSubelements( aMtiId , 0 );
  aArg.Apply( [ lBuf ]( auto&&... params ){ mti_get_magic( lBuf , params... ); } );
  mti_VsimFree( lBuf );
}
// ------------------------------------------------------------------------------
// ===============================================================================================








// ===============================================================================================
// ------------------------------------------------------------------------------
void mti_set( mtiSignalIdT& aMtiId , const uint64_t& aWord )
{
  auto lType = mti_GetTypeKind( mti_GetSignalType( aMtiId ) );
  
  if( lType == MTI_TYPE_SCALAR ) {
    mti_SetSignalValue( aMtiId , aWord );  
  } else {   
    auto Size = mti_TickLength( mti_GetSignalType( aMtiId ) );

    mtiSignalIdT *lBuf;
    lBuf = mti_GetSignalSubelements( aMtiId , 0 );

    uint64_t lMask(0x8000000000000000);
    for ( int i(0); i != Size; ++i ) {
      mti_SetSignalValue( lBuf[i] , (aWord&lMask) ? STD_LOGIC_1 : STD_LOGIC_0 );
      lMask >>= 1;
    }

    mti_VsimFree( lBuf ); 
  }
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
void mti_set( mtiSignalIdT& aMtiId , const int64_t& aWord )
{
  auto lType = mti_GetTypeKind( mti_GetSignalType( aMtiId ) );
  
  if( lType == MTI_TYPE_SCALAR ) {
    mti_SetSignalValue( aMtiId , aWord );  
  } else {
    auto Size = mti_TickLength( mti_GetSignalType( aMtiId ) );

    mtiSignalIdT *lBuf;
    lBuf = mti_GetSignalSubelements( aMtiId , 0 );

    uint64_t lMask(0x8000000000000000);
    for ( int i(0); i != Size; ++i ) {
      mti_SetSignalValue( lBuf[i] , (aWord&lMask) ? STD_LOGIC_1 : STD_LOGIC_0 );
      lMask >>= 1;
    }
    
    mti_VsimFree( lBuf );      
  }
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
void mti_set( mtiSignalIdT& aMtiId , const bool& aBool )
{
    if( mti_TickLength( mti_GetSignalType( aMtiId ) ) == 2 ) mti_SetSignalValue(aMtiId , aBool);
    else                                                     mti_SetSignalValue(aMtiId , aBool?STD_LOGIC_1:STD_LOGIC_0 );
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
template< typename T , std::size_t Size >
void mti_set( mtiSignalIdT& aMtiId , const std::array< T , Size >& aParallel )
{
  auto FliSize = mti_TickLength( mti_GetSignalType( aMtiId ) );
  if( FliSize != Size )
  {
    mti_PrintFormatted( "FLI size (%i) != C++ size (%i)" , FliSize , Size );
    mti_FatalError();
  }
  
  mtiSignalIdT *lBuf;  
  lBuf = mti_GetSignalSubelements( aMtiId , 0 );
  mtiTypeIdT lType = mti_GetSignalType( aMtiId );

  // for ( int i(0); i != Size; ++i ) std::cout << i << " : " <<  lBuf[i] << " : " << aParallel[i] << std::endl;

  if( mti_TickLeft( lType ) < mti_TickRight( lType ) ) {
    for ( int i(0); i != Size; ++i ) mti_set( lBuf[i] , aParallel[i] );
  } else {    
    for ( int i(0); i != Size; ++i ) mti_set( lBuf[i] , aParallel[(Size-1)-i] );
  }

  mti_VsimFree( lBuf );
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
void mti_set_magic ( mtiSignalIdT* aBuf )
{}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
template< typename S , typename... T >
void mti_set_magic ( mtiSignalIdT* aBuf , S& aArg , T&&... aRest )
{
  // char* lName = mti_GetSignalNameIndirect( *aBuf , NULL , 0 );
  // std::cout << "MTI " << lName << std::endl;
  mti_set( *aBuf , aArg );
  mti_set_magic ( ++aBuf , aRest... );
  // mti_VsimFree( lName );
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
template< typename T >
void mti_set( mtiSignalIdT& aMtiId , const magic< T > & aArg )
{
  // std::cout << "C++ " << aArg.MagicFields() << std::endl;  
  mtiSignalIdT *lBuf;
  lBuf = mti_GetSignalSubelements( aMtiId , 0 );
  aArg.Apply( [ lBuf ]( auto&&... params ){ mti_set_magic( lBuf , params... ); } );
  mti_VsimFree( lBuf );
}
// ------------------------------------------------------------------------------


// ===============================================================================================


















// ------------------------------------------------------------------------------
template< typename T >
struct ModelsimSignal
{
  mtiSignalIdT mSignal;
  T mData;

  ModelsimSignal() : mSignal( NULL ) , mData()
  {}

  ModelsimSignal( const char* aName ) : mSignal( NULL ) , mData()
  {   
    connect( aName );
  }

  void connect( const char* aName )
  {
    if( mSignal != NULL ) throw std::runtime_error( "Signal already connected" );
    mSignal = mti_FindSignal( (char*)(aName) );
    if( mSignal == NULL ) throw std::runtime_error( std::format( "Signal '{}' not found" , aName ) );
  }
  
  void get_signal()
  { 
    if( mSignal == NULL ) throw std::runtime_error( "Signal not connected" );
    mti_get( mSignal , mData ); 
  }

  void set_signal()
  { 
    if( mSignal == NULL ) throw std::runtime_error( "Signal not connected" );
    mti_set( mSignal , mData ); 
  }

    
};

template< typename T >
std::ostream& operator<< ( std::ostream& aStr , const ModelsimSignal< T >& aArg )
{
  return ( aStr << aArg.mData );
}
// ------------------------------------------------------------------------------


















