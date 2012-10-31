using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SlowpokeVM
{
    class Register
    {
        public HashSet<int> onValueBreakPoints;

        private VirtualMachine vm;
        protected int data;

        public bool BreakOnRead { get; set; }
        public bool BreakOnWrite { get; set; }

        public Register(VirtualMachine vm)
        {
            onValueBreakPoints = new HashSet<int>();
            this.vm = vm;
        }

        private void SetData(int data)
        {
            this.data = data;
            if (vm.DebugMode && onValueBreakPoints.Contains(data))
            {
                vm.BreakPoint = new BreakOnValue(this, data);
            }
        }

        public void BreakOnValue(int value)
        {
            onValueBreakPoints.Add(value);
        }

        public void RemoveBreakOnValue(int value)
        {
            onValueBreakPoints.Remove(value);
        }

        public int Signed
        {
            get
            {
                return (int)((Int16)data);
            }
            set
            {
                SetData((UInt16)((Int16)value));
            }
        }

        public int Unsigned
        {
            get
            {
                return data;
            }
            set
            {
                SetData((UInt16)value);
            }
        }
    }
}
