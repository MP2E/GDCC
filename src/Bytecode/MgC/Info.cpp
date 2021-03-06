//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// MageCraft target information and handling class.
//
//-----------------------------------------------------------------------------

#include "Bytecode/MgC/Info.hpp"

#include "IR/Function.hpp"

#include "IR/Exp/ValueGlyph.hpp"
#include "IR/Exp/ValueRoot.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace GDCC
{
   namespace Bytecode
   {
      namespace MgC
      {
         //
         // Info::CheckArg
         //
         void Info::CheckArg(IR::Arg const &arg, Core::Origin const &pos)
         {
            switch(arg.a)
            {
            case IR::ArgBase::Lit: break;
            case IR::ArgBase::Nul: break;
            case IR::ArgBase::Stk: break;

            case IR::ArgBase::LocArs:
               CheckArg(*arg.aLocArs.idx, pos);
               break;

            case IR::ArgBase::LocReg:
               CheckArgB(*arg.aLocReg.idx, IR::ArgBase::Lit, pos);
               break;

            default:
               std::cerr << "ERROR: " << pos << ": MgC cannot use Arg: "
                  << arg.a << '\n';
               throw EXIT_FAILURE;
            }
         }
      }
   }
}

// EOF

