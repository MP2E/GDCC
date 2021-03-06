//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Reference counting.
//
//-----------------------------------------------------------------------------

#ifndef GDCC__Core__Counter_H__
#define GDCC__Core__Counter_H__


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// GDCC_Core_CounterPreambleCommonTypes
//
// Used for common definitions for macros below.
//
#define GDCC_Core_CounterPreambleCommonTypes(type, base) \
public: \
   typedef type This; \
   typedef base Super; \
   typedef ::GDCC::Core::CounterPtr<type> Ptr; \
   typedef ::GDCC::Core::CounterPtr<type const> CPtr; \
   friend class ::GDCC::Core::CounterPtr<type>; \
   friend class ::GDCC::Core::CounterPtr<type const>; \
   typedef ::GDCC::Core::CounterRef<type> Ref; \
   typedef ::GDCC::Core::CounterRef<type const> CRef; \
   friend class ::GDCC::Core::CounterRef<type>; \
   friend class ::GDCC::Core::CounterRef<type const>

//
// GDCC_Core_CounterPreambleCommon
//
#define GDCC_Core_CounterPreambleCommon(type, base) \
public: \
   ::GDCC::Core::CounterRef<type> clone() const \
      {return static_cast<::GDCC::Core::CounterRef<type>>(cloneRaw());} \
   virtual char const *getClassName() const {return #type;} \
   GDCC_Core_CounterPreambleCommonTypes(type, base)

//
// GDCC_Core_CounterPreamble
//
#define GDCC_Core_CounterPreamble(type, base) \
private: \
   virtual type *cloneRaw() const {return new type(*this);} \
   GDCC_Core_CounterPreambleCommon(type, base)

//
// GDCC_Core_CounterPreambleAbstract
//
// For use in defining abstract reference-counted classes.
//
#define GDCC_Core_CounterPreambleAbstract(type, base) \
private: \
   virtual type *cloneRaw() const = 0; \
   GDCC_Core_CounterPreambleCommon(type, base)

//
// GDCC_Core_CounterPreambleNoClone
//
// For use in defining reference-counted classes that cannot be copied.
//
#define GDCC_Core_CounterPreambleNoClone(type, base) \
private: \
   virtual type *cloneRaw() const {throw 1;} \
   GDCC_Core_CounterPreambleCommon(type, base)

//
// GDCC_Core_CounterPreambleNoVirtual
//
// For use in defining reference-counted classes without virtuals.
//
#define GDCC_Core_CounterPreambleNoVirtual(type, base) \
public: \
   char const *getClassName() const {return #type;} \
   GDCC_Core_CounterPreambleCommonTypes(type, base)


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace GDCC
{
   namespace Core
   {
      template<typename T> class CounterRef;

      //
      // CounterPtr
      //
      // Reference-counting pointer.
      //
      template<typename T> class CounterPtr
      {
      public:
         // default constructor
         CounterPtr(decltype(nullptr) p_ = nullptr) : p{p_} {}

         // cast Pointer -> CounterPtr
         CounterPtr(T *p_) : p{static_cast<T *>(p_)} {if(p) ++p->refCount;}
         template<typename T2>
         CounterPtr(T2 *p_) : p{static_cast<T *>(p_)} {if(p) ++p->refCount;}

         // cast CounterPtr -> CounterPtr
         CounterPtr(CounterPtr<T> const &p_) :
            p{static_cast<T *>(p_)} {if(p) ++p->refCount;}
         template<typename T2>
         CounterPtr(CounterPtr<T2> const &p_) :
            p{static_cast<T *>(p_)} {if(p) ++p->refCount;}

         // cast CounterRef -> CounterPtr
         CounterPtr(CounterRef<T> const &p_) :
            p{static_cast<T *>(p_)} {++p->refCount;}
         template<typename T2>
         CounterPtr(CounterRef<T2> const &p_) :
            p{static_cast<T *>(p_)} {++p->refCount;}

         // destructor
         ~CounterPtr() {if(p && !--p->refCount) delete p;}

         // cast CounterPtr -> Pointer
         operator T * () const {return p;}

         // operator CounterPtr = nullptr
         CounterPtr<T> &operator = (decltype(nullptr) p_)
         {
            if(p && !--p->refCount) delete p; p = p_;
            return *this;
         }

         // operator CounterPtr = Pointer
         CounterPtr<T> &operator = (T *p_)
         {
            T *old = p; if((p = p_)) ++p->refCount;
            if(old && !--old->refCount) delete old;
            return *this;
         }
         template<typename T2>
         CounterPtr<T> &operator = (T2 *p_)
         {
            T *old = p; if((p = p_)) ++p->refCount;
            if(old && !--old->refCount) delete old;
            return *this;
         }

         // operator CounterPtr = CounterPtr
         CounterPtr<T> &operator = (CounterPtr<T> const &p_)
         {
            T *old = p; if((p = static_cast<T *>(p_))) ++p->refCount;
            if(old && !--old->refCount) delete old;
            return *this;
         }
         template<typename T2>
         CounterPtr<T> &operator = (CounterPtr<T2> const &p_)
         {
            T *old = p; if((p = static_cast<T *>(p_))) ++p->refCount;
            if(old && !--old->refCount) delete old;
            return *this;
         }

         // operator CounterPtr = CounterRef
         CounterPtr<T> &operator = (CounterRef<T> const &p_)
         {
            T *old = p; ++(p = static_cast<T *>(p_))->refCount;
            if(old && !--old->refCount) delete old;
            return *this;
         }
         template<typename T2>
         CounterPtr<T> &operator = (CounterRef<T2> const &p_)
         {
            T *old = p; ++(p = static_cast<T *>(p_))->refCount;
            if(old && !--old->refCount) delete old;
            return *this;
         }

         T *operator -> () const {return p;}

         T &operator * () const {return *p;}

         unsigned refCount() const {return p ? p->refCount : 0;}

      private:
         T *p;
      };

      //
      // CounterRef
      //
      // Reference-counting pointer that is never null.
      //
      template<typename T> class CounterRef
      {
      public:
         // default constructor
         CounterRef(decltype(nullptr) p_ = nullptr) = delete;

         // cast Pointer -> CounterRef
         explicit CounterRef(T *p_) : p(static_cast<T *>(p_)) {++p->refCount;}
         template<typename T2>
         explicit CounterRef(T2 *p_) : p(static_cast<T *>(p_)) {++p->refCount;}

         // cast CounterPtr -> CounterRef
         explicit CounterRef(CounterPtr<T> const &p_) :
            p(static_cast<T *>(p_)) {++p->refCount;}
         template<typename T2>
         explicit CounterRef(CounterPtr<T2> const &p_) :
            p(static_cast<T *>(p_)) {++p->refCount;}

         // cast CounterRef -> CounterRef
         CounterRef(CounterRef<T> const &p_) :
            p(static_cast<T *>(p_)) {++p->refCount;}
         template<typename T2>
         CounterRef(CounterRef<T2> const &p_) :
            p(static_cast<T *>(p_)) {++p->refCount;}

         // destructor
         ~CounterRef() {if(!--p->refCount) delete p;}

         // cast CounterRef -> Pointer
         operator T * () const {return p;}

         // CounterRef = CounterRef
         CounterRef<T> &operator = (CounterRef<T> const &p_)
         {
            T *old = p; ++(p = static_cast<T *>(p_))->refCount;
            if(!--old->refCount) delete old;
            return *this;
         }
         template<typename T2>
         CounterRef<T> &operator = (CounterRef<T2> const &p_)
         {
            T *old = p; ++(p = static_cast<T2 *>(p_))->refCount;
            if(!--old->refCount) delete old;
            return *this;
         }

         T *operator -> () const {return p;}

         T &operator * () const {return *p;}

         unsigned refCount() const {return p->refCount;}

      private:
         T *p;
      };

      //
      // Counter
      //
      // Reference-counted base class.
      //
      class Counter
      {
         GDCC_Core_CounterPreamble(GDCC::Core::Counter, GDCC::Core::Counter);

      protected:
         Counter() : refCount{0} {}
         Counter(Counter const &) : refCount{0} {}
         Counter(Counter &&) : refCount{0} {}
         virtual ~Counter() {}

         Counter &operator = (Counter const &) {return *this;}
         Counter &operator = (Counter &&) {return *this;}

         // Protected so that derived classes can use CounterPtr.
         // Mutable for CounterPtr-to-const.
         mutable unsigned refCount;
      };

      //
      // PlainCounter
      //
      // Reference-counted base class with no virtuals.
      //
      class PlainCounter
      {
         GDCC_Core_CounterPreambleNoVirtual(
            GDCC::Core::PlainCounter, GDCC::Core::PlainCounter);

      protected:
         PlainCounter() : refCount{0} {}
         PlainCounter(PlainCounter const &) : refCount{0} {}
         PlainCounter(PlainCounter &&) : refCount{0} {}

         PlainCounter &operator = (PlainCounter const &) {return *this;}
         PlainCounter &operator = (PlainCounter &&) {return *this;}

         // Protected so that derived classes can use CounterPtr.
         // Mutable for CounterPtr-to-const.
         mutable unsigned refCount;
      };
   }
}

#endif//GDCC__Core__Counter_H__

