// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2026
// Imperial College London Particles community
// -----------------------------------------------------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <vector>
#include <regex>

template < typename Derived >
struct magic
{    
  template< typename U > void Apply( const U& aFn )       { static_cast<       Derived* >( this )->Implement( aFn ); }
  template< typename U > void Apply( const U& aFn ) const { static_cast< const Derived* >( this )->Implement( aFn ); }  

  template< typename... U >
  void set( const U&... aB )
  {
    this->Apply( [&]( auto&&... params ){ ( (params=aB) , ... ); } ); // Variadic lambda invoking a C++17 fold-expression
  }
  
  // std::string MagicFields() const { return static_cast< const Derived* >( this )->ImplementMagicFields(); }
};


template< typename T >
std::ostream& operator<< ( std::ostream& aStr , const magic< T >& aArg )
{
  aStr << "(";
  aArg.Apply( [ &aStr ]( auto&&... params ){ ( ( aStr << ' ' << params ) , ... ); } ); // Variadic lambda invoking a C++17 fold-expression
  aStr << " )";
  return aStr;
}

// template< typename T >
// std::istream& operator>> ( std::istream& aStr , magic< T >& aArg )
// {
  // aArg.Apply( [ &aStr ]( auto&&... params ){ ( ( aStr >> params ) , ... ); } ); // Variadic lambda invoking a C++17 fold-expression
  // return aStr;
// }

template< typename T , typename... U >
bool Compare ( const magic< T >& aA , const U&... aB )
{
  bool lBool;
  aA.Apply( [&]( auto&&... params ){ lBool = ( (params==aB) && ... ); } ); // Variadic lambda invoking a C++17 fold-expression
  return lBool;
}

template< typename T >
bool operator== ( const magic< T >& aA , const magic< T >& aB )
{  
  bool lBool;
  aB.Apply( [&]( auto&... params ){ lBool = Compare( aA , params... ); } ); // Variadic lambda
  return lBool;
}

static std::vector< std::string > MagicDelimeter( const std::string& aStr )
{
  const std::regex lRegex( "\\s*,\\s*" ); 
  return std::vector<std::string>( std::sregex_token_iterator( aStr.begin(), aStr.end(), lRegex , -1 ), std::sregex_token_iterator() );  
}

#define MAGIC( ... ) \
  template< typename __________U__________ > void Implement( const __________U__________& aFn )       { aFn( __VA_ARGS__ ); } \
  template< typename __________U__________ > void Implement( const __________U__________& aFn ) const { aFn( __VA_ARGS__ ); } \
  const std::vector< std::string >& MagicFields() const { static const std::vector<std::string> v = MagicDelimeter( #__VA_ARGS__ ); return v; }

