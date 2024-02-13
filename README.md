# Dependencies
- GetPot for arg parsing


# Usage 

1. Create a exemple.geo file (with Gmsh)
2. Generate a .msh file in msh2 format using
`gmsh -2 -format msh2 -saveall exemple.geo -o exemple.msh`
3. Use the converter to convert it into the format descibed above using:
`./mshtofrac.exe -m exemple.msh -f1 id1 id2 id3 -f2 id4 id5 id6 ...`

The idx are the Physical Line id's in Gmsh during the creation of the mesh.


The -f1 identifies fractures of length df1, the -f2 identifies fractures of length df2, 
they can be omitted if no fractures.
