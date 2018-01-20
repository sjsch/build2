// file      : build2/bin/rule.hxx -*- C++ -*-
// copyright : Copyright (c) 2014-2017 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#ifndef BUILD2_BIN_RULE_HXX
#define BUILD2_BIN_RULE_HXX

#include <build2/types.hxx>
#include <build2/utility.hxx>

#include <build2/rule.hxx>

namespace build2
{
  namespace bin
  {
    // "Fail rule" for obj{}, bmi{}, and libu{} that issues diagnostics if
    // someone tries to build any of these groups directly.
    //
    class fail_rule: public rule
    {
    public:
      fail_rule () {}

      virtual bool
      match (action, target&, const string&) const override;

      virtual recipe
      apply (action, target&) const override;
    };

    // Pass-through to group members rule, similar to alias.
    //
    class lib_rule: public rule
    {
    public:
      lib_rule () {}

      virtual bool
      match (action, target&, const string&) const override;

      virtual recipe
      apply (action, target&) const override;

      static target_state
      perform (action, const target&);
    };
  }
}

#endif // BUILD2_BIN_RULE_HXX
