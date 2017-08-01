// file      : build2/install/rule.hxx -*- C++ -*-
// copyright : Copyright (c) 2014-2017 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#ifndef BUILD2_INSTALL_RULE_HXX
#define BUILD2_INSTALL_RULE_HXX

#include <build2/types.hxx>
#include <build2/utility.hxx>

#include <build2/rule.hxx>
#include <build2/target.hxx>
#include <build2/operation.hxx>

namespace build2
{
  namespace install
  {
    class alias_rule: public rule
    {
    public:
      static const alias_rule instance;

      alias_rule () {}

      virtual match_result
      match (action, target&, const string&) const override;

      virtual recipe
      apply (action, target&) const override;

      // Return NULL if this prerequisite should be ignored and pointer to its
      // target otherwise. The default implementation accepts all prerequsites.
      //
      virtual const target*
      filter (action, const target&, prerequisite_member) const;
    };

    struct install_dir;

    class file_rule: public rule
    {
    public:
      static const file_rule instance;

      file_rule () {}

      virtual match_result
      match (action, target&, const string&) const override;

      virtual recipe
      apply (action, target&) const override;

      // Return NULL if this prerequisite should be ignored and pointer to its
      // target otherwise. The default implementation ignores prerequsites that
      // are outside of this target's project.
      //
      virtual const target*
      filter (action, const target&, prerequisite_member) const;

      // Extra installation hooks.
      //
      using install_dir = install::install_dir;

      virtual void
      install_extra (const file&, const install_dir&) const;

      // Return true if anything was uninstalled.
      //
      virtual bool
      uninstall_extra (const file&, const install_dir&) const;

      // Installation "commands".
      //
      // If verbose is false, then only print the command at verbosity level 2
      // or higher.
      //
    public:
      // Install a symlink: base/link -> target.
      //
      static void
      install_l (const install_dir& base,
                 const path& target,
                 const path& link,
                 bool verbose);

      // Uninstall a file or symlink:
      //
      // uninstall <target> <base>/  rm <base>/<target>.leaf (); name empty
      // uninstall <target> <name>   rm <base>/<name>; target can be NULL
      //
      // Return false if nothing has been removed (i.e., the file does not
      // exist).
      //
      static bool
      uninstall_f (const install_dir& base,
                   const file* t,
                   const path& name,
                   bool verbose);

    protected:
      target_state
      perform_install (action, const target&) const;

      target_state
      perform_uninstall (action, const target&) const;
    };
  }
}

#endif // BUILD2_INSTALL_RULE_HXX
