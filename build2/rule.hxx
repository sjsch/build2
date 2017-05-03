// file      : build2/rule.hxx -*- C++ -*-
// copyright : Copyright (c) 2014-2017 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#ifndef BUILD2_RULE_HXX
#define BUILD2_RULE_HXX

#include <build2/types.hxx>
#include <build2/utility.hxx>

#include <build2/target.hxx>
#include <build2/operation.hxx>

namespace build2
{
  class match_result
  {
  public:
    bool result;

    // If set, then this is a recipe's action. It must override the original
    // action. Normally it is "unconditional inner operation". Only
    // noop_recipe can be overridden.
    //
    // It is passed to rule::apply() so that prerequisites are matched for
    // this action. It is also passed to target::recipe() so that if someone
    // is matching this target for this action, we won't end-up re-matching
    // it. However, the recipe itself is executed with the original action
    // so that it can adjust its logic, if necessary.
    //
    action recipe_action = action ();

    explicit
    operator bool () const {return result;}

    // Note that the from-bool constructor is intentionally implicit so that
    // we can return true/false from match().
    //
    match_result (bool r): result (r) {}
    match_result (bool r, action a): result (r), recipe_action (a) {}
  };

  // Once a rule is registered (for a scope), it is treated as immutable. If
  // you need to modify some state (e.g., counters or some such), then make
  // sure it is MT-safe.
  //
  // Note that match() may not be followed by apply() or be called several
  // times before the following apply() (see resolve_group_members()) which
  // means that it should be idempotent. The target_data object in the call
  // to match() may not be the same as target.
  //
  // match() can also be called by another rules (see cc/install).
  //
  class rule
  {
  public:
    virtual match_result
    match (action, target&, const string& hint) const = 0;

    virtual recipe
    apply (action, target&) const = 0;
  };

  // Fallback rule that only matches if the file exists.
  //
  class file_rule: public rule
  {
  public:
    file_rule () {}

    virtual match_result
    match (action, target&, const string&) const override;

    virtual recipe
    apply (action, target&) const override;

    static const file_rule instance;
  };

  class alias_rule: public rule
  {
  public:
    alias_rule () {}

    virtual match_result
    match (action, target&, const string&) const override;

    virtual recipe
    apply (action, target&) const override;

    static const alias_rule instance;
  };

  class fsdir_rule: public rule
  {
  public:
    fsdir_rule () {}

    virtual match_result
    match (action, target&, const string&) const override;

    virtual recipe
    apply (action, target&) const override;

    static target_state
    perform_update (action, const target&);

    static target_state
    perform_clean (action, const target&);

    // Sometimes, as an optimization, we want to emulate execute_direct()
    // of fsdir{} without the overhead of switching to the execute phase.
    //
    static void
    perform_update_direct (action, const target&);

    static const fsdir_rule instance;
  };

  // Fallback rule that always matches and does nothing.
  //
  class fallback_rule: public build2::rule
  {
  public:
    fallback_rule () {}

    virtual match_result
    match (action, target&, const string&) const override
    {
      return true;
    }

    virtual recipe
    apply (action, target&) const override {return noop_recipe;}

    static const fallback_rule instance;
  };
}

#endif // BUILD2_RULE_HXX