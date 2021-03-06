//-----------------------------------------------------------------------------
//
// Copyright (C) 2013-2014 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// IR::Function dump/disassembly functions.
//
//-----------------------------------------------------------------------------

#include "IRDump/IRDump.hpp"

#include "Core/Option.hpp"

#include "IR/CallType.hpp"
#include "IR/Function.hpp"
#include "IR/Linkage.hpp"
#include "IR/ScriptType.hpp"

#include "Option/Bool.hpp"


//----------------------------------------------------------------------------|
// Options                                                                    |
//

//
// --dump-block
//
static GDCC::Option::Bool DumpBlock
{
   &GDCC::Core::GetOptionList(), GDCC::Option::Base::Info()
      .setName("dump-block")
      .setGroup("output")
      .setDescS("Dump IR::Block objects."),

   &GDCC::IRDump::IRDumpOpt_Block
};

//
// --dump-labels
//
static GDCC::Option::Bool DumpLabels
{
   &GDCC::Core::GetOptionList(), GDCC::Option::Base::Info()
      .setName("dump-labels")
      .setGroup("output")
      .setDescS("Dump labels of statements."),

   &GDCC::IRDump::IRDumpOpt_Labels
};

//
// --dump-origin
//
static GDCC::Option::Bool DumpOrigin
{
   &GDCC::Core::GetOptionList(), GDCC::Option::Base::Info()
      .setName("dump-origin")
      .setGroup("output")
      .setDescS("Dump origin of statements."),

   &GDCC::IRDump::IRDumpOpt_Origin
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace GDCC
{
   namespace IRDump
   {
      //
      // IRDump_Function
      //
      void IRDump_Function(std::ostream &out, IR::Function const &fn)
      {
         out << "\nFunction "; IRDumpString(out, fn.glyph);

         if(fn.alloc)    out << " \\\n   alloc="    << fn.alloc;
                         out << " \\\n   ctype="    << fn.ctype;
         if(fn.defin)    out << " \\\n   defin="    << fn.defin;
         if(fn.label)   {out << " \\\n   label=";      IRDumpString(out, fn.label);}
                         out << " \\\n   linka="    << fn.linka;
         if(fn.localArs) out << " \\\n   localArs=" << fn.localArs;
         if(fn.localReg) out << " \\\n   localReg=" << fn.localReg;
         if(fn.param)    out << " \\\n   param="    << fn.param;
         if(fn.retrn)    out << " \\\n   retrn="    << fn.retrn;
         if(fn.sflagNet) out << " \\\n   sflagNet=" << fn.sflagNet;
         if(fn.sflagClS) out << " \\\n   sflagClS=" << fn.sflagClS;
                         out << " \\\n   stype="    << fn.stype;
         if(fn.valueInt) out << " \\\n   valueInt=" << fn.valueInt;
         if(fn.valueStr){out << " \\\n   valueStr=";   IRDumpString(out, fn.valueStr);}

         if(IRDumpOpt_Block && !fn.block.empty())
         {
            out << " \\\n   block\n{\n";

            Core::Origin pos{Core::STRNULL, 0};

            for(auto const &stmnt : fn.block)
            {
               // Dump origin, if different from previous.
               if(IRDumpOpt_Origin && stmnt.pos != pos)
               {
                  if(pos.file) out << '\n';
                  out << "   ; " << (pos = stmnt.pos) << '\n';
               }

               // Dump labels.
               if(IRDumpOpt_Labels) for(auto const &lab : stmnt.labs)
               {
                  IRDumpString(out, lab);
                  out << '\n';
               }

               // Dump instruction and arguments.
               out << "   " << stmnt.code;

               for(auto const &arg : stmnt.args)
               {
                  out << ", ";
                  IRDump_Arg(out, arg);
               }

               out << '\n';
            }

            out << "}";
         }

         out << '\n';
      }
   }
}

// EOF

