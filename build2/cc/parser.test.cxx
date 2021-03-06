// file      : build2/cc/parser.test.cxx -*- C++ -*-
// copyright : Copyright (c) 2014-2019 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#include <cassert>
#include <iostream>

#include <build2/types.hxx>
#include <build2/utility.hxx>

#include <build2/cc/parser.hxx>

using namespace std;
using namespace butl;

namespace build2
{
  namespace cc
  {
    // Usage: argv[0] [<file>]
    //
    int
    main (int argc, char* argv[])
    {
      try
      {
        const char* file;

        ifdstream is;
        if (argc > 1)
        {
          file = argv[1];
          is.open (file);
        }
        else
        {
          file = "stdin";
          is.open (fddup (stdin_fd ()));
        }

        parser p;
        unit u (p.parse (is, path (file)));
        unit_type ut (u.type);

        for (const module_import& m: u.module_info.imports)
          cout << (m.exported ? "export " : "")
               << "import " << m.name << ';' << endl;

        if (ut == unit_type::module_iface || ut == unit_type::module_impl)
          cout << (ut == unit_type::module_iface ? "export " : "")
               << "module " << u.module_info.name << ';' << endl;
      }
      catch (const failed&)
      {
        return 1;
      }

      return 0;
    }
  }
}

int
main (int argc, char* argv[])
{
  return build2::cc::main (argc, argv);
}
