// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2026
// Imperial College London Particles community
// -----------------------------------------------------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <regex>


// ------------------------------------------------------------------------------
// Utility
#include <format>
#include <array>
#include <cstdint>

std::ostream& operator<<( std::ostream& aStr , const std::uint8_t& aArg )
{
    aStr << static_cast<int>( aArg );
    return aStr;
}

template< typename T , std::size_t Size >
std::ostream& operator<< ( std::ostream& aStr , const std::array< T , Size >& aArg )
{
  
  std::cout << std::hex << std::showbase;

  for( int i(0); i!=Size; ++i ) aStr << std::format( "[{}]" , i ) << aArg[i] << " ";
  return aStr;
}

template< typename T >
std::ostream& operator<< ( std::ostream& aStr , const std::vector< T >& aArg )
{
  
  std::cout << std::hex << std::showbase;

  for( int i(0); i!=aArg.size(); ++i ) aStr << std::format( "[{}]" , i ) << aArg[i] << " ";
  return aStr;
}
// ------------------------------------------------------------------------------







template < typename Derived >
struct magic
{    
  template< typename U > void Apply( const U& aFn )       { static_cast<       Derived* >( this )->Implement( aFn ); }
  template< typename U > void Apply( const U& aFn ) const { static_cast< const Derived* >( this )->Implement( aFn ); }  

  template< typename... U >
  void magic_set( const U&... aB )
  {
    this->Apply( [&]( auto&&... params ){ ( (params=aB) , ... ); } ); // Variadic lambda invoking a C++17 fold-expression
  }

  const std::vector< std::string >& MagicFields() const
  {
    static const std::vector<std::string> v = MagicDelimeter( static_cast< const Derived* >( this )->MagicString() ); 
    return v;
  }
  
private:
  static std::vector< std::string > MagicDelimeter( const std::string& aStr )
  {
    const std::regex lRegex( "\\s*,\\s*" ); 
    return std::vector<std::string>( std::sregex_token_iterator( aStr.begin(), aStr.end(), lRegex , -1 ), std::sregex_token_iterator() );  
  }  
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
bool MagicCompare ( const magic< T >& aA , const U&... aB )
{
  bool lBool;
  aA.Apply( [&]( auto&&... params ){ lBool = ( (params==aB) && ... ); } ); // Variadic lambda invoking a C++17 fold-expression
  return lBool;
}

template< typename T >
bool operator== ( const magic< T >& aA , const magic< T >& aB )
{  
  bool lBool;
  aB.Apply( [&]( auto&... params ){ lBool = MagicCompare( aA , params... ); } ); // Variadic lambda
  return lBool;
}


#define MAGIC( ... ) \
  template< typename __________U__________ > void Implement( const __________U__________& aFn )       { aFn( __VA_ARGS__ ); } \
  template< typename __________U__________ > void Implement( const __________U__________& aFn ) const { aFn( __VA_ARGS__ ); } \
  std::string MagicString() const { return #__VA_ARGS__ ; }
