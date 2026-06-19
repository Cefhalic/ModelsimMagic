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
#include <map>

#include "Magic.hpp"


// ------------------------------------------------------------------------------
void print( mtiSignalIdT& aMtiId )
{
  char* lName = mti_GetSignalNameIndirect( aMtiId , NULL , 0 );
  std::cout << "MTI " << lName << std::endl;
  mti_VsimFree( lName );
}
// ------------------------------------------------------------------------------


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

std::map< mtiTypeKindT , std::string > TypeKind = { {MTI_TYPE_SCALAR,"Integer"} , {MTI_TYPE_ARRAY,"Array"} , {MTI_TYPE_RECORD,"Record"} , {MTI_TYPE_ENUM,"Enumeration"} };
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
void mti_get( mtiSignalIdT& aMtiId , uint64_t& aWord )
{
  auto lKind = mti_GetTypeKind( mti_GetSignalType( aMtiId ) );
  
  if( lKind == MTI_TYPE_SCALAR ) {
    aWord = mti_GetSignalValue(aMtiId);  
  } else {        
    auto Size = mti_TickLength( mti_GetSignalType( aMtiId ) );

    mtiSignalIdT* lBuf = mti_GetSignalSubelements( aMtiId , NULL );

    aWord = 0;
    uint64_t lMask( 1 << (Size-1) );     
    for ( int i(0); i != Size; ++i ) {
      if( mti_GetSignalValue(lBuf[i])==STD_LOGIC_1 ) aWord |= lMask;      
      lMask >>= 1;
    }

    mti_VsimFree( lBuf );    
  }
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
void mti_get( mtiSignalIdT& aMtiId , int64_t& aWord )
{
  auto lKind = mti_GetTypeKind( mti_GetSignalType( aMtiId ) );
  
  if( lKind == MTI_TYPE_SCALAR ) {
    aWord = mti_GetSignalValue(aMtiId);  
  } else {        
    auto Size = mti_TickLength( mti_GetSignalType( aMtiId ) );

    mtiSignalIdT* lBuf = mti_GetSignalSubelements( aMtiId , NULL );

    aWord = 0;
    uint64_t lMask( 1 << (Size-1) );    
    for ( int i(0); i != Size; ++i ) {
      if( mti_GetSignalValue(lBuf[i])==STD_LOGIC_1 ) aWord |= lMask;
      lMask >>= 1;
    }

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
  auto lType = mti_GetSignalType( aMtiId );
  auto lKind = mti_GetTypeKind( lType );
  if( lKind != MTI_TYPE_ARRAY ) throw std::runtime_error( "Expect array-types" );

  auto FliSize = mti_TickLength( lType );
  if( FliSize != Size ) throw std::runtime_error( std::format( "FLI size ({}) != C++ size ({})" , FliSize , Size ) );
  
  mtiSignalIdT* lBuf = mti_GetSignalSubelements( aMtiId , NULL );

  if( mti_TickLeft( lType ) < mti_TickRight( lType ) ) {
    for ( int i(0); i != Size; ++i ) mti_get( lBuf[i] , aParallel[i] );
  } else {    
    for ( int i(0); i != Size; ++i ) mti_get( lBuf[i] , aParallel[(Size-1)-i] );
  }

  mti_VsimFree( lBuf );
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
template< typename T >
void mti_get( mtiSignalIdT& aMtiId , magic< T > & aArg )
{
  // std::cout << "C++ " << aArg.MagicFields() << std::endl;  
  mtiSignalIdT* lBuf = mti_GetSignalSubelements( aMtiId , NULL );
  mtiSignalIdT* lPtr( lBuf );
  aArg.Apply( [ & ]( auto&&... params ){ ( mti_get( *lPtr++ , params ) , ... ); } );
  mti_VsimFree( lBuf );
}
// ------------------------------------------------------------------------------
// ===============================================================================================








// ===============================================================================================
// ------------------------------------------------------------------------------
void mti_set( mtiSignalIdT& aMtiId , const uint64_t& aWord )
{
  auto lKind = mti_GetTypeKind( mti_GetSignalType( aMtiId ) );
  
  if( lKind == MTI_TYPE_SCALAR ) {
    mti_SetSignalValue( aMtiId , aWord );  
  } else {   
    auto Size = mti_TickLength( mti_GetSignalType( aMtiId ) );

    mtiSignalIdT* lBuf = mti_GetSignalSubelements( aMtiId , NULL );

    uint64_t lMask( 1 << (Size-1) );
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
  auto lKind = mti_GetTypeKind( mti_GetSignalType( aMtiId ) );
  
  if( lKind == MTI_TYPE_SCALAR ) {
    mti_SetSignalValue( aMtiId , aWord );  
  } else {
    auto Size = mti_TickLength( mti_GetSignalType( aMtiId ) );

    mtiSignalIdT* lBuf = mti_GetSignalSubelements( aMtiId , NULL );

    uint64_t lMask( 1 << (Size-1) );    
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
  auto lType = mti_GetSignalType( aMtiId );
  auto lKind = mti_GetTypeKind( lType );
  if( lKind != MTI_TYPE_ARRAY ) throw std::runtime_error( "Expect array-types" );

  auto FliSize = mti_TickLength( lType );
  if( FliSize != Size ) throw std::runtime_error( std::format( "FLI size ({}) != C++ size ({})" , FliSize , Size ) );
  
  mtiSignalIdT* lBuf = mti_GetSignalSubelements( aMtiId , NULL );

  if( mti_TickLeft( lType ) < mti_TickRight( lType ) ) {
    for ( int i(0); i != Size; ++i ) mti_set( lBuf[i] , aParallel[i] );
  } else {    
    for ( int i(0); i != Size; ++i ) mti_set( lBuf[i] , aParallel[(Size-1)-i] );
  }

  mti_VsimFree( lBuf );
}
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
template< typename T >
void mti_set( mtiSignalIdT& aMtiId , const magic< T > & aArg )
{
  // std::cout << "C++ " << aArg.MagicFields() << std::endl;  
  mtiSignalIdT* lBuf = mti_GetSignalSubelements( aMtiId , NULL );
  mtiSignalIdT* lPtr( lBuf );
  aArg.Apply( [ & ]( auto&&... params ){ ( mti_set( *lPtr++ , params ) , ... ); } );
  mti_VsimFree( lBuf );
}
// ------------------------------------------------------------------------------


// ===============================================================================================










// ------------------------------------------------------------------------------
template< typename T >
class ModelsimSignal
{
template< typename U > friend class ModelsimModule;  
  
protected:
  mtiSignalIdT mSignal;
  T mData;

public:
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
    
  T& value()
  {
    return mData;
  }    
  
  T& operator* ()
  {
    return mData;
  }   

  T* operator-> ()
  {
    return &mData;
  }   

  void set( const T& aRef )
  {
    mData = aRef;
  }  
    
};

// template< typename T >
// std::ostream& operator<< ( std::ostream& aStr , const ModelsimSignal< T >& aArg )
// {
  // return ( aStr << aArg.mData );
// }
// ------------------------------------------------------------------------------









// ------------------------------------------------------------------------------
template< typename T , std::size_t size>
class ModelsimPipeline : public ModelsimSignal< std::array< T , size > >
{
public:

  void set0( const T& aRef )
  {
    for( int i=(size-1); i!=0 ; --i ) this->mData.at(i) = this->mData.at(i-1); // SHOULD CHECK mSignal ticks to get direction
    this->mData.at(0) = aRef;
  } 

  T& value0()
  {
    return this->mData.at(0);
  }  
  
};
// ------------------------------------------------------------------------------




