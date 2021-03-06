// -*- C++ -*-
//
// This file was generated by CLI, a command line interface
// compiler for C++.
//

#ifndef BUILD2_B_OPTIONS_HXX
#define BUILD2_B_OPTIONS_HXX

// Begin prologue.
//
//
// End prologue.

#include <deque>
#include <iosfwd>
#include <string>
#include <cstddef>
#include <exception>

#ifndef CLI_POTENTIALLY_UNUSED
#  if defined(_MSC_VER) || defined(__xlC__)
#    define CLI_POTENTIALLY_UNUSED(x) (void*)&x
#  else
#    define CLI_POTENTIALLY_UNUSED(x) (void)x
#  endif
#endif

namespace build2
{
  namespace cl
  {
    class usage_para
    {
      public:
      enum value
      {
        none,
        text,
        option
      };

      usage_para (value);

      operator value () const 
      {
        return v_;
      }

      private:
      value v_;
    };

    class unknown_mode
    {
      public:
      enum value
      {
        skip,
        stop,
        fail
      };

      unknown_mode (value);

      operator value () const 
      {
        return v_;
      }

      private:
      value v_;
    };

    // Exceptions.
    //

    class exception: public std::exception
    {
      public:
      virtual void
      print (::std::ostream&) const = 0;
    };

    ::std::ostream&
    operator<< (::std::ostream&, const exception&);

    class unknown_option: public exception
    {
      public:
      virtual
      ~unknown_option () throw ();

      unknown_option (const std::string& option);

      const std::string&
      option () const;

      virtual void
      print (::std::ostream&) const;

      virtual const char*
      what () const throw ();

      private:
      std::string option_;
    };

    class unknown_argument: public exception
    {
      public:
      virtual
      ~unknown_argument () throw ();

      unknown_argument (const std::string& argument);

      const std::string&
      argument () const;

      virtual void
      print (::std::ostream&) const;

      virtual const char*
      what () const throw ();

      private:
      std::string argument_;
    };

    class missing_value: public exception
    {
      public:
      virtual
      ~missing_value () throw ();

      missing_value (const std::string& option);

      const std::string&
      option () const;

      virtual void
      print (::std::ostream&) const;

      virtual const char*
      what () const throw ();

      private:
      std::string option_;
    };

    class invalid_value: public exception
    {
      public:
      virtual
      ~invalid_value () throw ();

      invalid_value (const std::string& option,
                     const std::string& value,
                     const std::string& message = std::string ());

      const std::string&
      option () const;

      const std::string&
      value () const;

      const std::string&
      message () const;

      virtual void
      print (::std::ostream&) const;

      virtual const char*
      what () const throw ();

      private:
      std::string option_;
      std::string value_;
      std::string message_;
    };

    class eos_reached: public exception
    {
      public:
      virtual void
      print (::std::ostream&) const;

      virtual const char*
      what () const throw ();
    };

    class file_io_failure: public exception
    {
      public:
      virtual
      ~file_io_failure () throw ();

      file_io_failure (const std::string& file);

      const std::string&
      file () const;

      virtual void
      print (::std::ostream&) const;

      virtual const char*
      what () const throw ();

      private:
      std::string file_;
    };

    class unmatched_quote: public exception
    {
      public:
      virtual
      ~unmatched_quote () throw ();

      unmatched_quote (const std::string& argument);

      const std::string&
      argument () const;

      virtual void
      print (::std::ostream&) const;

      virtual const char*
      what () const throw ();

      private:
      std::string argument_;
    };

    // Command line argument scanner interface.
    //
    // The values returned by next() are guaranteed to be valid
    // for the two previous arguments up until a call to a third
    // peek() or next().
    //
    class scanner
    {
      public:
      virtual
      ~scanner ();

      virtual bool
      more () = 0;

      virtual const char*
      peek () = 0;

      virtual const char*
      next () = 0;

      virtual void
      skip () = 0;
    };

    class argv_scanner: public scanner
    {
      public:
      argv_scanner (int& argc, char** argv, bool erase = false);
      argv_scanner (int start, int& argc, char** argv, bool erase = false);

      int
      end () const;

      virtual bool
      more ();

      virtual const char*
      peek ();

      virtual const char*
      next ();

      virtual void
      skip ();

      private:
      int i_;
      int& argc_;
      char** argv_;
      bool erase_;
    };

    class argv_file_scanner: public argv_scanner
    {
      public:
      argv_file_scanner (int& argc,
                         char** argv,
                         const std::string& option,
                         bool erase = false);

      argv_file_scanner (int start,
                         int& argc,
                         char** argv,
                         const std::string& option,
                         bool erase = false);

      struct option_info
      {
        // If search_func is not NULL, it is called, with the arg
        // value as the second argument, to locate the options file.
        // If it returns an empty string, then the file is ignored.
        //
        const char* option;
        std::string (*search_func) (const char*, void* arg);
        void* arg;
      };

      argv_file_scanner (int& argc,
                          char** argv,
                          const option_info* options,
                          std::size_t options_count,
                          bool erase = false);

      argv_file_scanner (int start,
                         int& argc,
                         char** argv,
                         const option_info* options,
                         std::size_t options_count,
                         bool erase = false);

      virtual bool
      more ();

      virtual const char*
      peek ();

      virtual const char*
      next ();

      virtual void
      skip ();

      private:
      const option_info*
      find (const char*) const;

      void
      load (const std::string& file);

      typedef argv_scanner base;

      const std::string option_;
      option_info option_info_;
      const option_info* options_;
      std::size_t options_count_;

      std::deque<std::string> args_;

      // Circular buffer of two arguments.
      //
      std::string hold_[2];
      std::size_t i_;

      bool skip_;
    };

    template <typename X>
    struct parser;
  }
}

#include <set>

#include <build2/types.hxx>

namespace build2
{
  class options
  {
    public:
    options ();

    // Return true if anything has been parsed.
    //
    bool
    parse (int& argc,
           char** argv,
           bool erase = false,
           ::build2::cl::unknown_mode option = ::build2::cl::unknown_mode::fail,
           ::build2::cl::unknown_mode argument = ::build2::cl::unknown_mode::stop);

    bool
    parse (int start,
           int& argc,
           char** argv,
           bool erase = false,
           ::build2::cl::unknown_mode option = ::build2::cl::unknown_mode::fail,
           ::build2::cl::unknown_mode argument = ::build2::cl::unknown_mode::stop);

    bool
    parse (int& argc,
           char** argv,
           int& end,
           bool erase = false,
           ::build2::cl::unknown_mode option = ::build2::cl::unknown_mode::fail,
           ::build2::cl::unknown_mode argument = ::build2::cl::unknown_mode::stop);

    bool
    parse (int start,
           int& argc,
           char** argv,
           int& end,
           bool erase = false,
           ::build2::cl::unknown_mode option = ::build2::cl::unknown_mode::fail,
           ::build2::cl::unknown_mode argument = ::build2::cl::unknown_mode::stop);

    bool
    parse (::build2::cl::scanner&,
           ::build2::cl::unknown_mode option = ::build2::cl::unknown_mode::fail,
           ::build2::cl::unknown_mode argument = ::build2::cl::unknown_mode::stop);

    // Option accessors.
    //
    const bool&
    v () const;

    const bool&
    V () const;

    const bool&
    quiet () const;

    const uint16_t&
    verbose () const;

    bool
    verbose_specified () const;

    const bool&
    stat () const;

    const std::set<string>&
    dump () const;

    bool
    dump_specified () const;

    const bool&
    progress () const;

    const bool&
    no_progress () const;

    const size_t&
    jobs () const;

    bool
    jobs_specified () const;

    const size_t&
    max_jobs () const;

    bool
    max_jobs_specified () const;

    const size_t&
    queue_depth () const;

    bool
    queue_depth_specified () const;

    const size_t&
    max_stack () const;

    bool
    max_stack_specified () const;

    const bool&
    serial_stop () const;

    const bool&
    dry_run () const;

    const bool&
    match_only () const;

    const bool&
    structured_result () const;

    const bool&
    mtime_check () const;

    const bool&
    no_mtime_check () const;

    const bool&
    no_column () const;

    const bool&
    no_line () const;

    const path&
    buildfile () const;

    bool
    buildfile_specified () const;

    const path&
    config_guess () const;

    bool
    config_guess_specified () const;

    const path&
    config_sub () const;

    bool
    config_sub_specified () const;

    const string&
    pager () const;

    bool
    pager_specified () const;

    const strings&
    pager_option () const;

    bool
    pager_option_specified () const;

    const bool&
    help () const;

    const bool&
    version () const;

    // Print usage information.
    //
    static ::build2::cl::usage_para
    print_usage (::std::ostream&,
                 ::build2::cl::usage_para = ::build2::cl::usage_para::none);

    // Implementation details.
    //
    protected:
    bool
    _parse (const char*, ::build2::cl::scanner&);

    private:
    bool
    _parse (::build2::cl::scanner&,
            ::build2::cl::unknown_mode option,
            ::build2::cl::unknown_mode argument);

    public:
    bool v_;
    bool V_;
    bool quiet_;
    uint16_t verbose_;
    bool verbose_specified_;
    bool stat_;
    std::set<string> dump_;
    bool dump_specified_;
    bool progress_;
    bool no_progress_;
    size_t jobs_;
    bool jobs_specified_;
    size_t max_jobs_;
    bool max_jobs_specified_;
    size_t queue_depth_;
    bool queue_depth_specified_;
    size_t max_stack_;
    bool max_stack_specified_;
    bool serial_stop_;
    bool dry_run_;
    bool match_only_;
    bool structured_result_;
    bool mtime_check_;
    bool no_mtime_check_;
    bool no_column_;
    bool no_line_;
    path buildfile_;
    bool buildfile_specified_;
    path config_guess_;
    bool config_guess_specified_;
    path config_sub_;
    bool config_sub_specified_;
    string pager_;
    bool pager_specified_;
    strings pager_option_;
    bool pager_option_specified_;
    bool help_;
    bool version_;
  };
}

// Print page usage information.
//
namespace build2
{
  ::build2::cl::usage_para
  print_b_usage (::std::ostream&,
                 ::build2::cl::usage_para = ::build2::cl::usage_para::none);
}

#include <build2/b-options.ixx>

// Begin epilogue.
//
//
// End epilogue.

#endif // BUILD2_B_OPTIONS_HXX
