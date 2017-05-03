// file      : build2/cxx/target.hxx -*- C++ -*-
// copyright : Copyright (c) 2014-2017 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#ifndef BUILD2_CXX_TARGET_HXX
#define BUILD2_CXX_TARGET_HXX

#include <build2/types.hxx>
#include <build2/utility.hxx>

#include <build2/target.hxx>
#include <build2/cc/target.hxx>

namespace build2
{
  namespace cxx
  {
    using cc::h;
    using cc::c;

    class hxx: public file
    {
    public:
      using file::file;

    public:
      static const target_type static_type;
      virtual const target_type& dynamic_type () const {return static_type;}
    };

    class ixx: public file
    {
    public:
      using file::file;

    public:
      static const target_type static_type;
      virtual const target_type& dynamic_type () const {return static_type;}
    };

    class txx: public file
    {
    public:
      using file::file;

    public:
      static const target_type static_type;
      virtual const target_type& dynamic_type () const {return static_type;}
    };

    class cxx: public cc::cc
    {
    public:
      using cc::cc;

    public:
      static const target_type static_type;
      virtual const target_type& dynamic_type () const {return static_type;}
    };
  }
}

#endif // BUILD2_CXX_TARGET_HXX