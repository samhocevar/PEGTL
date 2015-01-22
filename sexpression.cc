// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#include <pegtl.hh>

// First experiment on how to generate tree structures while parsing.

namespace sexpr
{
   using namespace pegtl;

   struct node_base
	 : private utils::nocopy< node_base >
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
	 o << "( " << m_car << " " << m_cdr << " )";
      }
   };

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

   template< typename Head, typename Tail >
   struct make_cons
   {
      static std::string key()
      {
	 return typeid( make_cons ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< Head, Tail >();
      }

      template< typename Input, typename Debug >
      static bool s_match( Input & in, Debug & de, std::shared_ptr< node_base > & result )
      {
	 position< Input > p( in );

	 std::shared_ptr< node_base > car;
	 std::shared_ptr< node_base > cdr;

	 if ( ! de.template match< Head >( in, car ) ) {
	    return p( false );
	 }
	 if ( ! de.template match< Tail >( in, cdr ) ) {
	    return p( false );
	 }
	 result.reset( new cons_node( car, cdr ) );
	 return p( true );
      }
   };

   template< typename Rule >
   struct make_token
   {
      static std::string key()
      {
	 return typeid( make_token ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< Rule >();
      }

      template< typename Input, typename Debug >
      static bool s_match( Input & in, Debug & de, std::shared_ptr< node_base > & result )
      {
	 position< Input > p( in );

	 if ( ! de.template match< Rule >( in ) ) {
	    return p( false );
	 }
	 result.reset( new token_node( std::string( p.here().get(), in.here().get() ), "foo" ) );
	 return p( true );
      }
   };

   struct comment
	 : ifmust< one< ';' >, until_eol_or_eof > {};

   struct separator
	 : sor< comment, white_plus > {};

   struct read_atom
	 : pad< make_token< plus< digit > >, separator > {};

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

} // sexpr

int main( int argc, char ** argv )
{
   for ( int arg = 1; arg < argc; ++arg ) {
      std::shared_ptr< sexpr::node_base > result;
      if ( pegtl::parse< sexpr::read_file >( argv[ arg ], arg, result ) ) {
	 UTILS_PRINT( "input " << argv[ arg ] << " result " << result );
      }
      else {
	 UTILS_PRINT( "input " << argv[ arg ] << " invalid" );
      }
   }
   return 0;
}
