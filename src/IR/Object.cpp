//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Intermediary Representation object handling.
//
//-----------------------------------------------------------------------------

#include "IR/Object.hpp"

#include "IR/Exp.hpp"
#include "IR/IArchive.hpp"
#include "IR/Linkage.hpp"
#include "IR/OArchive.hpp"
#include "IR/Program.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace GDCC
{
   namespace IR
   {
      //
      // Object constructor
      //
      Object::Object(Core::String name) :
         glyph{name},
         initi{},
         linka{Linkage::None},
         space{AddrBase::Cpy, Core::STR_},
         value{0},
         words{0},

         alias{false},
         alloc{false},
         defin{false}
      {
      }

      //
      // Object destructor
      //
      Object::~Object()
      {
      }

      //
      // Object::allocValue
      //
      void Object::allocValue(Program &prog)
      {
         for(;; ++value)
         {
            auto lo = value;
            auto hi = words + lo;

            for(auto const &obj : prog.rangeObject())
            {
               if(obj.alloc || &obj == this || obj.space != space)
                  continue;

               auto objLo = obj.value;
               auto objHi = obj.words + objLo;

               if((objLo <= lo && lo < objHi) || (objLo < hi && hi < objHi))
                  goto nextValue;

               if((lo <= objLo && objLo < hi) || (lo < objHi && objHi < hi))
                  goto nextValue;
            }

            break;

         nextValue:;
         }

         alloc = false;
      }

      //
      // operator OArchive << Object
      //
      OArchive &operator << (OArchive &out, Object const &in)
      {
         return out
            << in.initi
            << in.linka
            << in.space
            << in.value
            << in.words
            << in.alias
            << in.alloc
            << in.defin
         ;
      }

      //
      // operator IArchive >> Object
      //
      IArchive &operator >> (IArchive &in, Object &out)
      {
         in
            >> out.initi
            >> out.linka
            >> out.space
            >> out.value
            >> out.words
         ;

         out.alias = GetIR<bool>(in);
         out.alloc = GetIR<bool>(in);
         out.defin = GetIR<bool>(in);

         return in;
      }
   }
}

// EOF

