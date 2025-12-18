#include "fermiqcd.h"              

int main(int argc, char** argv) {
  mdp.open_wormholes(argc,argv);    // open communications
  if(argc<2) {
    mdp << "usage:\n";    
    mdp << "a) to create a hot gauge confguration:\n";
    mdp << "   a.out -hot -nc=3 -L=12x8x8x8 -output=name\n\n";
    mdp << "b) to create a cold gauge confguration:\n";
    mdp << "   a.out -cold -nc=3 -L=12x8x8x8 -output=name\n\n";
    mdp << "c) to perform 1 heatbath step at beta=6.0:\n";
    mdp << "   a.out -input=name -nc=3 -beta=6.0 -steps=1 -L=12x8x8x8 -output=name\n\n";
    mdp << "d) to compute the average plaquette:\n";
    mdp << "   a.out -input=name -nc=3 -L=12x8x8x8\n\n";
    mdp << "Other optional flags:\n";
    mdp << "  -verbose  (output all steps of the inverter)\n\n";
    mdp << "works for any nc and up to 10 dimensions.\n";
    exit(1);
  }
  int verbose=false;
  int ndim=4,nc=3;
  int L[10]={4,4,4,4,4,4,4,4,4,4};
  mdp_real beta=6.0;
  int steps=0;
  char input[1024]="";
  char output[1024]="";
  int mode=0; 

  // //////////////////////////////
  // Parsing command line arguments
  // //////////////////////////////
  for(int i=1; i<argc; i++) {
    if(strncmp(argv[i],"-verbose",8)==0)     verbose=true;
    else if(strncmp(argv[i],"-hot",4)==0)    mode=1;
    else if(strncmp(argv[i],"-cold",5)==0)   mode=2;
    else if(strncmp(argv[i],"-input",6)==0)  sscanf(argv[i],"-input=%s",input);  
    else if(strncmp(argv[i],"-output",7)==0) sscanf(argv[i], "-output=%s",output);
    else if(strncmp(argv[i],"-nc",3)==0)     sscanf(argv[i],"-nc=%i",&nc);
    else if(strncmp(argv[i],"-steps",6)==0)  sscanf(argv[i],"-steps=%i",&steps);
    else if(strncmp(argv[i],"-beta",5)==0)   sscanf(argv[i],"-beta=%f",&beta);    
    else if(strncmp(argv[i],"-L",2)==0)      ndim=sscanf(argv[i],"-L=%ix%ix%ix%ix%ix%ix%ix%ix%ix%i",L,L+1,L+2,L+3,L+4,L+5,L+6,L+7,L+8,L+9);
    else error("Wrong command line option");
  }
  
  if(mode==0) {
    mdp_field_file_header header;
    if(is_file(input)) header=get_info(input);
    else error("Unable to access input gauge configuration\n");
    ndim=header.ndim;
    nc=(int) sqrt((double) header.bytes_per_site/(ndim*sizeof(mdp_complex)));
    for(int i=0; i<ndim; i++) L[i]=header.box[i];
  }

  // //////////////////////////////
  // Output parameters
  // //////////////////////////////
  mdp << "=============================================\n";
  mdp << "Number of colors = " << nc << '\n';
  mdp << "Lattice size = " << L[0]; 
  for(int i=1; i<ndim; i++) mdp << "x" << L[i]; mdp << '\n';
  switch(mode) {
  case 0: mdp << "Reading input file = " << input << '\n'; break;
  case 1: mdp << "Creating a hot configuration\n"; break;
  case 2: mdp << "Creating a cold configuration\n"; break;
  }
  mdp << "Performing " << steps << " heatbath steps at beta = " << beta << '\n';
  mdp << "Saving output in file = " << output << '\n';
  mdp << "=============================================\n";

  // //////////////////////////////
  // Computation
  // //////////////////////////////
  
  if(!verbose)  mdp.print=false;  // eventualy print off
  mdp_lattice   lattice(ndim,L); // declare lattice
  mdp_site      x(lattice);      // declare site variable
  gauge_field	U(lattice,nc);   // declare SU(3) field
  coefficients	gauge;		 // declare coefficients
    
  gauge["beta"]=beta;		 // set beta
  switch(mode) {                 // set initial gauge   
  case 0:U.load(input); break;
  case 1:set_hot(U); break;       
  case 2:set_cold(U); break;
  }


  if(steps>0) WilsonGaugeAction::heatbath(U,gauge,steps); //heatbath 
  if(mdp.me()==0) mdp.print=true;             // print on
  mdp << "Average plaquette = " << average_plaquette(U) << '\n'; // print plaquette
  if(string(output)!="") U.save(output);      // save file
  mdp.close_wormholes();
  return 0;
}

