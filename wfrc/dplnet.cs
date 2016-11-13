using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;


namespace wfrc
{
  unsafe public partial class dplnet
  {
    [DllImport("dplnet.dll", CallingConvention =CallingConvention.Cdecl)]
    public static extern int dnaOpen(char* ip, int port, int type);

    [DllImport("dplnet.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int dnaClose(int sd);

    [DllImport("dplnet.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int dnaAccept(int sd, char* ip, int sz);

    [DllImport("dplnet.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int dnaRead(int sd, char* buf, int sz, char* ip, int port);

    [DllImport("dplnet.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int dnaWrite(int sd, char* buf, int sz, char* ip, int port);



    public int DNAOpen(byte[] ip, int port, int type)
    {
      int e = 0;
      fixed (byte* p = ip)
      {
        e = dnaOpen((char*)p, port, type);
      }
      return e;
    }
      
    public int DNAClose(int sd)
    {
        int e = 0;
        e = dnaClose(sd);
        return e;
    }
       
    public int DNAAccept(int sd, byte[] ip,int sz)
    {
        int e = 0;
          
        fixed(byte* p = ip)
        {
            e = dnaAccept(sd, (char*)p,sz);
        }
        return e;
    }
    
    public int DNARead(int sd, byte[] buf, int sz, byte[] ip, int port)
    {
        int e = 0;
        fixed(byte* b = buf)
        fixed(byte* p = ip)
        {
            e = dnaRead(sd, (char*)b, sz, (char*)p, port);
        }
        return e;
    }

    public int DNAWrite(int sd, byte[] buf, int sz, byte[] ip, int port)
    {
        int e = 0;
        fixed(byte* b = buf)
        fixed(byte* p = ip)
        {
            e = dnaWrite(sd, (char*)b, sz, (char*)p, port);
        }   
        return e;
    }
  }
}
      