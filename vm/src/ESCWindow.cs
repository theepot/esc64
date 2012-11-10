using System;
using System.Threading;

namespace ESC64VM
{
	public partial class ESCWindow : Gtk.Window
	{
		[Gtk.TreeNode (ListOnly = true)]
		public class RegisterNode : Gtk.TreeNode
		{
			private Register reg;
			
			[Gtk.TreeNodeValue (Column=0)]
			public string Name { get; private set; }
			
			[Gtk.TreeNodeValue (Column=1)]
			public string Value
			{
				get
				{
					return reg.Unsigned.ToString();
				}
			}
			
			public RegisterNode (string name, Register reg)
			{
				Name = name;
				this.reg = reg;
			}
		}
		
		[Gtk.TreeNode (ListOnly = true)]
		public class ProgramTableNode : Gtk.TreeNode
		{
			private MemoryController mem;
			public int address;
			
			public bool Wide
			{
				get
				{
					Instruction instr = Instruction.Read(mem, address);
					InstructionDescr descr = InstructionDescr.FindDescrByOpcode(instr.Opcode);
					if(descr == null)
					{
						return false;
					}
					return descr.Wide;
				}
			}
			
			[Gtk.TreeNodeValue (Column=0)]
			public string Address
			{
				get
				{
					return address.ToString();
				}
			}
			
			[Gtk.TreeNodeValue (Column=1)]
			public string Value
			{
				get
				{
					Instruction instr = Instruction.Read(mem, address);
					InstructionDescr descr = InstructionDescr.FindDescrByOpcode(instr.Opcode);
					if(descr == null)
					{
						return String.Format("UNDEFINED ({0})", mem[address]);
					}
					string name = descr.Name;
					string op0 = " " + (descr.HasOperand0 ? instr.Operand0.ToString() : "-");
					string op1 = ", " + (descr.HasOperand1 ? instr.Operand1.ToString() : "-");
					string op2 = ", " + (descr.HasOperand2 ? instr.Operand2.ToString() : "-");
					string op3 = descr.Wide ? mem[address + 1].ToString() : "";
					return name + op0 + op1 + op2 + op3;
				}
			}
			
			public ProgramTableNode(MemoryController mem, int address)
			{
				this.mem = mem;
				this.address = address;
			}
		}
		
		public class MemoryTableNode : Gtk.TreeNode
		{
			private MemoryController mem;
			private int address;
			
			[Gtk.TreeNodeValue (Column=0)]
			public string Address
			{
				get
				{
					return address.ToString();
				}
			}
			
			[Gtk.TreeNodeValue (Column=1)]
			public string Value
			{
				get
				{
					return mem[address].ToString();
				}
			}
			
			public MemoryTableNode(MemoryController mem, int address)
			{
				this.mem = mem;
				this.address = address;
			}
		}
		
		private VirtualMachine vm;
		
		private Gtk.NodeStore regTableNodes;
		private Gtk.NodeStore programTableNodes;
		private Gtk.NodeStore memoryTableNodes;
		
		int prgmAddrFrom = 0;
		int prgmAddrTo = 0;
		
		public ESCWindow () : 
				base(Gtk.WindowType.Toplevel)
		{
			this.Build ();
			Init ();
		}
		
		private void Init()
		{
			InitVirtualMachine();
			InitRegisterTable();
			InitProgramTable();
			InitMemoryTable();
		}
		
		private void InitVirtualMachine()
		{
			vm = new VirtualMachine();
			vm.DebugMode = true;
			vm.OnBreakPointHit += OnBreakPointHit;
		}
		
		private void OnBreakPointHit(VirtualMachine vm, BreakPoint bp)
		{
			if(bp is BreakOnProgramEnd)
			{
				InfoPopup("Program ended");
			}
			else
			{
				InfoPopup("Break point of type " + bp.GetType() + " hit");
			}
		}
		
		private void InitRegisterTable()
		{
			regTableNodes = new Gtk.NodeStore(typeof(RegisterNode));
			registerTable.NodeStore = regTableNodes;
			registerTable.AppendColumn("Name", new Gtk.CellRendererText(), "text", 0);
			registerTable.AppendColumn("Value", new Gtk.CellRendererText(), "text", 1);
			
			regTableNodes.AddNode(new RegisterNode("r0", vm.Registers.GPRegister0));
			regTableNodes.AddNode(new RegisterNode("r1", vm.Registers.GPRegister1));
			regTableNodes.AddNode(new RegisterNode("r2", vm.Registers.GPRegister2));
			regTableNodes.AddNode(new RegisterNode("r3", vm.Registers.GPRegister3));
			regTableNodes.AddNode(new RegisterNode("r4", vm.Registers.GPRegister4));
			regTableNodes.AddNode(new RegisterNode("r5", vm.Registers.GPRegister5));
			regTableNodes.AddNode(new RegisterNode("r6/lr", vm.Registers.LinkRegister));
			regTableNodes.AddNode(new RegisterNode("r7/pc", vm.Registers.ProgramCounter));
		}
		
		private void InitProgramTable()
		{
			programTableNodes = new Gtk.NodeStore(typeof(ProgramTableNode));
			programTable.NodeStore = programTableNodes;
			programTable.AppendColumn("Address", new Gtk.CellRendererText(), "text", 0);
			programTable.AppendColumn("Instruction", new Gtk.CellRendererText(), "text", 1);
			
			PopulateProgramTable();
		}
		
		private void InitMemoryTable()
		{
			memoryTableNodes = new Gtk.NodeStore(typeof(MemoryTableNode));
			memoryTable.NodeStore = memoryTableNodes;
			memoryTable.AppendColumn("Address", new Gtk.CellRendererText(), "text", 0);
			memoryTable.AppendColumn("Value", new Gtk.CellRendererText(), "text", 1);
			
			PopulateMemoryTable();
		}
		
		private void ErrorPopup(string message)
		{
			Gtk.MessageDialog dialog = new Gtk.MessageDialog(
				this,
				Gtk.DialogFlags.Modal,
				Gtk.MessageType.Error,
				Gtk.ButtonsType.Ok,
				message);
			dialog.Show();
		}
		
		private void InfoPopup(string message)
		{
			Gtk.MessageDialog dialog = new Gtk.MessageDialog(
				this,
				Gtk.DialogFlags.Modal,
				Gtk.MessageType.Info,
				Gtk.ButtonsType.Ok,
				message);
			dialog.Show();
		}
		
		private bool IsValidMemoryAddress(int addr)
		{
			return addr >= 0 && addr < MemoryController.MEMORY_SIZE;
		}
		
		private bool GetMemoryRange(Gtk.Entry entryFrom, Gtk.Entry entryTo, ref int addrFrom, ref int addrTo)
		{
			try
			{
				addrFrom = int.Parse(entryFrom.Text);
				addrTo = int.Parse(entryTo.Text);
			}
			catch(Exception) //TODO catch only number format
			{
				ErrorPopup("One or more memory addresses are not valid numbers");
				return false;
			}
			
			if(!IsValidMemoryAddress(addrFrom) || !IsValidMemoryAddress(addrTo))
			{
				ErrorPopup("One or more memory addresses out of bounds");
				return false;
			}
			
			return true;
		}
		
		private void PopulateProgramTable()
		{
			if(GetMemoryRange(programTableFrom, programTableTo, ref prgmAddrFrom, ref prgmAddrTo))
			{
				programTableNodes.Clear();
				for(int i = prgmAddrFrom; i <= prgmAddrTo; ++i)
				{
					ProgramTableNode node = new ProgramTableNode(vm.Memory, i);
					if(node.Wide)
					{
						++i;
					}
					programTableNodes.AddNode(node);
				}
			}
		}
		
		private void PopulateMemoryTable()
		{
			int memFrom = 0;
			int memTo = 0;
			if(GetMemoryRange(memoryTableFrom, memoryTableTo, ref memFrom, ref memTo))
			{
				memoryTableNodes.Clear();
				for(int i = memFrom; i <= memTo; ++i)
				{
					MemoryTableNode node = new MemoryTableNode(vm.Memory, i);
					memoryTableNodes.AddNode(node);
				}
			}
		}
		
		protected void OpenActivated (object sender, System.EventArgs e)
		{
			Gtk.FileChooserDialog fc = new Gtk.FileChooserDialog(
				"Choose the file to open",
				this,
				Gtk.FileChooserAction.Open,
				"Cancel", Gtk.ResponseType.Cancel,
				"Open", Gtk.ResponseType.Accept);

			if (fc.Run() == (int)Gtk.ResponseType.Accept) 
			{
				vm.LoadProgram(fc.Filename);
				vm.Reset();
				PopulateProgramTable();
				QueueDraw();
			}
			
			fc.Destroy();
		}

		protected void OnStep (object sender, System.EventArgs e)
		{
			vm.Step();
			int pc = vm.Registers.ProgramCounter.Unsigned;
			if(pc >= prgmAddrFrom && pc < prgmAddrTo)
			{
				int n = 0;
				foreach(ProgramTableNode node in programTableNodes)
				{
					if(node.address == pc)
					{
						programTable.SetCursor(new Gtk.TreePath(new int[]{n}), programTable.GetColumn(0), false);
						break;
					}
					++n;
				}
			}
			QueueDraw();
		}

		protected void programTableUpdateClick (object sender, System.EventArgs e)
		{
			PopulateProgramTable();
		}

		protected void MemoryTableUpdateButtonPressed (object sender, System.EventArgs e)
		{
			PopulateMemoryTable();
		}
	}
}

