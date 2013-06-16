//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Comment-stripping streambufs.
//
//-----------------------------------------------------------------------------

#ifndef GDCC__CommentBuf_H__
#define GDCC__CommentBuf_H__

#include "WrapperBuf.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace GDCC
{
   //
   // CommentBufLine
   //
   // Handles line-oriented comments started by a single character.
   //
   template<char Start, typename Buf = std::streambuf>
   class CommentBufLine final : public WrapperBuf<Buf>
   {
   public:
      explicit CommentBufLine(Buf &buf_) : WrapperBuf<Buf>{buf_} {}

   protected:
      using WrapperBuf<Buf>::buf;
      using WrapperBuf<Buf>::pbc;
      using WrapperBuf<Buf>::pbb;

      //
      // uflow
      //
      virtual int uflow()
      {
         if(pbc != EOF) return pbb = pbc, pbc = EOF, pbb;
         if((pbb = buf.sbumpc()) != Start) return pbb;

         do {pbb = buf.sbumpc();} while(pbb != EOF && pbb != '\n');
         return pbb;
      }

      //
      // underflow
      //
      virtual int underflow()
      {
         if(pbc != (pbb = EOF)) return pbc;
         if((pbc = buf.sbumpc()) != Start) return pbc;

         do {pbc = buf.sbumpc();} while(pbc != EOF && pbc != '\n');
         return pbc;
      }
   };
}

#endif//GDCC__CommentBuf_H__

