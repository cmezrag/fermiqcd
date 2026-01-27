#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <random>




double GaussianWeight(double x){

    
    return std::exp(-x*x);

}

// this function return a random number between a and b, but we use the old routine random() that is not thread safe
double oldRandomGenerator(double a, double b){

    double range = pow(2,31);

    return a + (b-a) * random() / range ;

}

double modernRandomGenerator(double a, double b){

     std::mt19937 engine{ std::random_device{}() }; // Here we set the seed

    // Wanted range : [0, 2^31‑1]
    const int64_t MIN_VAL = 0;
    const int64_t MAX_VAL = (static_cast<int64_t>(1) << 31) - 1;   // 2147483647

    std::uniform_int_distribution<int64_t> dist(MIN_VAL, MAX_VAL);

    return a +(b-a) * dist(engine) / MAX_VAL ; 

}


void Exercice1(){

    const int nbofhit = 100000 ;
    double x, sum, mean = 0. ; 

     std::ofstream optfileold("randomold.dat", std::ios::out);
     std::ofstream optfilenew("randomnew.dat", std::ios::out);
    

    for(int i=0 ; i< nbofhit;i++){

        x = oldRandomGenerator(0.,1.) ;
        optfileold << i << "  " << x << std::endl ;
        sum += x ;
        mean = sum / i ;
    }
    optfileold.close() ;

    //newmethod

    std::mt19937 engine{ std::random_device{}() };

    std::uniform_real_distribution<double> dist(0.0, 1.0);   // [0,1)

    /*--------------------------------------------------------------
      3️⃣ Génération et affichage
    --------------------------------------------------------------*/
    for (int i = 0; i < nbofhit ; ++i) {
        double r = dist(engine);          // r ∈ [0,1)
        optfilenew << i << " " << r << std::endl ;
    }

    optfilenew.close() ;

}

double ProbaDist(std::vector<double>* vec){
    
    if (!vec) {                     // protection contre nullptr
        std::cerr << "Pointeur nul reçu.\n";
        return 0.0;
    }
   
    if(vec->size() == 0){    
        throw("Vector is empty, impossible to compute ProbaDist");
    }

    const double* p   = vec->data();          // pointeur sur le premier élément
    const double* fin = p + vec->size();      // pointeur juste après le dernier

    double somme = 0.0;
    for (; p != fin; ++p)                    // itération « pointer‑style »
        somme += (*p) * (*p);                 // ajoute le carré

    return std::exp(-somme);  

}

double FunctionExercice2(std::vector<double>* vec){

        
    if (!vec) {                     // protection contre nullptr
        std::cerr << "Pointeur nul reçu.\n";
        return 0.0;
    }
   
    if(vec->size() == 0){    
        throw("Vector is empty, impossible to compute ProbaDist");
    }

    const double* p   = vec->data();          // pointeur sur le premier élément
    const double* fin = p + vec->size();      // pointeur juste après le dernier

    double somme = 0.0;
    for (; p != fin; ++p)                    // itération « pointer‑style »
        somme += (*p) * (*p);                 // ajoute le carré

    return somme ;  

}

void Exercice2(){

    std::ofstream optfile1("CalculND.dat", std::ios::out);
    std::ofstream optfile2("sample.dat", std::ios::out);
    std::ofstream optfile3("walk_2D.dat", std::ios::out);

    std::mt19937 engine{ std::random_device{}() };

    double epsi = 1.;
    std::uniform_real_distribution<double> randomstep(-epsi, epsi);   // (-1,1)
    std::uniform_real_distribution<double> randomweight(0., 1.);   // (0,1)


    int i, j, dim = 2, nmarkov = 100000 ;

    std::vector<double> x, y;
    std::vector<double> store_function;
    double accept = 0.;

    //initialisation procedure
    for(j;j<dim;j++){
        x.push_back(0.);
        y.push_back(0.);
    }

    std::cout << "initialisation of vectors done" << std::endl ;

    for(i=0; i<nmarkov; i++){

        for(j=0; j< dim; j++){

            y.at(j) = x.at(j) + randomstep(engine) ;

        }


        if(ProbaDist(&y)/ProbaDist(&x) > randomweight(engine)){

            x = y ;
            accept = accept +1. ;

        }


        store_function.push_back( FunctionExercice2(&x) );    

        if(dim==1) // write 1d hain for histogram
            {
                optfile2<<i<<" "<<x.at(0)<< std::endl;
            }
        if(dim==2) // write 2d chain
            {
                optfile3<<i << " " << x.at(0) <<" " << x.at(1) << std::endl;
            }

    }

    std::cout << "First part done" << std::endl ;

    double sum = 0., mean = 0., sum2 =0., mean2=0., sigma=0.; 
    double exact = dim/2 ;

    for(i=0;i<nmarkov;i++){
        sum=sum+store_function.at(i);
        mean=sum/i;
        sum2=sum2+store_function.at(i) * store_function.at(i);
        mean2=sum2/i;
        sigma=std::sqrt((mean2-mean*mean)/i);
        if(i%1000==0)
        {
            optfile1<<i<<" "<<mean<<" "<<sigma<< std::endl;
        }
    }



std::cout<<"calcul en dimension: "<<dim<< std::endl;
std::cout<<" accept rate: "<<accept/nmarkov<< std::endl;
std::cout<<"mean: "<<mean<<" std error: "<<sigma<<" exact: "<<exact<< std::endl;
//
optfile1.close(); 
optfile2.close(); 
optfile3.close(); 

}

int main(){

    //Excerice1();

    Exercice2();

    return 0;
}