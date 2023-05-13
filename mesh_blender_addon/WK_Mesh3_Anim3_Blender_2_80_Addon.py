# WK Mesh3/Anim3 Import/Export Addon for Blender 2.80
# Supports mesh vertices, faces, UV maps, materials, attachment points, ...)
# Version 0.1.1
#
# Copyright (c) 2018-2019 AdrienTD
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

bl_info = {
"name": "Warrior Kings Mesh3/Anim3 Tools",
"description": "Import Mesh3 and export Mesh3 and Anim3 files for Warrior Kings (Battles).",
"author": "AdrienTD",
"version": (0,1,1),
"blender": (2,80,0),
"location": "File > Import/Export > Warrior Kings Mesh/Animation",
"category": "Import-Export",
}

import bpy
import array
import struct
import math
import bpy_extras
import sys
import traceback
import os

normaltable = (
(0.808092, 0.444655, -0.386355), (-0.456426, -0.729133, 0.509942), (-0.600703, -0.527135, -0.601069), (-0.898221, -0.203762, -0.389462), (0.243796, -0.096599, 0.965004), (0.608059, 0.752839, -0.251987), (-0.421935, 0.886955, -0.187833), (0.910585, 0.268347, 0.314364), (-0.673626, -0.723759, 0.149674), (0.625561, 0.209396, -0.751550), (-0.696196, 0.269262, -0.665439), (-0.184171, -0.982517, -0.027217), (0.343731, -0.067672, -0.936627), (-0.602991, 0.737327, -0.304552), (-0.587473, 0.510824, -0.627642), (-0.647096, -0.554946, 0.522784),
(-0.354654, -0.257923, -0.898719), (0.274678, 0.801876, -0.530610), (-0.781383, -0.376611, -0.497599), (0.317486, 0.646651, -0.693574), (0.055929, 0.990906, -0.122380), (0.345819, -0.737888, 0.579594), (-0.451111, -0.887842, -0.090751), (-0.912034, 0.211783, -0.351201), (-0.620471, -0.347854, 0.702861), (-0.596301, 0.579799, 0.555210), (-0.641796, 0.685285, 0.344212), (0.428539, -0.656028, -0.621275), (0.363572, -0.323209, 0.873700), (0.264355, -0.282562, -0.922104), (0.787559, 0.011651, -0.616129), (0.844233, 0.182530, 0.503938),
(-0.969711, 0.243394, 0.020489), (-0.624959, 0.363099, 0.691075), (0.992850, -0.118391, 0.015265), (-0.335802, 0.736196, -0.587582), (0.227509, -0.683621, -0.693471), (0.040555, -0.645209, 0.762929), (-0.692354, 0.103003, 0.714168), (-0.780787, 0.560530, -0.276004), (-0.661048, 0.094352, -0.744387), (-0.372030, -0.673144, -0.639117), (0.013698, 0.494687, -0.868963), (0.546409, -0.638330, 0.542192), (-0.201223, 0.574127, 0.793655), (-0.547434, 0.836684, -0.016601), (-0.868532, 0.081126, 0.488948), (0.300499, -0.473701, -0.827833),
(-0.786697, 0.602525, 0.134431), (-0.054725, 0.273011, -0.960453), (0.570862, -0.031635, -0.820436), (0.738818, -0.585357, -0.333924), (0.945515, -0.267979, 0.184903), (-0.927834, -0.368990, -0.054505), (-0.461693, -0.542551, 0.701768), (0.454071, 0.392386, 0.799908), (-0.416641, 0.905267, 0.083083), (0.504375, 0.807268, 0.306470), (-0.203602, -0.553856, 0.807335), (0.679141, 0.413146, -0.606694), (0.485646, 0.440706, -0.754935), (0.673468, -0.720994, -0.163123), (0.886304, -0.461019, 0.043884), (0.104072, -0.815899, 0.568752),
(-0.738915, -0.141810, 0.658707), (-0.691632, -0.715600, -0.097782), (-0.441185, 0.771982, 0.457602), (0.441951, -0.870455, 0.216764), (-0.640856, 0.759761, 0.109846), (0.947975, 0.317187, 0.027132), (-0.808657, -0.585839, 0.053544), (0.081409, 0.868901, 0.488246), (-0.411668, 0.449848, -0.792569), (0.086523, -0.123656, -0.988546), (-0.444450, 0.823739, -0.352021), (-0.735922, -0.564408, -0.373982), (-0.792320, -0.076378, -0.605307), (-0.069488, 0.765591, 0.639564), (-0.308008, 0.912053, 0.270724), (0.387621, 0.155368, 0.908631),
(0.424741, 0.902984, 0.064917), (0.042874, 0.915413, -0.400227), (0.636084, -0.386358, -0.667926), (-0.308196, -0.160207, 0.937736), (-0.565209, -0.668202, -0.483781), (-0.180208, -0.605016, -0.775552), (-0.881837, -0.373109, -0.288363), (-0.389257, -0.488844, -0.780712), (0.809370, 0.549431, -0.207476), (-0.999680, 0.015266, 0.020168), (0.399733, 0.209016, -0.892483), (0.899296, 0.132047, -0.416931), (-0.286871, 0.685846, 0.668819), (-0.245328, -0.896727, 0.368367), (-0.098337, 0.705913, -0.701439), (0.694000, 0.352968, 0.627517),
(-0.461285, 0.227670, -0.857544), (-0.758681, 0.643233, -0.103219), (-0.834269, -0.526813, -0.162674), (0.718549, -0.562647, 0.408799), (-0.821934, 0.165341, -0.545057), (-0.423710, 0.477169, 0.769922), (-0.236313, 0.343992, -0.908750), (-0.195587, -0.795425, -0.573624), (-0.603167, -0.147616, -0.783837), (0.833170, -0.516343, -0.198034), (0.649966, 0.603441, -0.461956), (-0.562514, -0.138747, 0.815063), (0.600099, -0.741469, 0.300175), (-0.969717, -0.199623, -0.140710), (-0.158053, -0.830590, 0.533985), (-0.287408, -0.671873, 0.682630),
(-0.172117, 0.523199, -0.834649), (0.799135, -0.215860, -0.561060), (0.878999, -0.327944, -0.346140), (0.049084, -0.543296, -0.838105), (0.024893, -0.936850, 0.348845), (0.630469, 0.622958, 0.463069), (0.081136, -0.994592, 0.064839), (-0.527722, -0.354519, -0.771897), (-0.539652, -0.794932, -0.277233), (0.673748, -0.129787, 0.727475), (-0.132092, 0.909540, 0.394067), (-0.330035, 0.161145, 0.930112), (-0.890556, -0.128632, 0.436308), (0.015770, 0.661932, 0.749398), (-0.038407, 0.971738, 0.232917), (-0.242240, 0.968054, -0.064735),
(-0.974538, -0.006960, -0.224114), (0.484612, -0.286142, -0.826604), (0.490734, -0.460765, -0.739511), (0.563716, 0.824340, -0.051845), (-0.351184, -0.833821, -0.425926), (0.450206, -0.539459, 0.711546), (-0.279406, -0.930567, -0.236597), (0.202571, 0.737635, 0.644096), (-0.895900, 0.434866, -0.090856), (-0.333947, -0.935654, 0.114149), (-0.470405, -0.830342, 0.298748), (-0.404353, -0.385236, 0.829513), (0.682490, -0.379429, 0.624693), (-0.135019, -0.976950, 0.165344), (0.979605, 0.071204, -0.187893), (0.830159, -0.313955, 0.460727),
(-0.151092, -0.435964, -0.887191), (-0.019763, 0.178444, 0.983752), (0.157083, 0.107041, -0.981767), (0.738362, -0.656336, 0.155065), (-0.757158, 0.364606, 0.542009), (0.952384, -0.178236, -0.247380), (0.287041, 0.467366, -0.836168), (0.419787, -0.811948, 0.405611), (0.898981, -0.060293, -0.433817), (-0.007519, -0.712811, -0.701316), (0.455687, -0.101982, 0.884279), (-0.076145, -0.246821, -0.966065), (0.588239, -0.806938, 0.053158), (0.071673, -0.845833, -0.528610), (-0.446631, -0.002592, -0.894714), (0.108493, -0.284943, 0.952385),
(0.261393, 0.918640, -0.296266), (-0.189784, -0.070330, -0.979304), (0.805214, 0.209550, -0.554725), (0.940139, -0.331645, -0.078429), (-0.875143, 0.349788, 0.334324), (-0.249029, 0.160225, -0.955150), (0.165088, 0.981329, 0.098686), (0.942236, 0.087354, 0.323359), (0.521263, -0.781007, -0.343967), (0.851518, 0.523399, 0.031157), (-0.144472, -0.329223, 0.933135), (0.319886, -0.834856, -0.447982), (-0.181725, 0.838811, -0.513198), (0.922799, 0.331337, -0.196618), (-0.903520, 0.025291, -0.427799), (-0.470497, 0.273433, 0.838968),
(0.191713, -0.533305, 0.823912), (0.175347, 0.331251, -0.927106), (0.067668, -0.394194, -0.916533), (-0.970735, 0.186269, -0.151581), (0.047399, -0.440209, 0.896643), (-0.042103, 0.069657, -0.996682), (0.848974, 0.469431, 0.242645), (0.725640, 0.136628, 0.674374), (0.459494, 0.702795, 0.543088), (-0.771823, -0.556260, 0.307999), (-0.793686, 0.371934, -0.481380), (0.255866, -0.925992, 0.277619), (-0.892083, 0.423660, 0.157165), (-0.965763, -0.210795, 0.151219), (-0.196676, 0.930433, -0.309215), (-0.556744, 0.335791, -0.759790),
(-0.236658, 0.338326, 0.910785), (0.795864, -0.095783, 0.597851), (0.349423, -0.935995, 0.042628), (0.703558, 0.661548, 0.259539), (-0.883665, -0.435100, 0.172695), (-0.024928, -0.988799, -0.147159), (-0.153379, 0.983738, 0.093456), (0.478172, 0.618047, -0.623995), (-0.704287, -0.305509, -0.640815), (-0.753973, -0.360344, 0.549251), (0.773952, 0.439592, 0.455804), (0.432864, 0.787534, -0.438656), (-0.973200, -0.005774, 0.229889), (0.070277, 0.817631, -0.571437), (0.655257, -0.228501, -0.720018), (0.587018, 0.501629, 0.635435),
(0.565664, 0.268641, 0.779651), (0.791825, -0.608283, -0.054825), (0.239913, 0.931732, 0.272613), (0.119877, 0.648519, -0.751700), (0.233140, -0.855030, 0.463216), (0.447147, 0.877217, -0.174783), (-0.095030, -0.924959, -0.367994), (0.024404, 0.444556, 0.895419), (0.416874, -0.890209, -0.183698), (0.197727, -0.928956, -0.312962), (0.621295, 0.043942, 0.782344), (0.219016, -0.694018, 0.685836), (-0.536531, -0.838573, 0.094495), (0.217455, -0.968593, -0.120589), (0.989822, 0.105536, 0.095467), (0.234608, 0.555054, 0.798044),
(0.604123, -0.611391, -0.511113), (0.203710, 0.307910, 0.929351), (-0.625561, -0.701734, 0.340943), (-0.768539, 0.522992, 0.368548), (-0.028858, -0.113245, 0.993148), (-0.959738, 0.171029, 0.222827), (-0.841200, -0.389882, 0.374666), (0.318881, 0.839267, 0.440392), (-0.488911, 0.834567, 0.253897), (0.409386, 0.580371, 0.703969), (-0.245623, 0.814703, 0.525289), (0.920400, -0.176771, 0.348735), (0.320419, 0.945050, -0.064898), (-0.882297, 0.368563, -0.292767), (-0.479609, 0.722761, -0.497587), (0.844571, -0.479234, 0.238820),
(0.740620, 0.671214, 0.030870), (-0.675182, 0.593496, -0.438055), (-0.352247, 0.617404, -0.703373), (0.558015, -0.296253, 0.775148), (-0.178627, 0.016406, 0.983780), (0.603347, 0.784862, 0.141294), (0.133218, 0.088095, 0.987164), (0.777528, -0.403521, -0.482308), (0.976820, -0.064991, 0.203957), (-0.462412, 0.590766, 0.661189), (-0.563169, 0.112945, 0.818587), (-0.425479, 0.015978, 0.904827), (0.449499, 0.049594, -0.891903), (-0.112093, -0.707121, 0.698151), (-0.676042, -0.687868, -0.264207), (0.111249, -0.080442, 0.990532),
)

def writestr(outputfile, a):
	outputfile.write(bytes(a))

def writepack(outputfile, a, *b):
	outputfile.write(struct.pack("<" + a, *b))

def writearray(outputfile, *a):
	array.array(*a).tofile(outputfile)

def readpack(inputfile, fmt):
	return struct.unpack("<" + fmt, inputfile.read(struct.calcsize("<" + fmt)))

def readstr(inputfile):
	str = ""
	while True:
		c = inputfile.read(1)
		if (c == b"\0") or (c == b""): break
		str += c.decode()
	return str

def dbg(*a):
	print("WkMesh3Anim3Export: ", *a)

def showexc(self):
	if self != None:
		self.report({"ERROR"}, str(sys.exc_info()[1]) + "\n\n" + traceback.format_exc())
		traceback.print_exc()

def FindNearestNormalTableIndex(nrm):
	bestindex = 0
	bestdp = -1
	for i in range(256):
		cdd = normaltable[i]
		dp = nrm[0] * cdd[0] + nrm[1] * cdd[1] + nrm[2] * cdd[2]
		if dp >= bestdp:
			bestindex = i
			bestdp = dp
	return bestindex

def WriteSphere(outfile, mesh, swapyz):
	center = [0,0,0]
	mmdiff = [0,0,0]
	for c in range(3):
		crdmin = crdmax = mesh.vertices[0].co[c]
		for v in mesh.vertices:
			if v.co[c] < crdmin:
				crdmin = v.co[c]
			if v.co[c] > crdmax:
				crdmax = v.co[c]
		center[c] = (crdmin + crdmax) / 2
		mmdiff[c] = crdmax - crdmin
	if swapyz:
		center[1],center[2] = center[2],center[1]
	writepack(outfile, "ffff", center[0], center[1], center[2], max(mmdiff)/2)

#################
##### Mesh3 #####
#################

def ImportMesh3(input_filename, swap_yz_coords=True, self=None):
	infile = None
	obj = None
	mesh = None

	try:
		dbg("--- ImportMesh3 Beginning ---")

		if swap_yz_coords:
			def syz(v):
				return (v[0],v[2],v[1])
		else:
			def syz(v):
				return v

		infile = open(input_filename, "rb")
		signature = readpack(infile, "4s")[0]
		dbg(signature)
		if signature != b"Mesh":
			raise BaseException("File is not a Mesh3 file!")

		ver,unk = readpack(infile, "II")

		dirname = os.path.dirname(input_filename)
		objname = os.path.basename(input_filename)
		mesh = bpy.data.meshes.new(objname + "_Mesh")
		obj = bpy.data.objects.new(objname, mesh)
		bpy.context.scene.collection.objects.link(obj)

		dbg("Attachment points")
		nparts, = readpack(infile, "H")
		for i in range(nparts):
			ap = bpy.data.objects.new("%s_AttachPnt%03i" % (objname, i), None)
			bpy.context.scene.collection.objects.link(ap)
			ap.attachpnt_tag = readstr(infile)
			ap.location = syz(readpack(infile, "fff"))
			ap.rotation_mode = 'QUATERNION'
			ap.rotation_quaternion = readpack(infile, "ffff")
			ap.attachpnt_on, = readpack(infile, "B")
			ap.attachpnt_file = readstr(infile)
			ap.is_attachpnt = True
			ap.parent = obj

		dbg("Vertex positions")
		nverts, = readpack(infile, "H")
		verts = []
		for i in range(nverts):
			verts.append(syz(readpack(infile, "fff")))

		# Sphere
		readpack(infile, "ffff")

		# Vertex position remapper
		if ver == 3:
			nposremaps, = readpack(infile, "H")
			infile.seek(2*nposremaps, os.SEEK_CUR)

		# Normal constructors
		nnorm,nnele = readpack(infile, "HI")
		infile.seek(2*nnele, os.SEEK_CUR)

		dbg("Materials")
		nmat, = readpack(infile, "H")
		mats = []
		for i in range(nmat):
			f, = readpack(infile, "B")
			s = readstr(infile)
			n = s
			if f & 1: n += "_AlphaTest"
			m = bpy.data.materials.new(n)
			mesh.materials.append(m)
			m.use_nodes = True
			m.blend_method = 'CLIP' if (f & 1) else 'OPAQUE'
			m.alpha_threshold = 0.94
			mats.append(m)
			
			bsdf = m.node_tree.nodes['Principled BSDF']
			imgnode = m.node_tree.nodes.new('ShaderNodeTexImage')
			m.node_tree.links.new(imgnode.outputs['Color'], bsdf.inputs['Base Color'])
			imgpath = dirname + "/" + s
			if os.path.exists(imgpath):
				imgnode.image = bpy.data.images.load(imgpath, check_existing=True)

		dbg("Texture coordinates")
		nuvlist, = readpack(infile, "I")
		texcrds = []
		for i in range(nuvlist):
			mesh.uv_layers.new(name="UVList_" + str(i))
			ntexc, = readpack(infile, "H")
			tc = []
			for j in range(ntexc):
				u,v = readpack(infile, "ff")
				tc.append((u,1-v))
			texcrds.append(tc)

		dbg("Groups")
		ngrp, = readpack(infile, "I")
		assert nmat == ngrp
		grps = []
		for i in range(ngrp):
			sg, = readpack(infile, "H")
			l = []
			for j in range(sg):
				l.append(readpack(infile, "HHH"))
			grps.append(l)

		dbg("Polygon list")
		ntrilist, = readpack(infile, "I")

		# Just looking at the first polygon list at the moment.
		polylist = readpack(infile, "HHH")
		sample_distance = readpack(infile, "f")

		plist = []
		for i in range(nmat):
			sgrp,vused,matx = readpack(infile, "HHH")
			l = []
			for j in range(sgrp):
				l.append(list(syz(readpack(infile, "HHH"))))
			plist.append(l)

		faces = []
		lpsuv = []
		plmat = []
		for i in range(nmat):
			for t in plist[i]:
				z = [0,0,0]
				uv = [0,0,0]
				for j in range(3):
					z[j] = grps[i][t[j]][0]
					lpsuv.append(grps[i][t[j]][2])
				faces.append(z)
				plmat.append(i)

		mesh.from_pydata(verts, [], faces)

		for c in range(nuvlist):
			uvl = mesh.uv_layers[c]
			dbg(len(lpsuv), " ", len(uvl.data))
			assert len(lpsuv) == len(uvl.data)
			for i in range(len(lpsuv)):
				uvl.data[i].uv = texcrds[c][lpsuv[i]]

		for i in range(len(plmat)):
			mesh.polygons[i].material_index = plmat[i]

	except:
		if infile != None:
			dbg("Stopped file reading at 0x%X." % infile.tell())
		showexc(self)

	dbg("--- ImportMesh3 End ---")

	if infile != None:
		infile.close()

def ExportMesh3(obj, output_filename, apply_modifiers=False, swap_yz_coords=True, self=None):
	outfile = None
	aplist = []
	mesh = None

	try:
		dbg("--- ExportMesh3 Beginning ---")

		if obj == None:
			raise BaseException("Please select an object.")
		if obj.type != 'MESH':
			raise BaseException("The active object must be a MESH object!")

		if apply_modifiers:
			eval_obj = obj.evaluated_get(bpy.context.evaluated_depsgraph_get())
			mesh =  eval_obj.to_mesh().copy()
			assert len(mesh.vertices) == len(obj.data.vertices)
		else:
			mesh = obj.data

		for a in obj.children:
			if a.is_attachpnt:
				aplist.append(a)

		if swap_yz_coords:
			def syz(v):
				return v.xzy
		else:
			def syz(v):
				return v

		outfile = open(output_filename, "wb")

		writestr(outfile, b"Mesh")
		writepack(outfile, "II", 3, 0)

		dbg("Attachment points")
		writepack(outfile, "H", len(aplist))
		for a in aplist:
			orm = a.rotation_mode
			a.rotation_mode = 'QUATERNION'
			writestr(outfile, a.attachpnt_tag.encode() + b"\0")
			writearray(outfile, 'f', syz(a.location))
			writearray(outfile, 'f', a.rotation_quaternion)
			writepack(outfile, "B", a.attachpnt_on)
			writestr(outfile, a.attachpnt_file.encode() + b"\0")
			a.rotation_mode = orm

		dbg("Vertex positions")
		nverts = len(mesh.vertices)
		writepack(outfile, "H", nverts)
		for v in mesh.vertices:
			writearray(outfile, 'f', syz(v.co))

		dbg("Sphere")
		WriteSphere(outfile, mesh, swap_yz_coords)

		dbg("Position remapper")
		writepack(outfile, "H", nverts)
		for i in range(nverts):
			writepack(outfile, 'H', i)

		dbg("Normals")
		writepack(outfile, "HI", nverts, 2*nverts)
		for i in range(nverts):
			writepack(outfile, 'HH', 1, 0)

		dbg("Materials")
		nmats = len(mesh.materials)
		writepack(outfile, "H", nmats)
		for m in mesh.materials:
			if m.blend_method != 'OPAQUE':
				writepack(outfile, "B", 1)
			else:
				writepack(outfile, "B", 0)
			imgname = "Gold.tga"
			try:
				bsdf = m.node_tree.nodes['Principled BSDF']
				imgnode = bsdf.inputs['Base Color'].links[0].from_node
				imgname = imgnode.image.name
			except:
				self.report({"WARNING"}, "Unable to get the texture name of material '%s', Gold.tga is used instead." % m.name)

			writestr(outfile, imgname.encode())
			writepack(outfile, "B", 0)

		dbg("Texture coordinates")
		nuvlists = len(mesh.uv_layers)
		assert nuvlists >= 1, "This mesh does not have any UV maps. At least one UV map is required."
		ntxcrds = len(mesh.uv_layers[0].data)
		writepack(outfile, "I", nuvlists)
		for l in mesh.uv_layers:
			assert len(l.data) == ntxcrds
			writepack(outfile, "H", ntxcrds)
			for u in l.data:
				writepack(outfile, 'ff', u.uv.x, 1-u.uv.y)

		dbg("Groups")
		grplist = []
		writepack(outfile, "I", nmats)
		assert len(obj.material_slots) >= 1, "The mesh does not have at any material. At least one material is required."
		for m in range(len(obj.material_slots)):
			gl = []
			for p in mesh.polygons:
				if p.material_index == m:
					for l in p.loop_indices[2:]:
						gl.append(p.loop_start)
						gl.append(l - 1)
						gl.append(l)
			grplist.append(gl)
			writepack(outfile, "H", len(gl))
			for l in gl:
				writepack(outfile, "HHH", mesh.loops[l].vertex_index, mesh.loops[l].vertex_index, l)

		dbg("Polygon list")
		writepack(outfile, "I", 1)
		writepack(outfile, "HHHf", nverts, nverts, ntxcrds, 0)
		for m in range(len(obj.material_slots)):
			gl = grplist[m]
			writepack(outfile, "HHH", len(gl)//3, len(gl), m)
			for l in range(len(gl)//3):
				if swap_yz_coords:
					writepack(outfile, "HHH", 3*l, 3*l+2, 3*l+1)
				else:
					writepack(outfile, "HHH", 3*l, 3*l+1, 3*l+2)

		dbg("Normals")
		for v in mesh.vertices:
			nrm = v.normal
			writepack(outfile, "B", FindNearestNormalTableIndex(syz(nrm)))

		dbg("Normal remapper")
		writepack(outfile, "H", nverts)
		for i in range(nverts):
			writepack(outfile, "H", i)

		self.report({"INFO"}, "Mesh3 exported successfully!")
	except:
		showexc(self)

	dbg("--- ExportMesh3 End ---")

	if outfile != None:
		outfile.close()

	if apply_modifiers:
		if mesh != None:
			bpy.data.meshes.remove(mesh)


#################
##### Anim3 #####
#################

def ExportAnim3(obj, output_filename, meshname, swap_yz_coords=True, self=None):
	outfile = None
	frmeshs = []
	aplist = []

	try:
		dbg("--- ExportAnim3 Beginning ---")

		if obj == None:
			raise BaseException("Please select an object.")
		if obj.type != 'MESH':
			raise BaseException("The active object must be a MESH object!")
		scn = bpy.context.scene

		nverts = len(obj.data.vertices)
		nobjframes = scn.frame_end - scn.frame_start + 1
		frspeed = math.trunc(1000/24)
		assert nobjframes >= 2, "At least 2 animation frames are required."

		ap_loc = []
		ap_rot = []
		ap_on = []

		for a in obj.children:
			if a.is_attachpnt:
				aplist.append(a)
				ap_loc.append([])
				ap_rot.append([])
				ap_on.append([])

		for i in range(nobjframes):
			scn.frame_set(scn.frame_start + i)
			eval_obj = obj.evaluated_get(bpy.context.evaluated_depsgraph_get())
			m =  eval_obj.to_mesh()
			#m = obj.to_mesh()
			assert len(m.vertices) == nverts
			frmeshs.append(m.copy())

			for x in range(len(aplist)):
				a = aplist[x]
				orm = a.rotation_mode
				a.rotation_mode = 'QUATERNION'
				ap_loc[x].append(a.location.copy())
				ap_rot[x].append(a.rotation_quaternion.copy())
				ap_on[x].append(a.attachpnt_on)
				a.rotation_mode = orm

		if swap_yz_coords:
			def syz(v):
				return v.xzy
		else:
			def syz(v):
				return v

		outfile = open(output_filename, "wb")

		dbg("Header")
		writestr(outfile, b"Anim")
		writepack(outfile, "I", 3)
		writestr(outfile, meshname.encode() + b"\0")

		writepack(outfile, "I", (nobjframes-1) * frspeed)
		WriteSphere(outfile, frmeshs[0], swap_yz_coords)
		writepack(outfile, "I", nverts)

		def WriteFrameTimes():
			writepack(outfile, "I", nobjframes)
			for i in range(nobjframes):
				writepack(outfile, "I", i * frspeed)

		dbg("Vertices")
		crdrg = (0,2,1) if swap_yz_coords else (0,1,2)
		for c in crdrg:
			WriteFrameTimes()
			for f in range(nobjframes):
				m = frmeshs[f]
				# Search for min and max
				crdmin = crdmax = m.vertices[0].co[c]
				for v in m.vertices:
					if v.co[c] > crdmax:
						crdmax = v.co[c]
					if v.co[c] < crdmin:
						crdmin = v.co[c]
				crdadd = crdmin
				crdmul = crdmax-crdmin
				writepack(outfile, "ff", crdadd, crdmul)

				def writegroup(i, s):
					b = 0
					for j in range(s):
						a = round((m.vertices[3*i+j].co[c] - crdadd) * 1023 / crdmul)
						assert 0 <= a <= 1023
						b |= a << (j*11)
					writepack(outfile, "I", b)

				for i in range(nverts//3):
					writegroup(i, 3)
				if nverts % 3 != 0:
					writegroup(nverts // 3, nverts % 3)

		dbg("Attachment points")
		writepack(outfile, "I", len(aplist))
		for x in range(len(aplist)):
			WriteFrameTimes()
			for f in range(nobjframes):
				writearray(outfile, 'f', syz(ap_loc[x][f]))
				writearray(outfile, 'f', ap_rot[x][f])
				writepack(outfile, "B", ap_on[x][f])

		dbg("Normals")
		WriteFrameTimes()
		writepack(outfile, "I", nverts)
		for f in range(nobjframes):
			m = frmeshs[f]
			for v in m.vertices:
				nrm = v.normal
				writepack(outfile, "B", FindNearestNormalTableIndex(syz(nrm)))

		self.report({"INFO"}, "Anim3 exported successfully!")
	except:
		showexc(self)

	dbg("--- ExportAnim3 End ---")

	if outfile != None:
		outfile.close()
	for fm in frmeshs:
		bpy.data.meshes.remove(fm)

####

class ExportMesh3Operator(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
	"""Export the active object's static mesh to Mesh3 format for Warrior Kings."""
	bl_idname = "wkm3a3export.export_mesh3"
	bl_label = "Export to .mesh3"

	filename_ext = ".mesh3"

	apply_modifiers: bpy.props.BoolProperty(name="Apply modifiers", description="Apply modifiers (e.g. armature) to the vertices at the current frame when exporting the mesh.", default=False)
	swap_yz_coords: bpy.props.BoolProperty(name="Swap Y & Z coordinates", description="Swap the Y & Z coordinates of vertices, normals and attachment points.", default=True)

	def execute(self, context):
		ExportMesh3(context.active_object, self.filepath, self.apply_modifiers, self.swap_yz_coords, self)
		return {"FINISHED"}

class ExportAnim3Operator(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
	"""Export the active object's animation to Anim3 format for Warrior Kings."""
	bl_idname = "wkm3a3export.export_anim3"
	bl_label = "Export to .anim3"

	filename_ext = ".anim3"

	meshname: bpy.props.StringProperty(name="Mesh3 file", description="Name of Mesh3 file the animation is based on / linked to", default="Default.mesh3")
	swap_yz_coords: bpy.props.BoolProperty(name="Swap Y & Z coordinates", description="Swap the Y & Z coordinates of vertices, normals and attachment points.", default=True)

	def execute(self, context):
		ExportAnim3(context.active_object, self.filepath, self.meshname, self.swap_yz_coords, self)
		return {"FINISHED"}

class ImportMesh3Operator(bpy.types.Operator, bpy_extras.io_utils.ImportHelper):
	"""Import a Warrior Kings Mesh3 file."""
	bl_idname = "wkm3a3export.import_mesh3"
	bl_label = "Import .mesh3"

	filename_ext = ".mesh3"

	swap_yz_coords: bpy.props.BoolProperty(name="Swap Y & Z coordinates", description="Swap the Y & Z coordinates of vertices, normals and attachment points.", default=True)

	def execute(self, context):
		ImportMesh3(self.filepath, self.swap_yz_coords, self)
		return {"FINISHED"}

def menu_export_mesh3(self, context):
	self.layout.operator(ExportMesh3Operator.bl_idname, text="Warrior Kings Mesh (.mesh3)")

def menu_export_anim3(self, context):
	self.layout.operator(ExportAnim3Operator.bl_idname, text="Warrior Kings Animation (.anim3)")

def menu_import_mesh3(self, context):
	self.layout.operator(ImportMesh3Operator.bl_idname, text="Warrior Kings Mesh (.mesh3)")

class AttachmentPointPanel(bpy.types.Panel):
	"""Attachment Point Panel"""
	bl_label = "WK Attachment Point"
	bl_idname = "OBJECT_PT_wk_attachmentpoint"
	bl_space_type = 'PROPERTIES'
	bl_region_type = 'WINDOW'
	bl_context = "object"

	def draw(self, context):
		layout = self.layout

		obj = context.object

		layout.prop(obj, "is_attachpnt")
		lo = layout.column()
		lo.enabled = obj.is_attachpnt
		lo.label(text="Static properties:")
		box = lo.box()
		box.prop(obj, "attachpnt_tag")
		box.prop(obj, "attachpnt_file")
		lo.label(text="Animatable property:")
		lo.prop(obj, "attachpnt_on")

def register():
	bpy.types.Object.is_attachpnt = bpy.props.BoolProperty(name="Is Attachment Point", description="Indicates whether the object should be exported to Mesh3/Anim3 as an attachment point.")
	bpy.types.Object.attachpnt_tag = bpy.props.StringProperty(name="Tag", description="Tag of the attachment point")
	bpy.types.Object.attachpnt_file = bpy.props.StringProperty(name="File", description="Name of model file (.mesh3/.anim3) attached to the attachment point.\nPath relative to \"Warrior Kings Game Set\\\".")
	bpy.types.Object.attachpnt_on = bpy.props.BoolProperty(default=True, name="On", description="State of the attachment point.\nUsed for visibility in the game, as well as for triggers.\nCan be animated in Anim3.")
	bpy.utils.register_class(AttachmentPointPanel)
	bpy.utils.register_class(ExportMesh3Operator)
	bpy.utils.register_class(ExportAnim3Operator)
	bpy.utils.register_class(ImportMesh3Operator)
	bpy.types.TOPBAR_MT_file_export.append(menu_export_mesh3)
	bpy.types.TOPBAR_MT_file_export.append(menu_export_anim3)
	bpy.types.TOPBAR_MT_file_import.append(menu_import_mesh3)

def unregister():
	del bpy.types.Object.is_attachpnt
	del bpy.types.Object.attachpnt_tag
	del bpy.types.Object.attachpnt_file
	del bpy.types.Object.attachpnt_on
	bpy.utils.unregister_class(AttachmentPointPanel)
	bpy.utils.unregister_class(ExportMesh3Operator)
	bpy.utils.unregister_class(ExportAnim3Operator)
	bpy.utils.unregister_class(ImportMesh3Operator)
	bpy.types.TOPBAR_MT_file_export.remove(menu_export_mesh3)
	bpy.types.TOPBAR_MT_file_export.remove(menu_export_anim3)
	bpy.types.TOPBAR_MT_file_import.remove(menu_import_mesh3)

if __name__ == "__main__":
	register()
