#ifndef __aspeller_typo_edit_distance_hh__
#define __aspeller_typo_edit_distance_hh__

#include "cache.hpp"
#include "matrix.hpp"

namespace acommon {
  class Config;
}

namespace aspeller {

  class Language;

  using namespace acommon;

  typedef unsigned char NormalizedChar;
  struct NormalizedString {
    const NormalizedChar * data; // expected to be null terminated 
    unsigned size;
    ParmString str() const {return ParmString(reinterpret_cast<const char *>(data), size);}
    NormalizedString(const NormalizedChar * s, unsigned sz)
      : data(s), size(sz) {}
  };

  struct TypoEditDistanceInfo : public Cacheable {
    int missing; // the cost of having to insert a character
    int swap;    // the cost of swapping two adjecent letters
    short * data; // memory for repl and extra
    ShortMatrix repl; // the cost of replacing one letter with another
    ShortMatrix extra; // the cost of removing an extra letter

    int repl_dis1; // the cost of replace when the distance is 1
    int repl_dis2; //    "          "     otherwise
    int extra_dis1;// 
    int extra_dis2;//

    int case_mismatch; // currently unused by typo_edit_distance

    int max; // maximum edit dist

    NormalizedChar to_normalized_[256];
    int max_normalized;

    NormalizedChar to_normalized(char c) const {
      return to_normalized_[(unsigned char)c];}
    
    // IMPORTANT: It is still necessary to initialize and fill in
    //            repl and extra
  private:
    TypoEditDistanceInfo(int m = 85,  int s = 60, 
                         int r1 = 70, int r = 110, 
                         int e1 = 70, int e = 100,
                         int cm = 50)
      : missing(m), swap(s), data(0) 
      , repl_dis1(r1), repl_dis2(r)
      , extra_dis1(e1), extra_dis2(e)
      , case_mismatch(50)
      , max(-1) {set_max();}
    void set_max();
  public:
    ~TypoEditDistanceInfo() {if (data) free(data);}

    String keyboard;
    typedef const Config CacheConfig;
    typedef const Language CacheConfig2;
    typedef const char * CacheKey;
    bool cache_key_eq(const char * kb) const {return keyboard == kb;}
    static PosibErr<TypoEditDistanceInfo *> get_new(const char *, const Config *, const Language *);
  private:
    TypoEditDistanceInfo(const TypoEditDistanceInfo &);
    void operator=(const TypoEditDistanceInfo &);
  };

  PosibErr<void> setup(CachePtr<const TypoEditDistanceInfo> & res,
                       const Config * c, const Language * l, ParmString kb);

  // typo_edit_distance finds the shortest edit distance. 
  // Preconditions:
  // max(strlen(word), strlen(target))*max(of the edit weights) <= 2^15
  // word,target are not null pointers
  // w.repl and w.extra are square matrices
  // word,target have been converted to "normalized" form using w.to_normalized, which implies
  //   the maximum character value is less than the size of w.repl and w.extra 
  // Returns:
  //   the edit distance between a and b

  // the running time is tightly asymptotically bounded by strlen(a)*strlen(b)

  short typo_edit_distance(NormalizedString word, 
			   NormalizedString target,
			   const TypoEditDistanceInfo & w);
}

#endif
