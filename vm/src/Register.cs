using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ESC64VM
{
	public delegate void RegisterChanged(int oldValue);
	
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
		
		private void RegisterChanged(int oldValue)
		{
			try
			{
				Change(oldValue);
			}
			catch(Exception)
			{
			}
		}
		
        private void SetData(int data)
        {
			if(vm.DebugMode)
			{
				if(BreakOnWrite)
				{
					vm.CurrentBreakPoint = new BreakOnWrite(this);
					RegisterChanged(this.data);
				}
				else if (vm.DebugMode && onValueBreakPoints.Contains(data))
	            {
	                vm.CurrentBreakPoint = new BreakOnValue(this, data);
					RegisterChanged(this.data);
	            }
			}
			
			this.data = data;
        }
		
		private void GetData(int data)
		{
			//TODO implement
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
