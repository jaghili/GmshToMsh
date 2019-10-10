#include <iostream>
#include <fstream>
#include <vector>
#include "GetPot"
#include <algorithm>

// macros
typedef double Real;

struct Node {
  uint id;
  Real x,y;
};

struct FracFace { uint n1, n2; };
struct Triangle { uint n1, n2, n3; };

int main(int argc, char* argv[]) {

  double dummy; // dummy variable to store temp values

  // get arguments
  GetPot options(argc, argv);

  std::string mesh_file;  
  if(options.search(2, "--mesh", "-m")) { mesh_file = options.next(""); }

  // convert vector strings to uint
  std::vector<uint> frac1_list;
  std::vector<uint> frac2_list;
  {
    const std::vector<std::string> sfrac1_list = options.nominus_followers(2, "--frac1", "-f1");
    const std::vector<std::string> sfrac2_list = options.nominus_followers(2, "--frac2", "-f2");
    for (auto & fs : sfrac1_list) { frac1_list.push_back((uint)stoi(fs)); }
    for (auto & fs : sfrac2_list) { frac2_list.push_back((uint)stoi(fs)); }
  }

  // debug
  //std::cout << "Reading " <<  mesh_file << std::endl;
  //std::cout << "Frac1 : " << frac1_list.size() << std::endl;
  //std::cout << "Frac2 : " << frac2_list.size() << std::endl;
  //for (auto & fs : frac1_list) { std::cout << fs << " "; }
  //std::cout << std::endl;  
  //for (auto & fs : frac2_list) { std::cout << fs << " "; }
  //std::cout << std::endl;
  
  // Open file 
  std::fstream datafile(mesh_file, std::ios::in);
  std::string marker;


  // Parse Mesh Format block
  datafile >> marker;
  if (marker.compare("$MeshFormat") == 0) { // match 
    float version;
    datafile >> version;
    //std::cout << "GMSH version "<< version << std::endl;

    datafile >> dummy;
    //std::cout << "GMSH options "<< dummy << std::endl;
    datafile >> dummy;
    //std::cout << "GMSH options "<< dummy << std::endl;
  }
  
  datafile >> marker;
  if (marker.compare("$EndMeshFormat") != 0) {
    std::cout << "[ERROR] $EndMeshFormat is missing ! terminating..." << std::endl;
    return EXIT_FAILURE;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Parse Nodes
  std::vector<Node> Nodes;
  datafile >> marker;
  if (marker.compare("$Nodes") != 0) {
    std::cout << "$Nodes is missing" << std::endl;
    return EXIT_FAILURE;
  }
  uint NumberOfNodes = 0;
  datafile >> NumberOfNodes;
  
  // Allocate
  std::cout << std::endl << NumberOfNodes << std::endl;
  Nodes.resize(NumberOfNodes);
  for (uint iN = 1; iN <= NumberOfNodes; iN++) {
    Node n;

    datafile >> n.id;
    datafile >> n.x;
    datafile >> n.y;
    datafile >> dummy;
    
    // print  nodes
    std::cout << n.x << " " << n.y << std::endl;
    Nodes.push_back(n);
  }
  std::cout << std::endl;
  
  datafile >> marker;  
  if (marker.compare("$EndNodes") != 0) {
    std::cout << marker << std::endl;
    std::cout << "List of Nodes must end with $EndNodes... terminating" <<  std::endl;
    return EXIT_FAILURE;
  }
  
  ////////////////////////////////////////////////////////////////////////////
  // Parse elements (points, lines, elements...)
  std::vector<Triangle> Triangles;
  std::vector<FracFace> FracFaces1;  // 
  std::vector<FracFace> FracFaces2;  //
  
  datafile >> marker;
  if (marker.compare("$Elements") == 0) {
    uint NumberOfElements = 0;
    datafile >> NumberOfElements;
    uint gid = 0;
    uint itype = 0;
    uint iparams = 0;
    uint ifacetype = 0;
    uint n1, n2, n3;
    std::vector<uint>::iterator iter;
    
    for (uint iE = 1; iE <= NumberOfElements; iE++) {
      datafile >> gid;   // id of the element
      datafile >> itype;   // type of the element
      datafile >> iparams; // number of parameters
      datafile >> dummy;   // 1st param

      // type of the point
      switch(itype) {
	
      case 15 :            // element type
	datafile >> dummy; // type of the point
	datafile >> dummy; // id of the point
	break;
	
      case 1 :             // line type
	datafile >> ifacetype;
	datafile >> n1 >> n2;

	// If fracture is of first type 
	iter = std::find(frac1_list.begin(), frac1_list.end(), ifacetype);
	if (iter != frac1_list.end()) { // ifacetype IS a fracture
	  FracFace ff;
	  ff.n1 = n1;
	  ff.n2 = n2;
	  FracFaces1.push_back(ff);	  
	  //	  std::cout << "push " << gid << " ff1 Frac " << n1 << " " << n2 << std::endl;
	}

	// If fracture is of second type 
	iter = std::find(frac2_list.begin(), frac2_list.end(), ifacetype);
	if (iter != frac2_list.end()) { // ifacetype IS a fracture
	  FracFace ff;
	  ff.n1 = n1;
	  ff.n2 = n2;
	  FracFaces2.push_back(ff);	  
	  //	  std::cout << "push " << gid << " ff2 Frac " << n1 << " " << n2 << std::endl;
	}
	break;
	

      case 2 :
	datafile >> dummy; // element type
	datafile >> n1 >> n2 >> n3;
	{
	  Triangle tt;
	  tt.n1 = n1; tt.n2 = n2; tt.n3 = n3;
	  Triangles.push_back(tt);
	}
	break;
	
      } // end case
      
    } // end for 
    
  }  
  datafile.close();


  // Print triangles
  std::cout << Triangles.size() << std::endl;
  for (auto & T : Triangles) {
    std::cout << "3 " << T.n1 << " " << T.n2 << " " << T.n3 << std::endl;
  }
  std::cout << std::endl;

  // Print frac faces
  std::cout << FracFaces1.size() + FracFaces2.size() << std::endl;
  for (auto & FF : FracFaces1) { std::cout << "1 " << FF.n1 << " " << FF.n2 << std::endl;  }
  for (auto & FF : FracFaces2) { std::cout << "2 " << FF.n1 << " " << FF.n2 << std::endl;  }
  
  return 0;
}

