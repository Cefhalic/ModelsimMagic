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
template< typename T >
void mti_get_int( mtiSignalIdT& aMtiId , T& aWord )
{
  auto lKind = mti_GetTypeKind( mti_GetSignalType( aMtiId ) );
  
  if( lKind == MTI_TYPE_SCALAR ) {
    aWord = mti_GetSignalValue(aMtiId);  
  } else {        
    auto Size = mti_TickLength( mti_GetSignalType( aMtiId ) );
    if( Size > 8*sizeof(T) ) throw std::runtime_error( std::format( "FLI size ({}) != C++ size ({})" , Size , 8*sizeof(T) ) );

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
void mti_get( mtiSignalIdT& aMtiId ,  uint8_t& aWord ){ mti_get_int( aMtiId , aWord ); }
void mti_get( mtiSignalIdT& aMtiId , uint16_t& aWord ){ mti_get_int( aMtiId , aWord ); }
void mti_get( mtiSignalIdT& aMtiId , uint32_t& aWord ){ mti_get_int( aMtiId , aWord ); }
void mti_get( mtiSignalIdT& aMtiId , uint64_t& aWord ){ mti_get_int( aMtiId , aWord ); }
void mti_get( mtiSignalIdT& aMtiId ,   int8_t& aWord ){ mti_get_int( aMtiId , aWord ); }
void mti_get( mtiSignalIdT& aMtiId ,  int16_t& aWord ){ mti_get_int( aMtiId , aWord ); }
void mti_get( mtiSignalIdT& aMtiId ,  int32_t& aWord ){ mti_get_int( aMtiId , aWord ); }
void mti_get( mtiSignalIdT& aMtiId ,  int64_t& aWord ){ mti_get_int( aMtiId , aWord ); }
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
template< typename T >
void mti_set_int( mtiSignalIdT& aMtiId , const T& aWord )
{
  auto lKind = mti_GetTypeKind( mti_GetSignalType( aMtiId ) );
  
  if( lKind == MTI_TYPE_SCALAR ) {
    mti_SetSignalValue( aMtiId , aWord );  
  } else {
    auto Size = mti_TickLength( mti_GetSignalType( aMtiId ) );
    if( Size > 8*sizeof(T) ) throw std::runtime_error( std::format( "FLI size ({}) != C++ size ({})" , Size , 8*sizeof(T) ) );

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
void mti_set( mtiSignalIdT& aMtiId , const  uint8_t& aWord ){ mti_set_int( aMtiId , aWord ); }
void mti_set( mtiSignalIdT& aMtiId , const uint16_t& aWord ){ mti_set_int( aMtiId , aWord ); }
void mti_set( mtiSignalIdT& aMtiId , const uint32_t& aWord ){ mti_set_int( aMtiId , aWord ); }
void mti_set( mtiSignalIdT& aMtiId , const uint64_t& aWord ){ mti_set_int( aMtiId , aWord ); }
void mti_set( mtiSignalIdT& aMtiId , const   int8_t& aWord ){ mti_set_int( aMtiId , aWord ); }
void mti_set( mtiSignalIdT& aMtiId , const  int16_t& aWord ){ mti_set_int( aMtiId , aWord ); }
void mti_set( mtiSignalIdT& aMtiId , const  int32_t& aWord ){ mti_set_int( aMtiId , aWord ); }
void mti_set( mtiSignalIdT& aMtiId , const  int64_t& aWord ){ mti_set_int( aMtiId , aWord ); }
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
private:
  template< typename U > friend class ModelsimModule;  
  
// protected:
  mtiSignalIdT mSignal;

public:  
  ModelsimSignal() : mSignal( NULL )
  {}

  virtual void callback( const int& aCtr )
  {}

  void connect( const std::string& aName )
  {
    if( mSignal != NULL ) throw std::runtime_error( "Signal already connected" );
    mSignal = mti_FindSignal( (char*)(aName.c_str()) );
    if( mSignal == NULL ) throw std::runtime_error( std::format( "Signal '{}' not found" , aName ) );
    
    // TYPE AND BOUNDS CHECK HERE??????
  }

  T get()
  { 
    if( mSignal == NULL ) throw std::runtime_error( "Signal not connected" );
    T lData;
    mti_get( mSignal , lData ); 
    return lData;
  }

  void set( const T& aData )
  { 
    if( mSignal == NULL ) throw std::runtime_error( "Signal not connected" );
    mti_set( mSignal , aData ); 
  }
  
  std::string name()
  {
    if( mSignal == NULL ) throw std::runtime_error( "Signal not connected" );    
    char* lName = mti_GetSignalNameIndirect( mSignal , NULL , 0 );
    std::string lStr( lName );
    mti_VsimFree( lName );
    return lName;
  }
  
};

// template< typename T >
// std::ostream& operator<< ( std::ostream& aStr , const ModelsimSignal< T >& aArg )
// {
  // return ( aStr << aArg.mData );
// }
// ------------------------------------------------------------------------------
