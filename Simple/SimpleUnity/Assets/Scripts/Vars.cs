using UnityEngine;
using System.Collections;
using Nettention.Proud;
using System.Collections.Generic;

namespace SimpleUnity
{
	public class Vars
	{
		public static System.Guid Version = new System.Guid("{0x3ae33249,0xecc6,0x4980,{0xbc,0x5d,0x7b,0xa,0x99,0x9c,0x7,0x39}}");
		public static int ServerPort = 33334;

		public Vars()
		{

		}
	}

	// 사용자 정의 클래스
	public class MyClass
	{
		public int a;
		public float b;
		public double c;
	}

	public class MyMarshaler : Nettention.Proud.Marshaler
	{

		public static bool Read(Message msg, out MyClass value)
		{
			value = new MyClass();
			if (msg.Read(out value.a) && msg.Read(out value.b) && msg.Read(out value.c))
				return true;

			return false;
		}

		public static void Write(Message msg, MyClass value)
		{
			msg.Write(value.a);
			msg.Write(value.b);
			msg.Write(value.c);
		}

		public static void Read(Message msg, out List<int> value)
		{
			value = new List<int>();

			int count = 0;
			msg.ReadScalar(ref count);

			for (int i = 0; i < count; ++i)
			{
				int elem = 0;
				msg.Read(out elem);
				value.Add(elem);
			}
		}

		public static void Write(Message msg, List<int> value)
		{
			int size = value.Count;

			msg.WriteScalar(size);

			foreach (int temp in value)
			{
				msg.Write(temp);
			}
		}

		public static void Read(Message msg, out Dictionary<int, float> value)
		{
			value = new Dictionary<int, float>();

			int count = 0;
			msg.ReadScalar(ref count);

			for (int i = 0; i < count; ++i)
			{
				int key = 0;
				float elem = 0;
				msg.Read(out key);
				msg.Read(out elem);
				value.Add(key, elem);
			}
		}

		public static void Write(Message msg, Dictionary<int, float> value)
		{
			int size = value.Count;

			msg.WriteScalar(size);

			foreach (KeyValuePair<int, float> pair in value)
			{
				msg.Write(pair.Key);
				msg.Write(pair.Value);
			}
		}

	}

}
