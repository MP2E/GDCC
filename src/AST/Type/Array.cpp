//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Abstract Syntax Tree expression array type handling.
//
//-----------------------------------------------------------------------------

#include "AST/Type/Array.hpp"

#include "AST/Exp.hpp"

#include "IR/Type.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace GDCC
{
   namespace AST
   {
      //
      // Type::getTypeArray
      //
      Type::CRef Type::getTypeArray() const
      {
         if(!arrType0) new Type_Array0(this);
         return static_cast<CRef>(arrType0);
      }

      //
      // Type::getTypeArray
      //
      Type::CRef Type::getTypeArray(Core::FastU size) const
      {
         // Search for existing array type.
         if(auto type = arrType) do
         {
            if(type->size == size)
               return static_cast<CRef>(type);

            type = type->arrNext;
         }
         while(type != arrType);

         // Otherwise, create one and return it.
         return static_cast<CRef>(new Type_Array(this, size));
      }

      //
      // Type::getTypeArray
      //
      Type::CRef Type::getTypeArray(Exp const *size) const
      {
         if(!size)
         {
            if(!avmType0) new Type_ArrVM0(this);
            return static_cast<CRef>(avmType0);
         }

         // A check for size being a constant expression could go here. However,
         // that is really a higher level question that cannot be answered here.

         // Search for existing array type.
         // This is unlikely to succeed. Do it anyway.
         if(auto type = avmType) do
         {
            if(type->size == size)
               return static_cast<CRef>(type);

            type = type->avmNext;
         }
         while(type != avmType);

         // Otherwise, create one and return it.
         return static_cast<CRef>(new Type_ArrVM(this, size));
      }

      //
      // Type_ArrVM copy constructor
      //
      Type_ArrVM::Type_ArrVM(Type_ArrVM const &type) : Super{type},
         size{type.size}, avmNext{this}, avmPrev{this}
      {
      }

      //
      // Type_ArrVM constructor
      //
      Type_ArrVM::Type_ArrVM(Type const *base_, Exp const *size_) :
         Super{base_}, size{size_}
      {
         GDCC_AST_Type_Insert(avm);
      }

      //
      // Type_ArrVM destructor
      //
      Type_ArrVM::~Type_ArrVM()
      {
         GDCC_AST_Type_Unlink(avm);
      }

      //
      // Type_ArrVM::getSizeBytesVM
      //
      Exp::CRef Type_ArrVM::getSizeBytesVM() const
      {
         return ExpCreate_BinaryMulSize(size, base->getSizeBytesVM());
      }

      //
      // Type_ArrVM::getSizePointVM
      //
      Exp::CRef Type_ArrVM::getSizePointVM() const
      {
         return ExpCreate_BinaryMulSize(size, base->getSizePointVM());
      }

      //
      // Type_ArrVM::getSizeWordsVM
      //
      Exp::CRef Type_ArrVM::getSizeWordsVM() const
      {
         return ExpCreate_BinaryMulSize(size, base->getSizeWordsVM());
      }

      //
      // Type_ArrVM0 constructor
      //
      Type_ArrVM0::Type_ArrVM0(Type const *base_) : Super{base_}
      {
         base->avmType0 = this;
      }

      //
      // Type_ArrVM0 destructor
      //
      Type_ArrVM0::~Type_ArrVM0()
      {
         // Only nullify base's reference if this is the unqualified pointer.
         if(base->avmType0 == this)
            base->avmType0 = nullptr;
      }

      //
      // Type_Array copy constructor
      //
      Type_Array::Type_Array(Type_Array const &type) : Super{type},
         size{type.size}, arrNext{this}, arrPrev{this}
      {
      }

      //
      // Type_Array constructor
      //
      Type_Array::Type_Array(Type const *base_, Core::FastU size_) :
         Super{base_}, size{size_}
      {
         GDCC_AST_Type_Insert(arr);
      }

      //
      // Type_Array destructor
      //
      Type_Array::~Type_Array()
      {
         GDCC_AST_Type_Unlink(arr);
      }

      //
      // Type_Array::getIRType
      //
      IR::Type Type_Array::getIRType() const
      {
         return IR::Type_Multi(Core::Array<IR::Type>(size, base->getIRType()));
      }

      //
      // Type_Array::getSizeBytes
      //
      Core::FastU Type_Array::getSizeBytes() const
      {
         return base->getSizeBytes() * size;
      }

      //
      // Type_Array::getSizeBytesVM
      //
      Exp::CRef Type_Array::getSizeBytesVM() const
      {
         return ExpCreate_BinaryMulSize(base->getSizeBytesVM(), ExpCreate_ValueSize(size));
      }

      //
      // Type_Array::getSizePoint
      //
      Core::FastU Type_Array::getSizePoint() const
      {
         return base->getSizePoint() * size;
      }

      //
      // Type_Array::getSizePointVM
      //
      Exp::CRef Type_Array::getSizePointVM() const
      {
         return ExpCreate_BinaryMulSize(base->getSizePointVM(), ExpCreate_ValueSize(size));
      }

      //
      // Type_Array::getSizeWords
      //
      Core::FastU Type_Array::getSizeWords() const
      {
         return base->getSizeWords() * size;
      }

      //
      // Type_Array::getSizeWordsVM
      //
      Exp::CRef Type_Array::getSizeWordsVM() const
      {
         return ExpCreate_BinaryMulSize(base->getSizeWordsVM(), ExpCreate_ValueSize(size));
      }

      //
      // Type_Array::isTypeSizeVM
      //
      bool Type_Array::isTypeSizeVM() const
      {
         return base->isTypeSizeVM();
      }

      //
      // Type_Array::isTypeVM
      //
      bool Type_Array::isTypeVM() const
      {
         return base->isTypeSizeVM();
      }

      //
      // Type_Array0 constructor
      //
      Type_Array0::Type_Array0(Type const *base_) : Super{base_}
      {
         base->arrType0 = this;
      }

      //
      // Type_Array0 destructor
      //
      Type_Array0::~Type_Array0()
      {
         // Only nullify base's reference if this is the unqualified pointer.
         if(base->arrType0 == this)
            base->arrType0 = nullptr;
      }

      //
      // Type_Array0::isTypeSizeVM
      //
      bool Type_Array0::isTypeSizeVM() const
      {
         return base->isTypeSizeVM();
      }

      //
      // Type_Array0::isTypeVM
      //
      bool Type_Array0::isTypeVM() const
      {
         return base->isTypeSizeVM();
      }
   }
}

// EOF

