using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ESC64VM
{
	public delegate void RegisterChanged(Register register, int oldValue, int newValue);
	
    public class Register
    {
		public event RegisterChanged Change;
		
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
		
		private void RegisterChanged(Register register, int oldValue, int newValue)
		{
			try
			{
				Change(this, oldValue, newValue);
			}
			catch(Exception)
			{
			}
		}
		
        private int Data
        {
	        get
			{
				if(vm.DebugMode && BreakOnRead)
				{
					vm.CurrentBreakPoint = new BreakOnRead(this);
				}
				return data;
			}
        	set
        	{
				if(vm.DebugMode)
				{
					if(BreakOnWrite)
					{
						vm.CurrentBreakPoint = new BreakOnWrite(this);
					}
					else if (vm.DebugMode && onValueBreakPoints.Contains(value))
			        {
			            vm.CurrentBreakPoint = new BreakOnValue(this, value);
			        }
				}
			
				RegisterChanged(this, data, value);
				data = value;
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
                return (int)((Int16)Data);
            }
            set
            {
                Data = (UInt16)((Int16)value);
            }
        }

        public int Unsigned
        {
            get
            {
                return Data;
            }
            set
            {
                Data = (UInt16)value;
            }
        }
    }
}
