// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#include <pegtl/pegtl.hh>

// First experiment on how to generate tree structures while parsing.

namespace sexpression
{
   using namespace pegtl;

   // Base class for our tree-like data structure ...

   struct node_base
	 : private nocopy< node_base >
   {
      virtual ~node_base() {}

      virtual void v_print( std::ostream & ) const = 0;
   };

   inline std::ostream & operator<< ( std::ostream & o, const std::shared_ptr< node_base > & p )
   {
      if ( p ) {
	 p->v_print( o );
      }
      else {
	 o << "nil";
      }
      return o;
   }

   // ... and a derived class for a pair of pointers to nodes,
   // i.e. we are building lists/trees from pairs as in Lisp or
   // Scheme ...

   struct cons_node
	 : public node_base
   {
      cons_node( const std::shared_ptr< node_base > & a,
		 const std::shared_ptr< node_base > & d )
	    : m_car( a ),
	      m_cdr( d )
      { }

      const std::shared_ptr< node_base > m_car;
      const std::shared_ptr< node_base > m_cdr;

      void v_print( std::ostream & o ) const
      {
	 o << "( " << m_car << " . " << m_cdr << " )";
      }
   };

   // ... and have one atomic type of node, a token that stores
   // a string and some debug information; the debug information
   // will contain the marker in the input location the token value
   // was read (not yet implemented).

   struct token_node
	 : public node_base
   {
      token_node( const std::string & value,
		  const std::string & debug )
	    : m_value( value ),
	      m_debug( debug )
      { }

      const std::string m_value;
      const std::string m_debug;

      void v_print( std::ostream & o ) const
      {
	 o << m_value;
      }
   };

   // This class is a combined action and rule; this is too much work, either
   // I'm not using my library as best as possible yet (which, given its young
   // age, would be no surprise), or I need to extend the core rules by something
   // appropriate.

   template< typename Head, typename Tail >
   struct make_cons
   {
      typedef seq< Head, Tail > key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Head, Tail >();
      }

      template< bool Must, typename Input, typename Debug >
      static bool s_match( Input & in, Debug & de, std::shared_ptr< node_base > & result )
      {
	 marker< Input > p( in );

	 std::shared_ptr< node_base > car;
	 std::shared_ptr< node_base > cdr;

	 if ( ! de.template match< Must, Head >( in, car ) ) {
	    return p( false );
	 }
	 if ( ! de.template match< Must, Tail >( in, cdr ) ) {
	    return p( false );
	 }
	 result = std::make_shared< cons_node >( car, cdr );
	 return p( true );
      }
   };

   // This class is an action that, in the grammar below, is associated with the rule for
   // a token; it creates a new token and "returns" it in the passed-in "result"; this is
   // in line with the whole architecture: The caller of a rule must provide the return
   // value, and pass it as argument.

   struct token_action
   {
      static void matched( const std::string & token, std::shared_ptr< node_base > & result )
      {
	 result = std::make_shared< token_node >( token, "no debug information here yet" );
      }
   };

   struct comment
	 : ifmust< one< ';' >, until_eol > {};

   struct separator
	 : sor< comment, white_plus > {};

   struct read_atom
	 : pad< action< plus< digit >, token_action >, separator > {};

   struct read_expr;
   struct read_tail;

   struct read_cons
	 : make_cons< read_expr, read_tail > {};

   struct read_tail
	 : sor< pad_one< ')', separator >, read_cons > {};

   struct read_list
	 : ifmust< pad_one< '(', separator >, read_cons > {};

   struct read_expr
	 : sor< read_list, read_atom > {};

   struct read_file
	 : until< read_expr, until< separator, eof > > {};

} // sexpression

int main( int argc, char ** argv )
{
   for ( int arg = 1; arg < argc; ++arg ) {
      std::shared_ptr< sexpression::node_base > result;
      if ( pegtl::trace_parse_string_nothrow< sexpression::read_file >( true, pegtl::read_string( argv[ arg ] ), result ) ) {
	 PEGTL_PRINT( "input from file " << argv[ arg ] << " produced result " << result );
      }
      else {
	 PEGTL_PRINT( "input from file " << argv[ arg ] << " invalid" );
      }
   }
   return 0;
}
