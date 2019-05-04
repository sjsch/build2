#include <build2/moc/rule.hxx>
#include <build2/moc/target.hxx>

#include <build2/cxx/target.hxx>
#include <build2/algorithm.hxx>
#include <build2/depdb.hxx>

namespace build2
{
  namespace moc
  {
    bool compile_rule::
    match (action a, target& xt, const string&) const
    {
      if (!xt.is_a<moc_cxx> ())
        return false;

      bool found (false);
      for (prerequisite_member p: group_prerequisite_members(a, xt))
      {
        if (include (a, xt, p) != include_type::normal)
          continue;

        found |= p.is_a<cxx::hxx> ();
      }

      return found;
    }

    target_state
    perform_update (action a, const target& xt)
    {
      const cxx::cxx& t (static_cast<const moc_cxx&> (xt));
      const path& tp (t.path());
      const scope& rs (t.root_scope ());

      timestamp mt (t.load_mtime ());
      auto pr (execute_prerequisites<cxx::hxx> (a, xt, mt));

      bool update (!pr.first);
      target_state ts (update ? target_state::changed : *pr.first);

      const cxx::hxx& i (pr.second);
      const path& ip (i.path ());

      depdb dd (tp + ".d");
      {
        dd.expect ("moc.compile 1");
        dd.expect (cast<string> (rs["moc.checksum"]));

        sha256 cs;
        hash_options (cs, t, "moc.options");

        dd.expect (cs.string ());
        dd.expect (i.path ());
      }

      if (dd.writing () || dd.mtime > mt)
        update = true;
      dd.close ();

      if (!update)
        return ts;

      const process_path &moc (cast<process_path> (rs["moc.path"]));
      cstrings args{moc.recall_string()};
      append_options (args, t, "moc.options");
      args.push_back(ip.string().c_str());
      args.push_back("-o");
      args.push_back(tp.string().c_str());
      args.push_back(nullptr);

      if (verb >= 2)
        print_process(args);
      else if (verb)
        text << "moc " << i;

      if (!dry_run) {
        run(moc, args);
        dd.check_mtime (tp);
      }

      t.mtime(system_clock::now());

      return target_state::changed;
    }

    recipe compile_rule::
    apply (action a, target& xt) const
    {
      cxx::cxx& t (static_cast<cxx::cxx&> (xt));

      t.derive_path();
      inject_fsdir (a, xt);

      match_prerequisite_members (a, xt);

      switch (a)
      {
      case perform_update_id:
        return perform_update;
      case perform_clean_id:
        return perform_clean;
      default:
        return noop_recipe;
      }
    }
  }
}
