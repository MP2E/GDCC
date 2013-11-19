//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Source token streams.
//
//-----------------------------------------------------------------------------

#ifndef GDCC__TokenStream_H__
#define GDCC__TokenStream_H__

#include "TokenBuf.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace GDCC
{
   //
   // TokenStream
   //
   class TokenStream
   {
   public:
      TokenStream() : buf{nullptr} {}
      explicit TokenStream(TokenBuf *buf_) : buf{buf_}, eof{false}, err{false} {}

      virtual ~TokenStream() {}

      explicit operator bool () const {return buf && !eof && !err;}

      TokenStream &operator >> (Token &out) {out = get(); return *this;}

      //
      // drop
      //
      bool drop(TokenType tt)
      {
         auto const &tok = buf->peek();
         if(tok.tok == TOK_EOF)
            return eof = true, false;

         if(tok.tok == tt)
            return buf->get(), true;

         return false;
      }

      //
      // drop
      //
      bool drop(TokenType tt, String ts)
      {
         auto const &tok = buf->peek();
         if(tok.tok == TOK_EOF)
            return eof = true, false;

         if(tok.tok == tt && tok.str == ts)
            return buf->get(), true;

         return false;
      }

      //
      // get
      //
      Token const &get()
      {
         auto const &tok = buf->get();
         if(tok.tok == TOK_EOF)
            eof = true;

         return tok;
      }

      //
      // peek
      //
      Token const &peek()
      {
         auto const &tok = buf->peek();
         if(tok.tok == TOK_EOF)
            eof = true;

         return tok;
      }

      TokenBuf *tkbuf() const {return buf;}

      //
      // tkbuf
      //
      TokenBuf *tkbuf(TokenBuf *buf_)
      {
         auto oldbuf = buf;
         buf = buf_;
         eof = false;
         err = false;
         return oldbuf;
      }

      //
      // unget
      //
      void unget()
      {
         if(eof || err) return;

         if(!buf->unget())
            err = true;
      }

   private:
      TokenBuf *buf;

      bool eof : 1;
      bool err : 1;
   };
}

#endif//GDCC__TokenStream_H__

