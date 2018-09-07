// From https://tismith.id.au/2014/template-metaprogramming-fun-part3.html
// Author: Toby Smith

#ifndef META_LIST_H
#define META_LIST_H
#include <iostream>
//#include "conditional.hpp"
namespace Meta 
{
   template< bool CONDITION, class THEN, class ELSE > struct IF {};

   template<class THEN, class ELSE> struct IF< false, THEN, ELSE > {
      //typedef ELSE TEST;
      using RET = ELSE;
   };

   template<class THEN, class ELSE> struct IF< true, THEN, ELSE > {
      //typedef THEN TEST;
      using RET = THEN;
   };


   // basic lists and their constructors
   struct EmptyList {
      enum { LENGTH = 0 };
   };  /// Used as the last item in the list

   template< typename a, typename L > struct LIST 
   {
      using HEAD = a;
      using TAIL = L;
       enum { LENGTH = 1 + L::LENGTH };
   };

   template< typename a > struct LIST< a, EmptyList > 
   {
      using HEAD = a;
      using TAIL = EmptyList;
      enum { LENGTH = 1 };
   };

   template<typename a> struct LIST1 {
      typedef LIST<a, EmptyList > TYPE;
   };
   template<typename a, typename b> struct LIST2 {
      typedef LIST<a, LIST<b, EmptyList > > TYPE;
   };
   template<typename a, typename b, typename c> struct LIST3 {
      typedef LIST<a, LIST<b, LIST<c, EmptyList > > > TYPE;
   };
   template<typename a, typename b, typename c, typename d> struct LIST4 {
      typedef LIST<a, LIST<b, LIST<c, LIST<d, EmptyList > > > > TYPE;
   };
   template<typename a, typename b, typename c, typename d, typename e> struct LIST5 {
      typedef LIST<a, LIST<b, LIST<c, LIST<d, LIST<e, EmptyList > > > > > TYPE;
   };

   // printing lists to std::cout
   template <class L> struct ListPrinter
   {
      template< typename OUT > static inline void print(OUT &out)
      {
         //return out;
         //out << std::endl;
      }
   };

   template <typename a> struct ListPrinter<LIST<a, EmptyList> >
   {
      template< typename OUT > static inline void print(OUT &out)
      {
         out << a::DIM;
         ListPrinter<EmptyList>::print(out);
      }
   };

   template <typename a, class TAIL> struct ListPrinter<LIST<a, TAIL> >
   {
      template< typename OUT> static inline void print(OUT &out)
      {
         out << a::DIM << ", ";
         ListPrinter<TAIL>::print(out);
      }
   };

   // prepend
   template< typename a, class L> struct PREPEND
   {
      typedef LIST<a, EmptyList> TYPE;
   };

   template< typename a, typename b, class TAIL> struct PREPEND<a, LIST<b, TAIL> > 
   {
      typedef LIST<a, LIST< b, TAIL> > TYPE;
   };

   // tail
   template<class L>
   struct TAIL {
      typedef EmptyList TYPE;
   };

   template<typename a, class T>
   struct TAIL<LIST<a, T> > {
      typedef T TYPE;
   };

   //template< class L> struct SUM {
   //   enum { RESULT = 0 };
   //};

   ////include all the template parameters that are used here for 
   ////a template specialisation
   //template< typename a, class TAIL> struct SUM< LIST< a, TAIL> > 
   //{
   //   enum { RESULT = a + SUM<TAIL>::RESULT };
   //};

   // map over lists
   //template< class L, template <int> class F>
   //struct MAP {
   //   typedef EmptyList TYPE;
   //};

   //template< typename a, class TAIL, template <typename> class F> struct MAP<LIST<a, TAIL>, F> 
   //{
   //   //I need the typename, otherwise it thinks the MAP::TYPE is a value
   //   typedef LIST< F< a >::DIM, typename MAP<TAIL, F>::TYPE > TYPE;
   //};

   //// map reduce over lists
   //template< class L, template <int> class F, template <int, int> class R, int BASE>
   //struct MAP_REDUCE {
   //   static const int RESULT = BASE;
   //};

   ////include all the template parameters that are used here
   //template< typename a, class TAIL, template <int> class F, template <int, int> class R, int BASE>
   ////but only specialse based on what the original template defintion was
   //struct MAP_REDUCE< LIST< a, TAIL>, F, R, BASE> {
   //   static const int RESULT = R< F< a >::VALUE, MAP_REDUCE< TAIL, F, R, BASE >::RESULT >::VALUE;
   //};

   // tuple data structure
   template <typename A, typename B> struct PAIR {
      typedef A FST;
      typedef B SND;
   };

   //split - splits a list into two halves -- needs a PAIR
   template < class L > struct SPLIT {};

   template <> struct SPLIT <EmptyList> 
   {
      typedef PAIR<EmptyList, EmptyList> TYPE;
   };

   template <typename a> struct SPLIT <LIST<a, EmptyList> > 
   {
      typedef PAIR<LIST<a, EmptyList>, EmptyList> TYPE;
   };

   template <typename a, typename b, typename TAIL>
   struct SPLIT<LIST<a, LIST<b, TAIL> > > 
   {
   private:
      typedef typename SPLIT<TAIL>::TYPE _SPLIT_REC;
   public:
      typedef PAIR<
         typename PREPEND<a, typename _SPLIT_REC::FST>::TYPE,
         typename PREPEND<b, typename _SPLIT_REC::SND>::TYPE
      > TYPE;
   };

   // was template <int, int> class P
   //merge - take two sorted lists and return a sorted merged list -- needs an IF
   template < template <typename, typename> class P, class L1, class L2> struct MERGE {};

   template < template <typename, typename> class P, class L2>
   struct MERGE< P, EmptyList, L2> {
      typedef L2 TYPE;
   };

   template < template <typename, typename> class P, class L1>
   struct MERGE< P, L1, EmptyList > {
      typedef L1 TYPE;
   };

   template < template <typename, typename> class P, typename A1, class TAIL1, typename A2, class TAIL2>
   struct MERGE< P, LIST<A1, TAIL1>, LIST<A2, TAIL2> > 
   {
      typedef typename IF< P < A1, A2 >::VALUE,
         PREPEND< A1, typename MERGE <P, TAIL1, LIST<A2, TAIL2> >::TYPE >,
         PREPEND< A2, typename MERGE <P, LIST<A1, TAIL1>, TAIL2>::TYPE > >
         ::RET::TYPE TYPE;
   };

   //merge sort -- uses split and merge
   //Uses the algorithm from <http://en.literateprograms.org/Merge_sort_(Haskell)>
   template < template <typename, typename> class P, class L >
   struct SORT {
   private:
      typedef typename SPLIT<L>::TYPE _SPLIT_LIST;
      typedef typename SORT<P, typename _SPLIT_LIST::FST>::TYPE _L1;
      typedef typename SORT<P, typename _SPLIT_LIST::SND>::TYPE _L2;
   public:
      typedef typename MERGE<P, _L1, _L2>::TYPE TYPE;
   };

   template < template <typename, typename> class P > struct SORT<P, EmptyList> 
   {
      //typedef EmptyList TYPE;
      using TYPE = EmptyList;
   };

   template < template <typename, typename> class P, typename a>
   struct SORT<P, LIST<a, EmptyList> > 
   {
      //typedef LIST<a, EmptyList> TYPE;
      using TYPE = LIST<a, EmptyList >;
   };



}
#endif
