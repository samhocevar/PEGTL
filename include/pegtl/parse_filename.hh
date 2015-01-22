// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_PARSE_FILENAME_HH
#define COHI_PEGTL_PARSE_FILENAME_HH

namespace pegtl
{
   // Functions to parse input given the filename as std::string.

   // Wrapper functions that add another convenience layer: instantiation
   // and initialisation of the input class. See file parse_generic.hh for
   // the wrapped functions.

   // The *_parse_file_* functions set up the parser to parse a file; the
   // functions here use mmap(2), alternatively the function read_string()
   // defined in utilities.hh can be used to read a file into a std::string
   // (which can then be parsed by one of the parse_string functions).

   template< typename TopRule, typename Location = dummy_location, typename ... States >
   bool dummy_parse_file_throws( const std::string & filename, States && ... st )
   {
      file_mapper fm( filename );
      forward_input< file_mapper::iterator, Location > in( fm.begin(), fm.end() );
      return dummy_parse_throws< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = dummy_location, typename ... States >
   bool dummy_parse_file_nothrow( const std::string & filename, States && ... st )
   {
      file_mapper fm( filename );
      forward_input< file_mapper::iterator, Location > in( fm.begin(), fm.end() );
      return dummy_parse_nothrow< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool basic_parse_file_throws( const std::string & filename, States && ... st )
   {
      file_mapper fm( filename );
      forward_input< file_mapper::iterator, Location > in( fm.begin(), fm.end() );
      return basic_parse_throws< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool basic_parse_file_nothrow( const std::string & filename, States && ... st )
   {
      file_mapper fm( filename );
      forward_input< file_mapper::iterator, Location > in( fm.begin(), fm.end() );
      return basic_parse_nothrow< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool trace_parse_file_throws( const bool trace, const std::string & filename, States && ... st )
   {
      file_mapper fm( filename );
      forward_input< file_mapper::iterator, Location > in( fm.begin(), fm.end() );
      return trace_parse_throws< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool trace_parse_file_nothrow( const bool trace, const std::string & filename, States && ... st )
   {
      file_mapper fm( filename );
      forward_input< file_mapper::iterator, Location > in( fm.begin(), fm.end() );
      return trace_parse_nothrow< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

   // Please read the comment on the smart_parse_* functions in parse_generic.hh!

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool smart_parse_file_throws( const bool trace, const std::string & filename, States && ... st )
   {
      file_mapper fm( filename );
      forward_input< file_mapper::iterator, Location > in( fm.begin(), fm.end() );
      return smart_parse_throws< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool smart_parse_file_nothrow( const bool trace, const std::string & filename, States && ... st )
   {
      file_mapper fm( filename );
      forward_input< file_mapper::iterator, Location > in( fm.begin(), fm.end() );
      return smart_parse_nothrow< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

} // pegtl

#endif
