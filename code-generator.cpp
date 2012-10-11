#include "code-generator.h"
#include "bit-stream.h"

void
ADD::text( ostream& oss ) const
{
   oss << "ADD" << endl;
}

void
ADD::bytecode( ostream& oss ) const
{
   oss << U16( OP_ADD ) << endl;
}

void
AND::text( ostream& oss ) const
{
   oss << "AND" << endl;
}

void
AND::bytecode( ostream& oss ) const
{
   oss << U16( OP_ADD ) << endl;
}

void
ASSIGN::text( ostream& oss ) const
{
   oss << "ASSIGN" << endl;
}

void
ASSIGN::bytecode( ostream& oss ) const
{
   oss << U16( OP_ASSIGN );
}

void
CALL::text( ostream& oss ) const
{
   oss << "CALL " << ident << " " << argc << endl;
}

void
CALL::bytecode( ostream& oss ) const
{
   oss << U16( OP_CALL ) << STR( ident ) << U16( argc );
}

void
CMP::text( ostream& oss ) const
{
   oss << "CMP" << endl;
}

void
CMP::bytecode( ostream& oss ) const
{
   oss << U16( OP_CMP );
}

void
CMPGT::text( ostream& oss ) const
{
   oss << "CMPGT" << endl;
}

void
CMPGT::bytecode( ostream& oss ) const
{
   oss << U16( OP_CMPGT );
}

void
CMPGTE::text( ostream& oss ) const
{
   oss << "CMPGTE" << endl;
}

void
CMPGTE::bytecode( ostream& oss ) const
{
   oss << U16( OP_CMPGTE );
}

void
CMPLT::text( ostream& oss ) const
{
   oss << "CMPLT" << endl;
}

void
CMPLT::bytecode( ostream& oss ) const
{
   oss << U16( OP_CMPLT );
}

void
CMPLTE::text( ostream& oss ) const
{
   oss << "CMPLTE" << endl;
}

void
CMPLTE::bytecode( ostream& oss ) const
{
   oss << U16( OP_CMPLTE );
}

void
CMPNE::text( ostream& oss ) const
{
   oss << "CMPNE" << endl;
}

void
CMPNE::bytecode( ostream& oss ) const
{
   oss << U16( OP_CMPNE );
}

void
FUNCDEF::text( ostream& oss ) const
{
   REF::found_references[ ref ] = oss.tellp();
   oss << ref << " " << TFUNC << " " << argc << endl;
}

void
FUNCDEF::bytecode( ostream& oss ) const
{
   REF::found_references[ ref ] = oss.tellp();
   oss << U16( OP_FUNC );
}

void
JZ::text( ostream& oss ) const
{
   oss << "JZ " << ref << endl;
}

void
JZ::bytecode( ostream& oss ) const
{
   oss << U16( OP_CMP ) << REF( ref );
}

void
OR::text( ostream& oss ) const
{
   oss << "OR" << endl;
}

void
OR::bytecode( ostream& oss ) const
{
   oss << U16( OP_OR );
}

void
PUSH::text( ostream& oss ) const
{
   oss << "PUSH " << type << " " << value << endl;
}

void
PUSH::bytecode( ostream& oss ) const
{
   switch( type ) {
   case TSTR: oss << U16( OP_PUSH | OP_STR ) << REF( value ); break;
   case TFUNC: oss << U16( OP_PUSH | OP_FUNC ) << REF( value ); break;
   case TIDENT: oss << U16( OP_PUSH | OP_IDENT ) << STR( value ); break;
   case TINT: oss << U16( OP_PUSH | OP_INT ) << U64( value ); break;
   case TDOUBLE: oss << U16( OP_PUSH | OP_DBL ) << DBL( value ); break;
   case TNIL: oss << U16( OP_PUSH | OP_NIL ); break;
   default:
      printf( "Unknown type for PUSH: %d\n", type );
      exit( -1 );
      break;
   }
}

void
POP::text( ostream& oss ) const
{
   oss << "POP " << ident << endl;
}

void
POP::bytecode( ostream& oss ) const
{
}

void
REFERENCEDEF::text( ostream& oss ) const
{
   REF::found_references[ ref ] = oss.tellp();
   oss << ref << endl;
}

void
REFERENCEDEF::bytecode( ostream& oss ) const
{
   REF::found_references[ ref ] = oss.tellp();
}

void
RET::text( ostream& oss ) const
{
   oss << "RET" << endl;
}

void
RET::bytecode( ostream& oss ) const
{
}

void
STRINGDEF::text( ostream& oss ) const
{
   REF::found_references[ ref ] = oss.tellp();
   oss << ref << " " << TSTR << " " << ( str.size() ) << " " << str << endl;
}

void
STRINGDEF::bytecode( ostream& oss ) const
{
   REF::found_references[ ref ] = oss.tellp();
}

