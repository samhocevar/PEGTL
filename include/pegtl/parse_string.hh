// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_PARSE_STRING_HH
#define COHI_PEGTL_PARSE_STRING_HH

namespace pegtl
{
   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool dummy_parse_arg( const std::string & string, const int arg, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "argv[" + to_string( arg ) + "]" );
      return dummy_parse< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool dummy_parse_file( const std::string & filename, Class && ... cl )
   {
      const std::string string = read_string( filename );
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "filename " + filename );
      return dummy_parse< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool dummy_parse_string( const std::string & string, const std::string & source, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), source );
      return dummy_parse< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool basic_parse_arg( const std::string & string, const int arg, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "argv[" + to_string( arg ) + "]" );
      return basic_parse< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool basic_parse_file( const std::string & filename, Class && ... cl )
   {
      const std::string string = read_string( filename );
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "filename " + filename );
      return basic_parse< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool basic_parse_string( const std::string & string, const std::string & source, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), source );
      return basic_parse< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool trace_parse_arg( const bool trace, const std::string & string, const int arg, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "argv[" + to_string( arg ) + "\"" );
      return trace_parse< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool trace_parse_file( const bool trace, const std::string & filename, Class && ... cl )
   {
      const std::string string = read_string( filename );
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "filename " + filename );
      return trace_parse< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool trace_parse_string( const bool trace, const std::string & string, const std::string & source, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), source );
      return trace_parse< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

} // pegtl

#endif
