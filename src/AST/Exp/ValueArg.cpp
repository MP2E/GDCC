//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Abstract Syntax Tree Arg values.
//
//-----------------------------------------------------------------------------

#include "AST/Exp/ValueArg.hpp"

#include "AST/Type.hpp"

#include "IR/Exp.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace GDCC
{
   namespace AST
   {
      //
      // Exp_ValueArg constructor
      //
      Exp_ValueArg::Exp_ValueArg(Arg const &arg_, Core::Origin pos_) :
         Super{pos_}, arg{arg_}
      {
      }

      //
      // Exp_ValueArg::v_genStmnt
      //
      void Exp_ValueArg::v_genStmnt(IR::Block &block, Function *fn, Arg const &dst) const
      {
         genStmntMove(block, fn, dst, arg);
      }

      //
      // Exp_ValueArg::v_getArg
      //
      Arg Exp_ValueArg::v_getArg() const
      {
         return arg;
      }

      //
      // Exp_ValueArg::v_getIRExp
      //
      IR::Exp::CRef Exp_ValueArg::v_getIRExp() const
      {
         if(arg.type->getQualAddr().base == IR::AddrBase::Lit)
            return arg.data->getIRExp();

         return Exp::v_getIRExp();
      }

      //
      // Exp_ValueArg::v_getType
      //
      Type::CRef Exp_ValueArg::v_getType() const
      {
         return arg.type;
      }

      //
      // Exp_ValueArg::v_isEffect
      //
      bool Exp_ValueArg::v_isEffect() const
      {
         return arg.data->isEffect();
      }

      //
      // Exp_ValueArg::v_isIRExp
      //
      bool Exp_ValueArg::v_isIRExp() const
      {
         if(arg.type->getQualAddr().base == IR::AddrBase::Lit)
            return arg.data->isIRExp();

         return false;
      }

      //
      // ExpCreate_ValueArg
      //
      Exp::CRef ExpCreate_ValueArg(Arg const &arg, Core::Origin pos)
      {
         return static_cast<Exp::CRef>(new Exp_ValueArg(arg, pos));
      }
   }
}

// EOF

