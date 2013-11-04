//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// MageCraft output.
//
//-----------------------------------------------------------------------------

#include "Info.hpp"

#include "GDCC/IR/Function.hpp"
#include "GDCC/IR/Object.hpp"

#include "GDCC/IR/Exp/Binary.hpp"
#include "GDCC/IR/Exp/ValueGlyph.hpp"
#include "GDCC/IR/Exp/ValueMulti.hpp"
#include "GDCC/IR/Exp/ValueRoot.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace Bytecode
{
   namespace MgC
   {
      //
      // Info::put
      //
      void Info::put(std::ostream &out_)
      {
         out = &out_;

         *out << std::hex << "MgC_NTS" << '\0' << "CODEDEFS" << '\0' << '\0';

         for(auto const &itr : GDCC::IR::Space::LocArs.obset)
         {
            if(itr->defin)
               putObj(*itr);
         }

         for(auto &itr : GDCC::IR::FunctionRange())
         {
            if(itr.second.defin)
               putFunc(itr.second);
         }

         out = nullptr;
      }

      //
      // Info::putBlock
      //
      void Info::putBlock(GDCC::IR::Block &block)
      {
         try
         {
            auto end   = static_cast<GDCC::IR::Statement *>(block.end());
                 stmnt = static_cast<GDCC::IR::Statement *>(block.begin());
            for(; stmnt != end; stmnt = stmnt->next)
               putStmnt();
            stmnt = nullptr;
         }
         catch(...)
         {
            stmnt = nullptr;
            throw;
         }
      }

      //
      // Info::putExp
      //
      void Info::putExp(GDCC::IR::Exp const *exp)
      {
         switch(auto s = static_cast<GDCC::StringIndex>(exp->getName()))
         {
         case GDCC::STR_BinaryAdd:
            *out << '+' << '\0';
            putExp(static_cast<GDCC::IR::Exp_Binary const *>(exp)->expL);
            putExp(static_cast<GDCC::IR::Exp_Binary const *>(exp)->expR);
            break;

         case GDCC::STR_BinaryDiv:
            *out << '/' << '\0';
            putExp(static_cast<GDCC::IR::Exp_Binary const *>(exp)->expL);
            putExp(static_cast<GDCC::IR::Exp_Binary const *>(exp)->expR);
            break;

         case GDCC::STR_BinaryMod:
            *out << '%' << '\0';
            putExp(static_cast<GDCC::IR::Exp_Binary const *>(exp)->expL);
            putExp(static_cast<GDCC::IR::Exp_Binary const *>(exp)->expR);
            break;

         case GDCC::STR_BinaryMul:
            *out << '*' << '\0';
            putExp(static_cast<GDCC::IR::Exp_Binary const *>(exp)->expL);
            putExp(static_cast<GDCC::IR::Exp_Binary const *>(exp)->expR);
            break;

         case GDCC::STR_BinarySub:
            *out << '-' << '\0';
            putExp(static_cast<GDCC::IR::Exp_Binary const *>(exp)->expL);
            putExp(static_cast<GDCC::IR::Exp_Binary const *>(exp)->expR);
            break;

         case GDCC::STR_ValueGlyph:
            putGlyph(static_cast<GDCC::IR::Exp_ValueGlyph const *>(exp)->glyph);
            break;

         case GDCC::STR_ValueMulti:
            // This is kind of unfortunate, since it can easily result in
            // incorrect codegen by adding unexpected commas. However, it is
            // needed by putObj and is really a higher level problem.
            // FIXME/TODO: This does need to handle non-words properly, though.
            {
               auto multi = static_cast<GDCC::IR::Exp_ValueMulti const *>(exp);
               auto itr = multi->expv.begin(), end = multi->expv.end();
               if(itr != end) for(putExp(*itr++); itr != end; ++itr)
               {
                  *out << ',' << '\0';
                  putExp(*itr);
               }
            }
            break;

         case GDCC::STR_ValueRoot:
            putValue(static_cast<GDCC::IR::Exp_ValueRoot const *>(exp)->value);
            break;

         default:
            std::cerr << "ERROR: " << exp->pos << ": cannot put Exp for MgC: "
               << static_cast<GDCC::String>(s) << "\n";
            throw EXIT_FAILURE;
         }
      }

      //
      // Info::putFunc
      //
      void Info::putFunc(GDCC::IR::Function &func_)
      {
         func = &func_;

         // Put the function header.
         *out << "function" << '\0' << func->glyph << '\0'
            << '{' << '\0'
               << "argCount" << '\0' << '=' << '\0' << func->param    << '\0' << ';' << '\0'
               << "label"    << '\0' << '=' << '\0' << func->label    << '\0' << ';' << '\0'
               << "regCount" << '\0' << '=' << '\0' << func->localReg << '\0' << ';' << '\0'
               << "retCount" << '\0' << '=' << '\0' << func->retrn    << '\0' << ';' << '\0'
            << '}' << '\0';

         // Put the function code.
         *out << "code" << '\0' << func->label << '\0'
            << '{' << '\0';

         putBlock(func->block);

         *out << '}' << '\0';

         func = nullptr;
      }

      //
      // Info::putGlyph
      //
      void Info::putGlyph(GDCC::IR::Glyph glyph)
      {
         auto str = static_cast<GDCC::String>(glyph);

         switch(str.data()[0])
         {
         case '@':
         case '$':
         case '%':
         case '&':
         case '*':
         case '{':
         case '"':
         case '\'':
            *out        << str << '\0'; break;
         default:
            *out << '$' << str << '\0'; break;
         }
      }

      //
      // Info::putObj
      //
      void Info::putObj(GDCC::IR::Object const &obj)
      {
         *out << "data" << '\0' << obj.glyph << '\0' << obj.words << '\0';

         if(obj.initi)
         {
            *out << '(' << '\0';

            if(obj.initi->canGetValue())
               putObjValue(obj.initi->getValue());
            else
               putExp(obj.initi);

            *out << ')' << '\0';
         }
         else
            *out << ';' << '\0';
      }

      //
      // Info::putObjValue
      //
      void Info::putObjValue(GDCC::IR::Value const &val)
      {
         switch(val.v)
         {
         case GDCC::IR::ValueBase::Fixed:
            *out << val.vFixed.value << '\0';
            break;

         case GDCC::IR::ValueBase::Multi:
            putObjValue_Multi(val.vMulti);
            break;

         default:
            std::cerr << "bad ObjValue\n";
            throw EXIT_FAILURE;
         }
      }

      //
      // Info::putObjValue_Multi
      //
      void Info::putObjValue_Multi(GDCC::IR::Value_Multi const &val)
      {
         std::size_t i = 0, e = val.value.size();
         GDCC::FastU bucket = 0, bucketBits = 0;
         GDCC::FastU bits;

         //
         // flushBucket
         //
         auto flushBucket = [&]()
         {
            if(!bucketBits) return;

            *out << bucket << '\0';

            if(i != e) *out << ',' << '\0';

            bucket     = 0;
            bucketBits = 0;
         };

         //
         // writeBucket
         //
         auto writeBucket = [&](GDCC::FastU value)
         {
            if(bucketBits + bits > 32)
               flushBucket();

            value <<= bucketBits;
            bucket |= value;
            bucketBits += bits;
         };

         for(; i != e; ++i) switch(val.value[i].v)
         {
         case GDCC::IR::ValueBase::Fixed:
            bits = val.value[i].vFixed.vtype.getBits();
            if(bits > 32) goto defcase;
            writeBucket(number_cast<GDCC::FastU>(val.value[i].vFixed.value));
            break;

         default:
         defcase:
            flushBucket();
            putObjValue(val.value[i]);
            break;
         }

         flushBucket();
      }

      //
      // Info::putStmnt
      //
      void Info::putStmnt()
      {
         // Put labels.
         for(auto lab : stmnt->labs)
            *out << "label" << '\0' << '(' << '\0' << lab << '\0' << ')' << '\0';

         // Put code and args.
         switch(stmnt->code)
         {
         case GDCC::IR::Code::Nop:
            *out << "nop" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::AddU_W:
            *out << "addu_stk" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::AndU_W:
            *out << "andu_stk" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::Call:
            *out << "call_stk" << '\0' << '(' << '\0';
               *out << (stmnt->args.size() - 2) << '\0';
            *out << ')' << '\0';
            break;

         case GDCC::IR::Code::Cjmp_Nil:
            *out << "cjmp_stk_nil" << '\0' << '(' << '\0';
               putExp(stmnt->args[1].aLit.value);
            *out << ')' << '\0';
            break;

         case GDCC::IR::Code::Cjmp_Tru:
            *out << "cjmp_stk_tru" << '\0' << '(' << '\0';
               putExp(stmnt->args[1].aLit.value);
            *out << ')' << '\0';
            break;

         case GDCC::IR::Code::CmpU_EQ_W:
            *out << "cmpu_stk_eq" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::CmpU_GE_W:
            *out << "cmpu_stk_ge" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::CmpU_GT_W:
            *out << "cmpu_stk_gt" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::CmpU_LE_W:
            *out << "cmpu_stk_le" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::CmpU_LT_W:
            *out << "cmpu_stk_lt" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::CmpU_NE_W:
            *out << "cmpu_stk_ne" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::Jump:
            putStmnt_Jump();
            break;

         case GDCC::IR::Code::Move_W:
            putStmnt_Move_W();
            break;

         case GDCC::IR::Code::MulU_W:
            *out << "mulu_stk" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::NotU_W:
            *out << "notu_stk" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::Retn:
            *out << "retn" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::ShRU_W:
            *out << "shru_stk" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::Code::SubU_W:
            *out << "subu_stk" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         default:
            std::cerr << "bad Code\n";
            throw EXIT_FAILURE;
         }
      }

      //
      // Info::putStmnt_Jump
      //
      void Info::putStmnt_Jump()
      {
         switch(stmnt->args[0].a)
         {
         case GDCC::IR::ArgBase::Lit:
            *out << "jump_imm" << '\0' << '(' << '\0';
               putExp(stmnt->args[0].aLit.value);
            *out << ')' << '\0';
            break;

         case GDCC::IR::ArgBase::Stk:
            *out << "jump_stk" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         default:
            std::cerr << "bad Code::Jump(?)\n";
            throw EXIT_FAILURE;
         }
      }

      //
      // Info::putStmnt_Move_W
      //
      void Info::putStmnt_Move_W()
      {
         // push_?
         if(stmnt->args[0].a == GDCC::IR::ArgBase::Stk) switch(stmnt->args[1].a)
         {
         case GDCC::IR::ArgBase::Lit:
            *out << "push_imm" << '\0' << '(' << '\0';
               putExp(stmnt->args[1].aLit.value);
            *out << ')' << '\0';
            break;

         case GDCC::IR::ArgBase::LocArs:
            switch(stmnt->args[1].aLocArs.idx->a)
            {
            case GDCC::IR::ArgBase::Lit:
               *out << "push_dat" << '\0' << '(' << '\0';
                  *out << '+' << '\0';
                     putExp(stmnt->args[1].aLocArs.off);
                     putExp(stmnt->args[1].aLocArs.idx->aLit.value);
               *out << ')' << '\0';
               break;

            case GDCC::IR::ArgBase::Stk:
               *out << "push_ptr" << '\0' << '(' << '\0';
                  putExp(stmnt->args[1].aLocArs.off);
               *out << ')' << '\0';
               break;

            default:
               std::cerr << "bad Code::Move_W(Stk, LocArs(?))\n";
               throw EXIT_FAILURE;
            }
            break;

         case GDCC::IR::ArgBase::LocReg:
            *out << "push_reg" << '\0' << '(' << '\0';
               putExp(stmnt->args[1].aLocReg.idx->aLit.value);
            *out << ')' << '\0';
            break;

         default:
            std::cerr << "bad Code::Move_W(Stk, ?)\n";
            throw EXIT_FAILURE;
         }

         // drop_?
         else if(stmnt->args[1].a == GDCC::IR::ArgBase::Stk) switch(stmnt->args[0].a)
         {
         case GDCC::IR::ArgBase::Nul:
            *out << "drop_nul" << '\0' << '(' << '\0' << ')' << '\0';
            break;

         case GDCC::IR::ArgBase::LocArs:
            switch(stmnt->args[0].aLocArs.idx->a)
            {
            case GDCC::IR::ArgBase::Lit:
               *out << "drop_dat" << '\0' << '(' << '\0';
                  *out << '+' << '\0';
                     putExp(stmnt->args[0].aLocArs.off);
                     putExp(stmnt->args[0].aLocArs.idx->aLit.value);
               *out << ')' << '\0';
               break;

            case GDCC::IR::ArgBase::Stk:
               *out << "drop_ptr" << '\0' << '(' << '\0';
                  putExp(stmnt->args[0].aLocArs.off);
               *out << ')' << '\0';
               break;

            default:
               std::cerr << "bad Code::Move_W(LocArs(?), Stk)\n";
               throw EXIT_FAILURE;
            }
            break;

         case GDCC::IR::ArgBase::LocReg:
            *out << "drop_reg" << '\0' << '(' << '\0';
               putExp(stmnt->args[0].aLocReg.idx->aLit.value);
            *out << ')' << '\0';
            break;

         default:
            std::cerr << "bad Code::Move_W(?, Stk)\n";
            throw EXIT_FAILURE;
         }

         // ???
         else
         {
            std::cerr << "bad Code::Move_W\n";
            throw EXIT_FAILURE;
         }
      }

      //
      // Info::putValue
      //
      void Info::putValue(GDCC::IR::Value const &val)
      {
         switch(val.v)
         {
         case GDCC::IR::ValueBase::Fixed:
            *out << val.vFixed.value << '\0';
            break;

         default:
            std::cerr << "bad Value\n";
            throw EXIT_FAILURE;
         }
      }
   }
}

// EOF

