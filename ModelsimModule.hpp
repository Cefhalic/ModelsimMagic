// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2026
// Imperial College London Particles community
// -----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "mti.h"
#include "Magic.hpp"
#include "ModelsimSignal.hpp"

template< typename T >
struct ModelsimModule : public magic< T >
{ 
private:
  ModelsimSignal<bool> mClk;
  int mCounter;

  virtual void Process()
  {
      if( !mClk.get() ) return; // On falling_edge, return  
      callback( mCounter );
      this->Apply( [&]( auto&&... params ){ ( params.callback( mCounter ) , ... ); } ); // Apply the callback to each signal 
      UpdateCounter( mCounter );
  }
   
  // The callback that will be called on each event
  static void Process( void *aStruct )
  { 
    try {
      ((T*) aStruct)->Process();        
    } catch( const std::exception& aExc ) {
      mti_PrintMessage( aExc.what() );
      mti_FatalError();      
    }
  }

protected:
  ModelsimModule( const int& aCounter = 0 ) : mClk() , mCounter( aCounter )
  {}
  
  virtual void callback( const int& aCtr )
  {}
  
  virtual void UpdateCounter( int& aCtr )
  {
    ++aCtr;
  }
  
public:
  // FLI initialization algorithm
  static void Initialization( const std::string& aClkName = "clk" )
  {   
    try {        
      T* lStruct = new T();     

      // Connect the clock and use it as the trigger
      lStruct->mClk.connect( aClkName );
      mtiProcessIdT lProcess = mti_CreateProcessWithPriority( NULL , ModelsimModule::Process , lStruct , MTI_PROC_POSTPONED );
      mti_Sensitize( lProcess , lStruct->mClk.mSignal , MTI_EVENT );    

      // Connect the magic fields by name
      auto lIt = lStruct->MagicFields().begin();
      lStruct->Apply( [&]( auto&&... params ){ ( params.connect( lIt++ -> c_str() ) , ... ); } );      

      lStruct->callback( 0 );
          
    } catch( const std::exception& aExc ) {
      mti_PrintMessage( aExc.what() );
      mti_FatalError();      
    }      
  }
  
};

#define ExportModule( MODULE ) extern "C" void MODULE( mtiRegionIdT , char* , mtiInterfaceListT* , mtiInterfaceListT* ) { MODULE::Initialization(); }
