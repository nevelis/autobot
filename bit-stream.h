#ifndef _BIT_STREAM_H_
#define _BIT_STREAM_H_

#include <map>
#include <sstream>
#include <string>

using namespace std;

class U16 {
public:
   U16( unsigned short i_ )
      : i( i_ ) {}
private:
   friend ostream& operator<<( ostream& oss, const U16& u16 )
   {
      oss.write( ( char* ) &u16.i, sizeof( u16.i ) );
      oss.flush();

      return oss;
   }

   unsigned short i;
};

class U32 {
public:
   U32( unsigned int i_ )
      : i( i_ ) {}
private:
   friend ostream& operator<<( ostream& oss, const U32& u32 )
   {
      oss.write( ( char* ) &u32.i, sizeof( u32.i ) );
      oss.flush();

      return oss;
   }

   unsigned int i;
};

class U64 {
public:
   U64( unsigned int i_ )
      : i( i_ ) {}

   U64( const string& s )
   {
      istringstream iss( s );
      iss >> i;
   }

private:
   friend ostream& operator<<( ostream& oss, const U64& u64 )
   {
      oss.write( ( char* ) &u64.i, sizeof( unsigned long long ) );
      oss.flush();

      return oss;
   }

   unsigned long long i;
};

class DBL {
public:
   DBL( double d_ )
      : d( d_ ) {}

   DBL( const string& s )
   {
      istringstream iss( s );
      iss >> d;
   }

private:
   friend ostream& operator<<( ostream& oss, const DBL& dbl )
   {
      oss.write( ( char* ) &dbl.d, sizeof( double ) );
   }

   double d;
};

class STR {
public:
   STR( const string& s_ )
      : s( s_ ) {}

private:
   friend ostream& operator<<( ostream& oss, const STR& str )
   {
      oss << U16( str.s.size() );
      oss.write( str.s.data(), str.s.size() );

      return oss;
   }

   string s;
};

class REF {
public:
   REF( const string& name_ )
      : name( name_ ), pos( 0 ) {}

   static std::map<int, string> to_resolve;
   static std::map<string, int> found_references;

private:
   friend ostream& operator<<( ostream& oss, const REF& ref )
   {
      // Remember the position of the address so it can be linked later, and
      // insert a placeholder
      to_resolve[ oss.tellp() ] = ref.name;
      oss << U32( 0 );
   }

   string name;
   int pos;
};

#endif
