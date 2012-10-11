#include <sstream>
#include "assembler-parser.tab.h"

using namespace std;

enum Opcode {
   OP_ADD    = 0x0100,
   OP_AND    = 0x0200,
   OP_ASSIGN = 0x0300,
   OP_CALL   = 0x0400,
   OP_CMP    = 0x0500,
   OP_CMPGT  = 0x0600,
   OP_CMPGTE = 0x0700,
   OP_CMPLT  = 0x0800,
   OP_CMPLTE = 0x0900,
   OP_CMPNE  = 0x0A00,
   OP_FUNC   = 0x0B00,
   OP_OR     = 0x0C00,
   OP_PUSH   = 0x0D00,

   OP_STR    = 0x0001,
   OP_INT    = 0x0002,
   OP_DBL    = 0x0003,
   OP_IDENT  = 0x0004,
   OP_NIL    = 0x0005,
};

class Instruction {
public:
   virtual ~Instruction() {}
   virtual void text( ostream& oss ) const = 0;
   virtual void bytecode( ostream& oss ) const = 0;

   static Opcode TokenToOpcode( int token );
};

class ADD : public Instruction {
   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class AND : public Instruction {
   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class ASSIGN : public Instruction {
   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class CALL : public Instruction {
public:
   CALL( const string& ident_, long long argc_ )
      : ident( ident_ ), argc( argc_ ) {}

private:
   string ident;
   long long argc;

   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class CMP : public Instruction {
   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class CMPGT : public Instruction {
   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class CMPGTE : public Instruction {
   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class CMPLT : public Instruction {
   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class CMPLTE : public Instruction {
   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class CMPNE : public Instruction {
   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class FUNCDEF : public Instruction {
public:
   FUNCDEF( const string& ref_, int argc_ )
      : ref( ref_ ), argc( argc_ ) {}

private:
   string ref;
   int argc;

   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class JZ : public Instruction {
public:
   JZ( const string& ref_ )
      : ref( ref_ ) {}

private:
   string ref;

   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class OR : public Instruction {
   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class PUSH : public Instruction {
public:
   PUSH( int type_, long long value_ )
      : type( type_ ) {
      ostringstream oss;
      oss << value_;
      value = oss.str();
   }

   PUSH( int type_, double value_ )
      : type( type_ ) {
      ostringstream oss;
      oss << value_;
      value = oss.str();
   }

   PUSH( int type_, string value_ )
      : type( type_ ), value( value_ ) {}

private:
   int type;
   string value;

   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class POP : public Instruction {
public:
   POP( const string& ident_ )
      : ident( ident_ ) {}

private:
   string ident;

   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class REFERENCEDEF : public Instruction {
public:
   REFERENCEDEF( const string& ref_ )
      : ref( ref_ ) {}

private:
   string ref;

   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class RET : public Instruction {
   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

class STRINGDEF : public Instruction {
public:
   STRINGDEF( const string& ref_, const string& str_ )
      : ref( ref_ ), str( str_ ) {}

private:
   string ref;
   string str;

   virtual void text( ostream& oss ) const;
   virtual void bytecode( ostream& oss ) const;
};

