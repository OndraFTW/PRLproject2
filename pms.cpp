/*
    Autor: Ondřej Šlampa, xslamp01@stud.fit.vutbr.cz
    Projekt: PRL proj1
    Popis: Implementace algoritmu pipeline merge sort pomocí OpenMPI.
*/

#include<mpi.h>
#include<iostream>
#include<fstream>
#include<vector>
#include<queue>
#include<cmath>

using namespace std;

//jméno vstupního souboru
const char* INPUT_FILE="numbers";
//značka konce proudu
const int EOS=-1;

/*
    Vytiskne vektor v.
    v vektor
*/
void print_vector(vector<int> v){
    if(!v.empty()){
        cout<<v[0];
        for(int i=1;i<v.size();i++){
            cout<<" "<<v[i];
        }
    }
    cout<<"\n";
}

/*
    Činnost prvního procesu (id je rovno 0).
    numprocs počet procesů
*/
void first(int numprocs){
    //vstupní soubor
    ifstream input_file;
    input_file.open(INPUT_FILE, ios::in|ios::binary);
    
    vector<int> v;
    int c;
    
    //načtení posloupnosti čísel
    while((c=input_file.get())!=EOF){
        v.push_back(c);
    }
    
    input_file.close();
    print_vector(v);
    cout.flush();
    
    //pokud existuje pouze jeden proces, tak má poslounost čísel délku 1
    if(numprocs==1){
        cout<<v[0]<<"\n";
    }
    //pokud existuje více procesů, pošlou se čísla následujícímu procesu
    else{
        for(int i=0;i<v.size();i++){
            MPI_Send(&v[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
        
        //odeslání značky konec proudu čísel
        c=EOS;
        MPI_Send(&c, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
}

/*
    Činnost jiného než prvního procesu (id je rovno 0).
    id identifikace tohoto procesu
    numprocs počet procesů
*/
void other(int id, int numprocs){
    int a;
    int size=pow(2,id-1);
    int i=0;
    int li=0;
    int ri=0;
    queue<int> left;
    queue<int> right;
    bool push_left=true;
    bool next=true;
    bool send=false;
    MPI_Status stat;
    bool last=id==numprocs-1;
    
    //pokud není splněna podmínka pro ukončení proveď následující krok
    //podmínka pro ukončení: nemůže být přijato další číslo a obě vstupní posloupnosti jsou prázdné
    while(!(!next && left.empty() && right.empty())){
        //pokud je možné přijmou další číslo, přijme se
        if(next){
            MPI_Recv(&a, 1, MPI_INT, id-1, 0, MPI_COMM_WORLD, &stat);
            //pokud došla značka konec proudu
            if(a==EOS){
                next=false;
                send=true;
            }
            //došlo číslo
            else{
                //pokud bylo přijato 2^(id-1) čísel v předchozích cyklech,
                //uloží se následující čísla do jiné vstupní posloupnosti
                //a začnou se odesílat čísla
                if(i==size){
                    i=0;
                    push_left=!push_left;
                    send=true;
                }
                i++;
                
                if(push_left){
                    left.push(a);
                }
                else{
                    right.push(a);
                }
            }
        }
        
        //pokud je možné odesílat čísla, odešle se jedno číslo
        if(send){
            //pokud se odeslalo 2^(id-1) čísel z obou vstupních posloupností
            //začnou se odesílat nové posloupnosti
            if(li==size && ri==size){
                li=ri=0;
            }
            
            //pokud je levá vstupní posloupnost prázdná, vybere se číslo z pravé
            if(left.empty()){
                a=right.front();
                right.pop();
                ri++;
            }
            //pokud je pravá vstupní posloupnost prázdná, vybere se číslo z levé
            else if(right.empty()){
                a=left.front();
                left.pop();
                li++;
            }
            //pokud z obou vstupních posloupností bylo vybráno méně než 2^(id-1)
            //čísel, vybere se větší číslo z jedné posloupnosti
            else if(li<size && ri<size){
                if(left.front()<=right.front()){
                    a=left.front();
                    left.pop();
                    li++;
                }
                else{
                    a=right.front();
                    right.pop();
                    ri++;
                }
            }
            //pokud z levé vstupní posloupnosti bylo vybráno méně než 2^(id-1)
            //čísel, vybere se z ní číslo
            else if(li<size){
                a=left.front();
                left.pop();
                li++;
            }
            //pokud z pravé vstupní posloupnosti bylo vybráno méně než 2^(id-1)
            //čísel, vybere se z ní číslo
            else if(ri<size){
                a=right.front();
                right.pop();
                ri++;
            }
            
            //pokud je toto poslední proces v řadě, vybrané číslo se vytiskne
            if(last){
                cout<<a<<"\n";
                cout.flush();
            }
            //jinak se pošle následujícímu procesu
            else{
                MPI_Send(&a, 1, MPI_INT, id+1, 0, MPI_COMM_WORLD);
            }
        }
    }
    
    //pokud toto není poslední proces odešle se značka konce proudu
    //následujícímu procesu
    if(!last){
        a=EOS;
        MPI_Send(&a, 1, MPI_INT, id+1, 0, MPI_COMM_WORLD);
    }
}

/*
    Funkce Main.
    argc počet argumentů
    argv argumenty
*/
int main(int argc, char *argv[]){
    int numprocs;
    int id;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    
    if(id==0){
        first(numprocs);
    }
    else{
        other(id, numprocs);
    }
    
    MPI_Finalize();
}
