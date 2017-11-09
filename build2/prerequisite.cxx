// file      : build2/prerequisite.cxx -*- C++ -*-
// copyright : Copyright (c) 2014-2017 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#include <build2/prerequisite.hxx>

#include <build2/scope.hxx>
#include <build2/target.hxx>
#include <build2/context.hxx>
#include <build2/diagnostics.hxx>

using namespace std;

namespace build2
{
  // prerequisite_key
  //
  const optional<string> prerequisite_key::nullproj = nullopt;

  ostream&
  operator<< (ostream& os, const prerequisite_key& pk)
  {
    if (pk.proj)
      os << *pk.proj << '%';
    //
    // Don't print scope if we are project-qualified or the prerequisite's
    // directory is absolute. In both these cases the scope is not used to
    // resolve it to target.
    //
    else if (!pk.tk.dir->absolute ())
    {
      // Avoid printing './' in './:...', similar to what we do for the
      // directory in target_key.
      //
      const dir_path& s (pk.scope->out_path ());

      if (stream_verb (os) < 2)
      {
        const string& r (diag_relative (s, false));

        if (!r.empty ())
          os << r << ':';
      }
      else
        os << s << ':';
    }

    return os << pk.tk;
  }

  // prerequisite
  //
  static inline optional<string>
  to_ext (const string* e)
  {
    return e != nullptr ? optional<string> (*e) : nullopt;
  }

  prerequisite::
  prerequisite (const target_type& t)
      : proj (nullopt),
        type (t.type ()),
        dir (t.dir),
        out (t.out),   // @@ If it's empty, then we treat as undetermined?
        name (t.name),
        ext (to_ext (t.ext ())),
        scope (t.base_scope ()),
        target (&t),
        vars (false /* global */)
  {
  }

  bool prerequisite::
  belongs (const target_type& t) const
  {
    const auto& p (t.prerequisites ());
    return !(p.empty () || this < &p.front () || this > &p.back ());
  }

  value& prerequisite::
  append (const variable& var, const target_type& t)
  {
    if (value* r = vars.find_to_modify (var))
      return *r;

    value& r (assign (var)); // NULL.

    // Note: pretty similar logic to target::append().
    //
    lookup l (t.find_original (var).first);

    if (l.defined ())
      r = *l; // Copy value (and type) from the target/outer scope.

    return r;
  }
}
