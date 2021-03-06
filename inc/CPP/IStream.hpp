//-----------------------------------------------------------------------------
//
// Copyright (C) 2013 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// C input stream.
//
//-----------------------------------------------------------------------------

#ifndef GDCC__CPP__IStream_H__
#define GDCC__CPP__IStream_H__

#include "../CPP/TrigraphBuf.hpp"

#include "../Core/CommentBuf.hpp"
#include "../Core/EscapeBuf.hpp"
#include "../Core/FeatureHold.hpp"
#include "../Core/OriginBuf.hpp"

#include <istream>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace GDCC
{
   namespace Core
   {
      struct Token;
   }

   namespace CPP
   {
      //
      // IStream
      //
      class IStream : public std::istream
      {
      public:
         IStream(std::streambuf &buf, Core::String file, std::size_t line = 1) :
            std::istream{&cbuf}, obuf{buf, file, line}, tbuf{obuf}, ebuf{tbuf},
            cbuf{ebuf} {}

         void disableComments() {rdbuf(&obuf);}

         void enableComments() {rdbuf(&cbuf);}

         Core::Origin getOrigin() const {return obuf.getOrigin();}

         using CommentsHold = Core::FeatureHold<IStream,
            &IStream::disableComments, &IStream::enableComments>;

         CommentsHold holdComments() {return CommentsHold(*this);}

         bool needHeader : 1;


         static bool GetHeader(std::istream &in, Core::Token &out);

      protected:
         using OBuf = Core::OriginBuf<>;
         using TBuf = TrigraphBuf<OBuf>;
         using EBuf = Core::EscapeBufStrip<'\n', '\\', TBuf>;
         using CBuf = Core::CommentBufC<EBuf>;

         OBuf obuf;
         TBuf tbuf;
         EBuf ebuf;
         CBuf cbuf;
      };
   }
}

//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace GDCC
{
   namespace CPP
   {
      IStream &operator >> (IStream &in, Core::Token &out);
   }
}

#endif//GDCC__CPP__IStream_H__

