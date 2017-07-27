// file      : build2/cc/utility.cxx -*- C++ -*-
// copyright : Copyright (c) 2014-2017 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#include <build2/cc/utility.hxx>

#include <build2/variable.hxx>
#include <build2/algorithm.hxx> // search()

#include <build2/bin/target.hxx>

using namespace std;

namespace build2
{
  namespace cc
  {
    using namespace bin;

    lorder
    link_order (const scope& bs, otype ot)
    {
      // Initialize to suppress 'may be used uninitialized' warning produced
      // by MinGW GCC 5.4.0.
      //
      const char* var (nullptr);

      switch (ot)
      {
      case otype::e: var = "bin.exe.lib";  break;
      case otype::a: var = "bin.liba.lib"; break;
      case otype::s: var = "bin.libs.lib"; break;
      }

      const auto& v (cast<strings> (bs[var]));
      return v[0] == "shared"
        ? v.size () > 1 && v[1] == "static" ? lorder::s_a : lorder::s
        : v.size () > 1 && v[1] == "shared" ? lorder::a_s : lorder::a;
    }

    const target&
    link_member (const bin::libx& x, action a, linfo li)
    {
      if (const libu* u = x.is_a<libu> ())
      {
        otype ot (li.type);
        return search (*u,
                       ot == otype::e ? libue::static_type :
                       ot == otype::a ? libua::static_type :
                       libus::static_type,
                       u->dir, u->out, u->name);
      }
      else
      {
        const lib& l (x.as<lib> ());

        // Make sure group members are resolved.
        //
        group_view gv (resolve_group_members (a, l));
        assert (gv.members != nullptr);

        lorder lo (li.order);

        bool ls (true);
        switch (lo)
        {
        case lorder::a:
        case lorder::a_s:
          ls = false; // Fall through.
        case lorder::s:
        case lorder::s_a:
          {
            if (ls ? l.s == nullptr : l.a == nullptr)
            {
              if (lo == lorder::a_s || lo == lorder::s_a)
                ls = !ls;
              else
                fail << (ls ? "shared" : "static") << " variant of " << l
                     << " is not available";
            }
          }
        }

        return *(ls ? static_cast<const target*> (l.s) : l.a);
      }
    }
  }
}
