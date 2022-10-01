using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.IO;

public class PluginTest : MonoBehaviour {
	// Ugly growable array: For PluginMesh
	public class ManualList<T> {
		public T[] arry;
		GCHandle handle;
		public void Resize(int len) {
			if (arry == null || arry.Length < len) {
				if (handle.IsAllocated) {
					handle.Free();
				}
				arry = new T[len];
			}
		}
		public IntPtr Pointer() {
			if (!handle.IsAllocated)
				handle = GCHandle.Alloc(arry, GCHandleType.Pinned);
			return handle.AddrOfPinnedObject();
		}
		~ManualList() {
			if (handle.IsAllocated)
				handle.Free();
		}
	}

	// Representation of a mesh
	public class PluginMesh {
		public GameObject obj;
		public Mesh mesh;
		public List<int> index = new List<int>();
		public ManualList<Vector3> vertices = new ManualList<Vector3>();
		public ManualList<Color> colors = new ManualList<Color>();
		public bool reIndex;
		public PluginMesh(GameObject parent) {
			obj = new GameObject("CPlugin");
			obj.transform.parent = parent.transform;
			mesh = new Mesh();
			mesh.MarkDynamic();
			MeshRenderer renderer = obj.AddComponent<MeshRenderer>(); // TODO: Material
			MeshFilter filter = obj.AddComponent<MeshFilter>();
			filter.mesh = mesh;
		}
	}

	List<PluginMesh> meshes = new List<PluginMesh>();

	public static bool SnipTo<T>(List<T> l, int len) {
		if (l.Count > len) {
			l.RemoveRange(len, l.Count - len);
			return true;
		}
		return false;
	}

	public static bool ExtendTo<T>(List<T> l, int len) {
		while (l.Count < len) {
			l.Add(default(T));
//			Repeat<T> r = new Repeat<T>(default(T), l - l.Count); // Slower, creates garbage?
//			l.AddRange(r);
			return true;
		}
		return false;
	}

	public int CreateMesh () {
		int id = meshes.Count;
		meshes.Add(new PluginMesh(gameObject));
		return id;
	}
	public IntPtr GetBufferPointer(int id, int buf) {
		PluginMesh m = meshes[id];
		if (buf == 0) return m.vertices.Pointer();
		return m.colors.Pointer();
	}
	public void ResizeMesh (int id, int len) {
		Debug.Log("Ugh " + len);
		PluginMesh m = meshes[id];
		m.vertices.Resize(len*4);
		m.colors.Resize(len*4);

		if (m.index.Count/6 < len) {
			for(int c = m.index.Count/6; c < len; c++) {
				m.index.Add(c*4);
				m.index.Add(c*4+3);
				m.index.Add(c*4+1);
				m.index.Add(c*4+1);
				m.index.Add(c*4+3);
				m.index.Add(c*4+2);
			}
			m.reIndex = true;
		} else {
			m.reIndex = SnipTo(m.index, len*6);
		}
	}
	public void FlushMesh (int id) {
		PluginMesh m = meshes[id];
		m.mesh.vertices = m.vertices.arry;
		m.mesh.colors = m.colors.arry;
		if (m.reIndex) {
			m.mesh.SetTriangles(m.index, 0);
			m.reIndex = false;
		}
	}

	// Use this for initialization
	void Start () {
		Plugin.UnityPluginInit(Path.GetDirectoryName(Application.dataPath), Plugin.StandardDebugPrint, CreateMesh, ResizeMesh, GetBufferPointer, FlushMesh);
	}
	
	// Update is called once per frame
	void Update () {
		Plugin.UnityPluginUpdate();
	}
}
