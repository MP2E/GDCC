//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// C floating-point types.
//
//-----------------------------------------------------------------------------

#include "CC/Type/Float.hpp"

#include "Bytecode/Platform.hpp"

#include "IR/Type.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

// Table: Type_Float::getBaseType
static GDCC::AST::Type::CRef const FloatTable_BaseType[4] =
{
   GDCC::CC::GetTypeFloatRS(), // FIXME
   GDCC::CC::GetTypeFloatRS(),
   GDCC::CC::GetTypeFloatRSL(),
   GDCC::CC::GetTypeFloatRSLL(),
};

// Table: Type_Float::getSizeBitsF
static constexpr GDCC::Core::FastU FloatTable_SizeBitsF[4] = {5, 8, 11, 11};

// Table: Type_Float::getSizeBitsI
static constexpr GDCC::Core::FastU FloatTable_SizeBitsI[4] = {10, 23, 52, 52};

// Table: Type_Float::getSizeBytes MageCraft
static constexpr GDCC::Core::FastU FloatTable_SizeBytes_MageCraft[8] =
   {2, 4, 4, 8, 8, 16, 8, 16};

// Table: Type_Float::getSizeBytes ZDoom
static constexpr GDCC::Core::FastU FloatTable_SizeBytes_ZDoom[8] =
   {1, 1, 1, 2, 2, 4, 2, 4};

// Table: Type_Float::getSizePoint MageCraft
static constexpr GDCC::Core::FastU FloatTable_SizePoint_MageCraft[8] =
   {1, 1, 1, 2, 2, 4, 2, 4};

// Table: Type_Float::getSizeWords MageCraft
static constexpr GDCC::Core::FastU FloatTable_SizeWords_MageCraft[8] =
   {1, 1, 1, 2, 2, 4, 2, 4};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace GDCC
{
   namespace CC
   {
      GDCC_CC_Type_ImplGet(FloatCS,   Float, 1, true, false, true)
      GDCC_CC_Type_ImplGet(FloatCSL,  Float, 2, true, false, true)
      GDCC_CC_Type_ImplGet(FloatCSLL, Float, 3, true, false, true)

      GDCC_CC_Type_ImplGet(FloatIS,   Float, 1, false, true, true)
      GDCC_CC_Type_ImplGet(FloatISL,  Float, 2, false, true, true)
      GDCC_CC_Type_ImplGet(FloatISLL, Float, 3, false, true, true)

      GDCC_CC_Type_ImplGet(FloatRS,   Float, 1, false, false, true)
      GDCC_CC_Type_ImplGet(FloatRSL,  Float, 2, false, false, true)
      GDCC_CC_Type_ImplGet(FloatRSLL, Float, 3, false, false, true)

      //
      // Type_Float constructor
      //
      Type_Float::Type_Float(unsigned size_, bool cplx_, bool imag_, bool sign_) :
         size{size_},
         cplx{cplx_},
         imag{imag_},
         real{!cplx_ && !imag_},
         sign{sign_},
         unsi{!sign_}
      {
      }

      //
      // Type_Float::getBaseType
      //
      AST::Type::CRef Type_Float::getBaseType() const
      {
         if(real) throw AST::TypeError();
         return FloatTable_BaseType[size];
      }

      //
      // Type_Float::getIRType
      //
      IR::Type Type_Float::getIRType() const
      {
         return IR::Type_Float(getSizeBitsI(), getSizeBitsF(), getSizeBitsS(),
            isTypeSaturate());
      }

      //
      // Type_Float::getSizeAlign
      //
      Core::FastU Type_Float::getSizeAlign() const
      {
         switch(Bytecode::TargetCur)
         {
         case Bytecode::Target::MageCraft: return !size && !cplx ? 2 : 4;
         default:                          return 1;
         }
      }

      //
      // Type_Float::getSizeBitsF
      //
      Core::FastU Type_Float::getSizeBitsF() const
      {
         return FloatTable_SizeBitsF[size];
      }

      //
      // Type_Float::getSizeBitsI
      //
      Core::FastU Type_Float::getSizeBitsI() const
      {
         return FloatTable_SizeBitsI[size];
      }

      //
      // Type_Float::getSizeBytes
      //
      Core::FastU Type_Float::getSizeBytes() const
      {
         switch(Bytecode::TargetCur)
         {
         case Bytecode::Target::MageCraft:
            return FloatTable_SizeBytes_MageCraft[(size << 1) | cplx];

         default:
            return FloatTable_SizeBytes_ZDoom[(size << 1) | cplx];
         }
      }

      //
      // Type_Float::getSizePoint
      //
      Core::FastU Type_Float::getSizePoint() const
      {
         switch(Bytecode::TargetCur)
         {
         case Bytecode::Target::MageCraft:
            return FloatTable_SizePoint_MageCraft[(size << 1) | cplx];

         default:
            return FloatTable_SizeBytes_ZDoom[(size << 1) | cplx];
         }
      }

      //
      // Type_Float::getSizeShift
      //
      Core::FastU Type_Float::getSizeShift() const
      {
         switch(Bytecode::TargetCur)
         {
         case Bytecode::Target::MageCraft: return !size && !cplx ? 2 : 4;
         default:                          return 1;
         }
      }

      //
      // Type_Float::getSizeWords
      //
      Core::FastU Type_Float::getSizeWords() const
      {
         switch(Bytecode::TargetCur)
         {
         case Bytecode::Target::MageCraft:
            return FloatTable_SizeWords_MageCraft[(size << 1) | cplx];

         default:
            return FloatTable_SizeBytes_ZDoom[(size << 1) | cplx];
         }
      }
   }
}

// EOF

