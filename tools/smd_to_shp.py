import random
import sys
import traceback


if len(sys.argv) != 3:
    print "usage: smd_to_shp.py <input.smd> <output.shp>"
    print "Converts half life smd files into shape files"
    sys.exit(0)

smdFilename = sys.argv[1]
shpFilename = sys.argv[2]
i = open(smdFilename)
lineNum = 1

try:
    while 1:
        l = i.readline()
        lineNum += 1
        if not l:
            print "Shit"
            sys.exit(0);
        l = l.strip()
        if l == "triangles":
            break

    # Parse triangle defs and form a list of positions and triangles
    positions = []
    vertices = []
    materials = []
    tris = []

    def add_pos(p):
        for (i, val) in enumerate(positions):
            dx = val[0] - p[0]
            dy = val[1] - p[1]
            dz = val[2] - p[2]
            distSqrd = dx ** 2 + dy ** 2 + dz ** 2
            if distSqrd < 0.1:
                return i              
        i = len(positions)
        positions.append(p)
        return i
        
    def add_vert(materialIdx, pointIdx):
        for (i, val) in enumerate(vertices):
            if val == (materialIdx, pointIdx):
                return i        
        i = len(vertices)
        vertices.append((materialIdx, pointIdx))
        return i
        
    def add_tri(v1Idx, v2Idx, v3Idx):
        for (i, val) in enumerate(tris):
            if val == (v1Idx, v2Idx, v3Idx):
                print "Ignoring duplicate triangle"
                return
        tris.append((v1Idx, v2Idx, v3Idx))
                    
    while 1:
        l = i.readline()
        lineNum += 1
        if not l:
            break
        l = l.strip()
        if l == "end":
            break
        
        # Assume material name is actually rgb hex values
        try:
            if len(l) == 7:
                rgb = (int(l[0],16), int(l[1],16), int(l[2],16))
            else:
                rgb = (int(l[0:2],16), int(l[2:4],16), int(l[4:6],16))
        except:
            print "Material name is not a colour on line", lineNum
            print l
            
        try:
            matIdx = materials.index(rgb)
        except:
            matIdx = len(materials)
            materials.append(rgb)
        
        # Vertex defs look like:
        # 0 -542.886719 414.154785 0.000000 0.000000 -0.000000 1.000000 0.000000 0.750000
        #      x            z         y
        p1 = map(float, i.readline().strip().split()[1:4])
        p2 = map(float, i.readline().strip().split()[1:4])
        p3 = map(float, i.readline().strip().split()[1:4])

        p1Idx = add_pos(p1)
        p2Idx = add_pos(p2)
        p3Idx = add_pos(p3)

        v1Idx = add_vert(matIdx, p1Idx)
        v2Idx = add_vert(matIdx, p2Idx)
        v3Idx = add_vert(matIdx, p3Idx)
        
        add_tri(v1Idx, v2Idx, v3Idx)
except:
    print "Parse error on line", lineNum, "\n"
    traceback.print_exc()


#
# Write the shape file

o = open(shpFilename, "w")

# Header
o.write("Positions: ")
    
# Positions    
print >>o, len(positions)
for i in range(len(positions)):
    p = positions[i]
    o.write("\t%d: %5.2f\t%5.2f\t%5.2f\n" % (i, p[0], p[2], p[1]))

# Colours
print >>o, "Colours:", len(materials)
for i in range(len(materials)):
    mat = materials[i]
    print >>o, "\t%d: %d %d %d" % (i, mat[0], mat[1], mat[2])
    
# Vertices    
print >>o, "Vertices:", len(vertices)
for i in range(len(vertices)):
    v = vertices[i]
    matIdx, posIdx = v
    p = positions[posIdx]
    o.write("\t%d: %d %d\n" % (i, posIdx, matIdx))

# Triangles
print >>o, "Triangles:", len(tris), "# Position ID then Colour ID"
for t in tris:
    print >>o, "\t%d,%d,%d" % (t[2], t[1], t[0])
