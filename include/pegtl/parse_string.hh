// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_PARSE_STRING_HH
#define COHI_PEGTL_PARSE_STRING_HH

namespace pegtl
{
   // Wrapper functions that add another convenience layer: instantiation
   // and initialisation of the input from various sources. See include
   // file parse_generic.hh for the wrapped functions.

   // The *_parse_arg_* functions set up the parser to parse the supplied
   // string, which, for diagnostic purposes, is assumed to be obtained
   // from command line argument argv[ arg ].

   // The *_parse_file_* functions interpret the supplied string as
   // filename and read the entire file for parsing (mmap() based
   // version coming later).

   // The *_parse_string_* functions set up the parser to parse the supplied
   // string, where the second supplied string is assumed to indicate where
   // said string originates, which is again used for diagnostic purposes.

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool dummy_parse_arg_throws( const std::string & string, const int arg, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "argv[" + to_string( arg ) + "]" );
      return dummy_parse_throws< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool dummy_parse_arg_nothrow( const std::string & string, const int arg, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "argv[" + to_string( arg ) + "]" );
      return dummy_parse_nothrow< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool dummy_parse_file_throws( const std::string & filename, Class && ... cl )
   {
      const std::string string = read_string( filename );
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "filename " + filename );
      return dummy_parse_throws< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool dummy_parse_file_nothrow( const std::string & filename, Class && ... cl )
   {
      const std::string string = read_string( filename );
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "filename " + filename );
      return dummy_parse_nothrow< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool dummy_parse_string_throws( const std::string & string, const std::string & source, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), source );
      return dummy_parse_throws< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool dummy_parse_string_nothrow( const std::string & string, const std::string & source, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), source );
      return dummy_parse_nothrow< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool basic_parse_arg_throws( const std::string & string, const int arg, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "argv[" + to_string( arg ) + "]" );
      return basic_parse_throws< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool basic_parse_arg_nothrow( const std::string & string, const int arg, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "argv[" + to_string( arg ) + "]" );
      return basic_parse_nothrow< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool basic_parse_file_throws( const std::string & filename, Class && ... cl )
   {
      const std::string string = read_string( filename );
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "filename " + filename );
      return basic_parse_throws< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool basic_parse_file_nothrow( const std::string & filename, Class && ... cl )
   {
      const std::string string = read_string( filename );
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "filename " + filename );
      return basic_parse_nothrow< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool basic_parse_string_throws( const std::string & string, const std::string & source, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), source );
      return basic_parse_throws< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool basic_parse_string_nothrow( const std::string & string, const std::string & source, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), source );
      return basic_parse_nothrow< TopRule >( in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool trace_parse_arg_throws( const bool trace, const std::string & string, const int arg, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "argv[" + to_string( arg ) + "\"" );
      return trace_parse_throws< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool trace_parse_arg_nothrow( const bool trace, const std::string & string, const int arg, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "argv[" + to_string( arg ) + "\"" );
      return trace_parse_nothrow< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool trace_parse_file_throws( const bool trace, const std::string & filename, Class && ... cl )
   {
      const std::string string = read_string( filename );
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "filename " + filename );
      return trace_parse_throws< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool trace_parse_file_nothrow( const bool trace, const std::string & filename, Class && ... cl )
   {
      const std::string string = read_string( filename );
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "filename " + filename );
      return trace_parse_nothrow< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool trace_parse_string_throws( const bool trace, const std::string & string, const std::string & source, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), source );
      return trace_parse_throws< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool trace_parse_string_nothrow( const bool trace, const std::string & string, const std::string & source, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), source );
      return trace_parse_nothrow< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   // Please read the comment on the smart_parse_* functions in parse_generic.hh!

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool smart_parse_arg_throws( const bool trace, const std::string & string, const int arg, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "argv[" + to_string( arg ) + "\"" );
      return smart_parse_throws< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool smart_parse_arg_nothrow( const bool trace, const std::string & string, const int arg, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "argv[" + to_string( arg ) + "\"" );
      return smart_parse_nothrow< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool smart_parse_file_throws( const bool trace, const std::string & filename, Class && ... cl )
   {
      const std::string string = read_string( filename );
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "filename " + filename );
      return smart_parse_throws< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool smart_parse_file_nothrow( const bool trace, const std::string & filename, Class && ... cl )
   {
      const std::string string = read_string( filename );
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), "filename " + filename );
      return smart_parse_nothrow< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool smart_parse_string_throws( const bool trace, const std::string & string, const std::string & source, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), source );
      return smart_parse_throws< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... Class >
   bool smart_parse_string_nothrow( const bool trace, const std::string & string, const std::string & source, Class && ... cl )
   {
      iterator_input< std::string::const_iterator, Location > in( string.begin(), string.end(), source );
      return smart_parse_nothrow< TopRule >( trace, in, std::forward< Class >( cl ) ... );
   }

} // pegtl

#endif
