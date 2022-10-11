#pragma once
#include <string>
#include "ListeJeux.hpp"
using namespace std;

class Developpeur
{
    //TODO: En faire une classe qui suit les principes OO.
    //TODO: La destruction d'un Developpeur doit s'assurer que la désallocation de ListeJeux est faite.
    //TODO: Les méthodes à faire...

public:
    Developpeur(string nomDev);
    ~Developpeur();

    string getnom();
    int compterNombrede_Jeux(ListeJeux& listeJeux);
    void mise_a_jourListe(ListeJeux& listeJeux);
    void print_ListeJeux();

private:
    pair<string, ListeJeux> paireNomJeux_;
};