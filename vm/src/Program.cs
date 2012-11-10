using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ESC64VM
{
//    class PrintDevice : IODevice
//    {
//        /*
//         * #r0: string start
//         * print:
//         *   ldr r1, r0
//         *   cmp r1, 0
//         *   jeq _print_return
//         *   str 0xFFFE, r1
//         *   add r0, r0
//         *   jmp print
//         * _print_return
//         *   mov pc, lr
//         */
//
//        private const int ADDRESS = 0xFFFE;
//
//        public PrintDevice(VirtualMachine vm) : base(vm, ADDRESS)
//        {
//        }
//
//        public override int Read(int address)
//        {
//            return 0;
//        }
//
//        public override void Write(int address, int data)
//        {
//            Console.Write((char)data);
//        }
//    }

    public class Program
    {
        static void Main(string[] args)
        {
			Gtk.Application.Init();
			ESCWindow window = new ESCWindow();
			window.Show();
			Gtk.Application.Run();
        }
    }
}
