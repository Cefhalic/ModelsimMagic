// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2026
// Imperial College London Particles community
// -----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "mti.h"
#include "Magic.hpp"

template< typename T >
struct ModelsimModule : public magic< T >
{ 
  ModelsimSignal<bool> clk;
  
  virtual void Handler() = 0;  
   
  // The callback that will be called on each event
  static void Process( void *aStruct )
  { 
    try {
      T* lStruct = (T*) aStruct;        
      lStruct->Apply( []( auto&&... params ){ ( params.get_signal() , ... ); } ); // Get the current value on all magic members from Modelsim to FLI via Variadic lambda invoking a C++17 fold-expression       
      if( ! lStruct->clk.mData ) return; // On falling_edge, return        
      lStruct->Handler();   
      lStruct->Apply( []( auto&&... params ){ ( params.set_signal() , ... ); } ); // Set the updated value on all magic members from Modelsim to FLI via Variadic lambda invoking a C++17 fold-expression  
    } catch( const std::exception& aExc ) {
      mti_PrintMessage( aExc.what() );
      mti_FatalError();      
    }
  }

  // FLI initialization algorithm
  static void Initialization()
  {   
    try {        
      T* lStruct = new T();
      auto lIt = lStruct->MagicFields().begin();
      lStruct->Apply( [&]( auto&&... params ){ ( params.connect( lIt++ -> c_str() ) , ... ); } );      
      mtiProcessIdT lProcess = mti_CreateProcessWithPriority( NULL , ModelsimModule::Process , lStruct , MTI_PROC_POSTPONED );
      mti_Sensitize( lProcess , lStruct->clk.mSignal , MTI_EVENT );    
    } catch( const std::exception& aExc ) {
      mti_PrintMessage( aExc.what() );
      mti_FatalError();      
    }      
  }

  // std::cout << std::hex << std::showbase << std::boolalpha;
  
};

#define ExportModule( MODULE ) extern "C" void MODULE( mtiRegionIdT , char* , mtiInterfaceListT* , mtiInterfaceListT* ) { MODULE::Initialization(); }
