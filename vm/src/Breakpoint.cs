using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SlowpokeVM
{
    class BreakPoint
    {
    }

    class BreakOnProgramEnd : BreakPoint
    {
    }

    class BreakOnValueChanged : BreakPoint
    {
        public Register Source { get; private set; }

        public BreakOnValueChanged(Register source)
        {
            Source = source;
        }
    }

    class BreakOnWrite : BreakOnValueChanged
    {
        public BreakOnWrite(Register source)
            : base(source)
        {
        }
    }

    class BreakOnRead : BreakOnValueChanged
    {
        public BreakOnRead(Register source)
            : base(source)
        {
        }
    }

    class BreakOnValue : BreakOnValueChanged
    {
        public int Value { get; private set; }

        public BreakOnValue(Register source, int value) : base(source)
        {
            Value = value;
        }
    }

    class BreakOnPause : BreakPoint
    {
    }
}
