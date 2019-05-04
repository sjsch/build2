#include <build2/moc/init.hxx>
#include <build2/moc/rule.hxx>
#include <build2/moc/target.hxx>

#include <build2/cxx/target.hxx>
#include <build2/scope.hxx>
#include <build2/config/utility.hxx>

namespace build2
{
  namespace moc
  {
    static const compile_rule compile_rule_;

    bool
    config_init (scope& rs,
                 scope& bs,
                 const location& l,
                 unique_ptr<module_base>&,
                 bool first,
                 bool optional,
                 const variable_map& hints)
    {
      tracer trace ("moc::config_init");

      if (first)
      {
        auto& v (var_pool.rw (rs));

        v.insert<path> ("config.moc", true);
	v.insert<strings> ("config.moc.options", true);

        v.insert<process_path> ("moc.path");
	v.insert<string> ("moc.checksum");
	v.insert<strings> ("moc.options");

        process_path pp;
        pp = process::path_search ("moc", true, dir_path (), true);

        rs.assign ("moc.path") = move (pp);
        rs.assign ("moc.checksum") = sha256 (pp.effect_string ()).string ();
      }

      bs.assign ("moc.options") += cast_null<strings> (
	config::optional (rs, "config.moc.options"));

      if (verb >= 3)
      {
	diag_record dr (text);
	dr << "moc " << project (rs) << '@' << rs.out_path ();
      }

      return true;
    }

    bool
    init (scope& rs,
          scope& bs,
          const location& l,
          unique_ptr<module_base>&,
          bool,
          bool optional,
          const variable_map& hints)
    {
      tracer trace ("moc::init");

      if (!cast_false<bool> (bs["moc.config.loaded"]))
      {
        if (!load_module (rs, bs, "moc.config", l, optional, hints))
          return false;
      }

      {
	auto& t (bs.target_types);
	t.insert<moc_cxx> ();
      }

      // Register rules.
      {
        auto& r (bs.rules);
        r.insert<moc_cxx> (perform_update_id, "moc.compile", compile_rule_);
        r.insert<moc_cxx> (perform_clean_id, "moc.compile", compile_rule_);
      }

      return true;
    }
  }
}
