// file      : build/algorithm.txx -*- C++ -*-
// copyright : Copyright (c) 2014-2015 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

namespace build
{
  template <typename T>
  T*
  execute_prerequisites (action a, target& t, const timestamp& mt, bool& e)
  {
    //@@ Can factor the bulk of it into a non-template code. Can
    // either do a function template that will do dynamic_cast check
    // or can scan the target type info myself. I think latter.
    //
    T* r (nullptr);

    if (t.group != nullptr)
      r = execute_prerequisites<T> (a, *t.group, mt, e);

    for (target* pt: t.prerequisites)
    {
      if (pt == nullptr) // Skip ignored.
        continue;

      target_state ts (execute (a, *pt));

      if (!e)
      {
        // If this is an mtime-based target, then compare timestamps.
        //
        if (auto mpt = dynamic_cast<const mtime_target*> (pt))
        {
          timestamp mp (mpt->mtime ());

          // What do we do if timestamps are equal? This can happen, for
          // example, on filesystems that don't have subsecond resolution.
          // There is not much we can do here except detect the case where
          // the prerequisite was changed in this run which means the
          // action must be executed on the target as well.
          //
          if (mt < mp || (mt == mp && ts == target_state::changed))
            e = true;
        }
        else
        {
          // Otherwise we assume the prerequisite is newer if it was changed.
          //
          if (ts == target_state::changed)
            e = true;
        }
      }

      if (T* tmp = dynamic_cast<T*> (pt))
        r = tmp;
    }

    return r;
  }
}
