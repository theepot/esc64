using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ESC64VM
{
    public class BreakPoint
    {
    }

    public class BreakOnProgramEnd : BreakPoint
    {
    }

    public class BreakOnValueChanged : BreakPoint
    {
        public Register Source { get; private set; }

        public BreakOnValueChanged(Register source)
        {
            Source = source;
        }
    }

    public class BreakOnWrite : BreakOnValueChanged
    {
        public BreakOnWrite(Register source)
            : base(source)
        {
        }
    }

    public class BreakOnRead : BreakOnValueChanged
    {
        public BreakOnRead(Register source)
            : base(source)
        {
        }
    }

    public class BreakOnValue : BreakOnValueChanged
    {
        public int Value { get; private set; }

        public BreakOnValue(Register source, int value) : base(source)
        {
            Value = value;
        }
    }

    public class BreakOnPause : BreakPoint
    {
    }
}
