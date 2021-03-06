//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// C _Bool type.
//
//-----------------------------------------------------------------------------

#ifndef GDCC__CC__Type__Bool_H__
#define GDCC__CC__Type__Bool_H__

#include "../../CC/Type.hpp"

#include "../../AST/Type.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace GDCC
{
   namespace CC
   {
      //
      // Type_Bool
      //
      class Type_Bool final : public AST::Type
      {
         GDCC_Core_CounterPreamble(GDCC::CC::Type_Bool, GDCC::AST::Type);

      public:
         // Type information.
         virtual IR::Type    getIRType()    const;
         virtual Core::FastU getSizeAlign() const {return 1;}
         virtual Core::FastU getSizeBitsF() const {return 0;}
         virtual Core::FastU getSizeBitsI() const {return 1;}
         virtual Core::FastU getSizeBitsO() const {return 0;}
         virtual bool        getSizeBitsS() const {return 0;}
         virtual Core::FastU getSizeBytes() const {return 1;}
         virtual Core::FastU getSizePoint() const {return 1;}
         virtual Core::FastU getSizeShift() const {return 1;}
         virtual Core::FastU getSizeWords() const {return 1;}

         // Type classification: General classifications.
         virtual bool isTypeBoolean()  const {return true;}
         virtual bool isTypeComplete() const {return true;}

         // Type classification: C/C++ classifications.
         virtual bool isCTypeArith()  const {return true;}
         virtual bool isCTypeInteg()  const {return true;}
         virtual bool isCTypeIntegU() const {return true;}
         virtual bool isCTypeObject() const {return true;}
         virtual bool isCTypeReal()   const {return true;}
         virtual bool isCTypeScalar() const {return true;}


         friend AST::Type::CRef GetTypeBool();

      protected:
         Type_Bool() = default;
      };
   }
}

#endif//GDCC__CC__Type__Bool_H__

