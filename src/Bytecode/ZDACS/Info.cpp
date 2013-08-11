//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// ZDoom ACS target information and handling class.
//
//-----------------------------------------------------------------------------

#include "Info.hpp"

#include "GDCC/IR/Arg.hpp"
#include "GDCC/IR/Glyph.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

namespace Bytecode
{
   namespace ZDACS
   {
      GDCC::IR::Type_Fixed const Info::TypeWord{32, 0, false, false};
   }
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace Bytecode
{
   namespace ZDACS
   {
      //
      // Info default constructor
      //
      Info::Info() :
         jumpPos{16}, // HACK: Should be calculated.
         numChunkAIMP{0},
         numChunkARAY{0},
         numChunkFNAM{0},
         numChunkFUNC{0},
         numChunkLOAD{0},
         numChunkMEXP{0},
         numChunkMIMP{0},
         numChunkSFLG{0},
         numChunkSNAM{0},
         numChunkSPTR{0},
         numChunkSTRL{0},
         numChunkSVCT{0}
      {
      }

      //
      // Info::lenString
      //
      std::size_t Info::lenString(GDCC::String str)
      {
         auto const &data = str.getData();
         std::size_t len = 0;

         for(auto i = data.str, e = i + data.len; i != e; ++i) switch(*i)
         {
         case '\0':
            if('0' <= i[1] && i[1] <= '7')
               len += 4;
            else
               len += 2;
            break;

         case '\\':
            len += 2;
            break;

         default:
            len += 1;
            break;
         }

         return len + 1;
      };

      //
      // Info::BackGlyphFunc
      //
      void Info::BackGlyphFunc(GDCC::String glyph, GDCC::FastU val, GDCC::IR::CallType ctype)
      {
         auto &data = GDCC::IR::Glyph::GetData(glyph);

         data.type  = GDCC::IR::Type_Funct(ctype);
         data.value = GDCC::IR::ExpCreate_ValueRoot(
            GDCC::IR::Value_Funct(val, GDCC::IR::Type_Funct(ctype)),
            GDCC::Origin(GDCC::STRNULL, 0));
      }

      //
      // Info::BackGlyphGlyph
      //
      void Info::BackGlyphGlyph(GDCC::String glyph, GDCC::String val)
      {
         auto &data = GDCC::IR::Glyph::GetData(glyph);

         data.type  = GDCC::IR::Glyph::GetData(val).type;
         data.value = GDCC::IR::ExpCreate_ValueGlyph(
            static_cast<GDCC::IR::Glyph>(val),
            GDCC::Origin(GDCC::STRNULL, 0));
      }

      //
      // Info::BackGlyphStr
      //
      void Info::BackGlyphStr(GDCC::String glyph, GDCC::FastU val)
      {
         auto &data = GDCC::IR::Glyph::GetData(glyph);

         data.type  = GDCC::IR::Type_StrEn();
         data.value = GDCC::IR::ExpCreate_ValueRoot(
            GDCC::IR::Value_StrEn(val, GDCC::IR::Type_StrEn()),
            GDCC::Origin(GDCC::STRNULL, 0));
      }

      //
      // Info::BackGlyphWord
      //
      void Info::BackGlyphWord(GDCC::String glyph, GDCC::FastU val)
      {
         auto &data = GDCC::IR::Glyph::GetData(glyph);

         data.type  = TypeWord;
         data.value = GDCC::IR::ExpCreate_ValueRoot(
            GDCC::IR::Value_Fixed(val, TypeWord), GDCC::Origin(GDCC::STRNULL, 0));
      }

      //
      // Info::CheckArg
      //
      void Info::CheckArg(GDCC::IR::Arg const &arg, GDCC::Origin pos)
      {
         switch(arg.a)
         {
         case GDCC::IR::ArgBase::GblArr:
            CheckArgB(*arg.aGblArr.arr, GDCC::IR::ArgBase::Lit, pos);
            CheckArg (*arg.aGblArr.idx, pos);
            break;

         case GDCC::IR::ArgBase::GblReg:
            CheckArgB(*arg.aGblReg.idx, GDCC::IR::ArgBase::Lit, pos);
            break;

         case GDCC::IR::ArgBase::Lit: break;
         case GDCC::IR::ArgBase::Nul: break;
         case GDCC::IR::ArgBase::Stk: break;

         case GDCC::IR::ArgBase::LocReg:
            CheckArgB(*arg.aLocReg.idx, GDCC::IR::ArgBase::Lit, pos);
            break;

         case GDCC::IR::ArgBase::MapArr:
            CheckArgB(*arg.aMapArr.arr, GDCC::IR::ArgBase::Lit, pos);
            CheckArg (*arg.aMapArr.idx, pos);
            break;

         case GDCC::IR::ArgBase::MapReg:
            CheckArgB(*arg.aMapReg.idx, GDCC::IR::ArgBase::Lit, pos);
            break;

         case GDCC::IR::ArgBase::WldArr:
            CheckArgB(*arg.aWldArr.arr, GDCC::IR::ArgBase::Lit, pos);
            CheckArg (*arg.aWldArr.idx, pos);
            break;

         case GDCC::IR::ArgBase::WldReg:
            CheckArgB(*arg.aWldReg.idx, GDCC::IR::ArgBase::Lit, pos);
            break;

         default:
            std::cerr << "ERROR: " << pos << ": ZDACS cannot use Arg: " << arg.a << '\n';
            throw EXIT_FAILURE;
         }
      }

      //
      // Info::ResolveGlyph
      //
      GDCC::IR::Exp::Ref Info::ResolveGlyph(GDCC::String glyph)
      {
         if(auto exp = GDCC::IR::Glyph::GetData(glyph).value)
            return static_cast<GDCC::IR::Exp::Ref>(exp);

         std::cerr << "ERROR: unbacked glyph: '" << glyph << "'\n";
         throw EXIT_FAILURE;
      }

      //
      // Info::ResolveValue
      //
      GDCC::FastU Info::ResolveValue(GDCC::IR::Value const &val)
      {
         switch(val.v)
         {
         case GDCC::IR::ValueBase::Fixed:
            if(val.vFixed.vtype.bitsS)
               return number_cast<GDCC::FastI>(val.vFixed.value);
            else
               return number_cast<GDCC::FastU>(val.vFixed.value);

         case GDCC::IR::ValueBase::Funct:
             return val.vFunct.value;

         case GDCC::IR::ValueBase::StrEn:
             return val.vFunct.value;

         default:
            std::cerr << "ERROR: bad Value\n";
            throw EXIT_FAILURE;
         }
      }
   }
}

// EOF

