# Applying using variadic functions

```
void aFunction () {}

template< typename S , typename... T >
void aFunction ( S& aArg , T&&... aRest ) {
  //  ... Do something with aArg ...
  //  ...        and then        ...
  aFunction( ++aBuf , aRest... );
}

aMagic.Apply( []( auto&&... params ){ aFunction( params... ); } );
```

# Applying using C++20 fold expressions

```
template< typename S >
void aFunction2 ( S& aArg ) {
  //  ... Do something with aArg ...
}

aMagic.Apply( []( auto&&... params ){ ( aFunction2( params ) , ... ); } );
```
