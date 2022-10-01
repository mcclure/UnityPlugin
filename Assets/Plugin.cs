using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

// TODO: Call [MonoPInvokeCallback (typeof (StringCallback))] on AOT platforms

public class Plugin {
	// Callback: Print string
	public delegate void StringCallback (string s);

	public static void StandardDebugPrint(string s) {
		Debug.Log("CPLUGIN: " + s);
	}

	// ASSUMES NONMOVING GC
	public delegate int CreateMeshCallback ();
	public delegate void ResizeMeshCallback (int id, int len);
	public delegate IntPtr GetBufferPointerCallback (int len, int buf);
	public delegate void FlushMeshCallback (int len);

	#if UNITY_IPHONE && !UNITY_EDITOR
	[DllImport ("__Internal")]
	#else
	[DllImport("UnityPlugin")]
	#endif
	public static extern int UnityPluginCustomTest();

	#if UNITY_IPHONE && !UNITY_EDITOR
	[DllImport ("__Internal")]
	#else
	[DllImport("UnityPlugin")]
	#endif
	public static extern void UnityPluginInit(string loadPath, StringCallback s, CreateMeshCallback cm, ResizeMeshCallback rm, GetBufferPointerCallback gbp, FlushMeshCallback fm);

	#if UNITY_IPHONE && !UNITY_EDITOR
	[DllImport ("__Internal")]
	#else
	[DllImport("UnityPlugin")]
	#endif
	public static extern void UnityPluginUpdate();


}
