// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2026
// Imperial College London Particles community
// -----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "mti.h"
#include "Magic.hpp"
#include "ModelsimSignal.hpp"
#include "ModelsimUtils.hpp"

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
      ++mCounter;
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

  static void SimStatus( void *aStruct , int aRunStatus )
  { 
    try{
      SimTime( true ); // Update the time resolution at start (and end) of run
      if( aRunStatus ) ((T*) aStruct)->pre_run(); 
      else             ((T*) aStruct)->post_run(); 
    } catch( const std::exception& aExc ) {
      mti_PrintMessage( aExc.what() );
      mti_FatalError();      
    }
  }  

  static void Quit( void *aStruct )
  {
    try{
      ((T*) aStruct)->quit(); 
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
  
  virtual void pre_run()
  {}  

  virtual void post_run()
  {} 

  virtual void quit()
  {} 
    
public:
  // FLI initialization algorithm
  static void Initialization( const std::string& aClkName = "clk" )
  {   
    try {        
      T* lStruct = new T();     

      // Connect the simulation-status callback
      mti_AddSimStatusCB( SimStatus , lStruct );
      mti_AddRestartCB( Quit , lStruct );
      mti_AddQuitCB( Quit , lStruct );

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
