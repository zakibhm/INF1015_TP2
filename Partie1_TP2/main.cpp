/*
*Ce programme devra afficher une liste de jeux avec les infomations de chaque jeu comme 
le titre du jeu, son année de sortie, le developpeur et la liste de concepteurs qui ont
concus le jeu a partir d'un fichier de données. Il devra ensuite detruire ces données de la memoire
* \file main.cpp
* \author Zekaria Bouchama    Sofiane Moualdi
* \date 02 octobre 2022
* Créé le 23 Septembre 2022
*/
#include "Jeu.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp"  //NOTE: Incompatible avec le "placement new", ne pas utiliser cette entête si vous utilisez ce type de "new" dans les lignes qui suivent cette inclusion.

using namespace std;
using namespace iter;
using namespace gsl;

#pragma region "Fonctions de base pour vous aider"
template <typename T>
T lireType(istream& fichier)
{
	T valeur{};
	fichier.read(reinterpret_cast<char*>(&valeur), sizeof(valeur));
	return valeur;
}
#define erreurFataleAssert(message) assert(false&&(message)),terminate()
static const uint8_t enteteTailleVariableDeBase = 0xA0;
size_t lireUintTailleVariable(istream& fichier)
{
	uint8_t entete = lireType<uint8_t>(fichier);
	switch (entete) {
	case enteteTailleVariableDeBase+0: return lireType<uint8_t>(fichier);
	case enteteTailleVariableDeBase+1: return lireType<uint16_t>(fichier);
	case enteteTailleVariableDeBase+2: return lireType<uint32_t>(fichier);
	default:
		erreurFataleAssert("Tentative de lire un entier de taille variable alors que le fichier contient autre chose à cet emplacement.");
	}
}

string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUintTailleVariable(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}
gsl::span<Jeu*> spanListeJeux(const ListeJeux& liste)
{
	return gsl::span(liste.elements, liste.nElements);
}
gsl::span<Concepteur*> spanListeConcepteurs(const ListeConcepteurs& liste)
{
	return gsl::span(liste.elements, liste.nElements);
}
#pragma endregion

//TODO: Fonction qui cherche un concepteur par son nom dans une ListeJeux.
// Cette fonction renvoie le pointeur vers le concepteur si elle le trouve dans
// un des jeux de la ListeJeux. En cas contraire, elle renvoie un pointeur nul.
Concepteur* trouver_un_concepteur(ListeJeux structure_de_Listejeux,string nom)
{
	for (int i = 0; i < structure_de_Listejeux.nElements; i++)
	{	
		for (int j = 0; j < structure_de_Listejeux.elements[i]->concepteurs.nElements; j++)
		{			
			if (nom == structure_de_Listejeux.elements[i]->concepteurs.elements[j]->nom)
				return structure_de_Listejeux.elements[i]->concepteurs.elements[j];
		}
	}
	return nullptr;
}


Concepteur* lireConcepteur(ListeJeux structure_de_Listejeux,istream& fichier)
{
	Concepteur concepteur = {}; // On initialise une structure vide de type Concepteur.
	concepteur.nom = lireString(fichier);
	concepteur.anneeNaissance = int(lireUintTailleVariable(fichier));
	concepteur.pays = lireString(fichier);
	// Rendu ici, les champs précédents de la structure concepteur sont remplis
	// avec la bonne information.
	
	//TODO: Ajouter en mémoire le concepteur lu. Il faut revoyer le pointeur créé.
	// Attention, valider si le concepteur existe déjà avant de le créer, sinon
	// on va avoir des doublons car plusieurs jeux ont des concepteurs en commun
	// dans le fichier binaire. Pour ce faire, cette fonction aura besoin de
	// la liste de jeux principale en paramètre.
	if (trouver_un_concepteur(structure_de_Listejeux, concepteur.nom) == nullptr) {
		Concepteur* Concepteur_pointeur = new Concepteur(concepteur);
		// Afficher un message lorsque l'allocation du concepteur est réussie.
		cout << "l'allocation du concepteur est réussie." << endl;
		return Concepteur_pointeur; //TODO: Retourner le pointeur vers le concepteur crée.
	}

	return trouver_un_concepteur(structure_de_Listejeux, concepteur.nom); //TODO: Retourner le pointeur vers le concepteur crée.
}

//TODO: Fonction qui change la taille du tableau de jeux de ListeJeux.
// Cette fonction doit recevoir en paramètre la nouvelle capacité du nouveau
// tableau. Il faut allouer un nouveau tableau assez grand, copier ce qu'il y
// avait dans l'ancien, et éliminer l'ancien trop petit. N'oubliez pas, on copie
// des pointeurs de jeux. Il n'y a donc aucune nouvelle allocation de jeu ici !
void changer_la_taille_du_tableau(ListeJeux& structure_de_Listejeux,size_t nouvelle_capacite)
{
	Jeu** nouveaux_elements = new Jeu * [nouvelle_capacite];
	for (int i = 0; i < structure_de_Listejeux.nElements; i++)
	{		
		nouveaux_elements[i] = structure_de_Listejeux.elements[i];
	}
	structure_de_Listejeux.capacite = nouvelle_capacite;
	delete[] structure_de_Listejeux.elements;
	structure_de_Listejeux.elements = nouveaux_elements;
}

//TODO: Fonction pour ajouter un Jeu à ListeJeux.
// Le jeu existant déjà en mémoire, on veut uniquement ajouter le pointeur vers
// le jeu existant. De plus, en cas de saturation du tableau elements, cette
// fonction doit doubler la taille du tableau elements de ListeJeux.
// Utilisez la fonction pour changer la taille du tableau écrite plus haut.
void ajouter_jeu(ListeJeux& structure_de_Listejeux,Jeu& jeu)
{
	if (structure_de_Listejeux.capacite == 0)
	{
		changer_la_taille_du_tableau(structure_de_Listejeux, 1);
	}
	if (structure_de_Listejeux.nElements >= structure_de_Listejeux.capacite)
	{
		changer_la_taille_du_tableau(structure_de_Listejeux, structure_de_Listejeux.capacite * 2);
	}

	structure_de_Listejeux.elements[structure_de_Listejeux.nElements] = &jeu;
	structure_de_Listejeux.nElements++;
}

//TODO: Fonction qui enlève un jeu de ListeJeux.
// Attention, ici il n'a pas de désallocation de mémoire. Elle enlève le
// pointeur de la ListeJeux, mais le jeu pointé existe encore en mémoire.
// Puisque l'ordre de la ListeJeux n'a pas être conservé, on peut remplacer le
// jeu à être retiré par celui présent en fin de liste et décrémenter la taille
// de celle-ci.
void enlever_jeu(ListeJeux& structure_de_Listejeux,Jeu& jeu)
{
	for(int i=0;i< structure_de_Listejeux.nElements;i++)
	{ 		
		if (structure_de_Listejeux.elements[i]->titre == jeu.titre)
			structure_de_Listejeux.elements[i] = structure_de_Listejeux.elements[structure_de_Listejeux.nElements-1];
			structure_de_Listejeux.nElements--;
	}
	
}

Jeu* lireJeu(ListeJeux& structure_de_Listejeux,istream& fichier)
{
	Jeu jeu = {}; // On initialise une structure vide de type Jeu
	jeu.titre = lireString(fichier);
	jeu.anneeSortie = int(lireUintTailleVariable(fichier));
	jeu.developpeur = lireString(fichier);
	jeu.concepteurs.nElements = lireUintTailleVariable(fichier);
	// Rendu ici, les champs précédents de la structure jeu sont remplis avec la
	// bonne information.
	
	//TODO: Ajouter en mémoire le jeu lu. Il faut revoyer le pointeur créé.
	Jeu* pointeur_jeu = new Jeu(jeu);

	// Attention, il faut aussi créer un tableau dynamique pour les concepteurs
	// que contient un jeu. Servez-vous de votre fonction d'ajout de jeu car la
	// liste de jeux participé est une ListeJeu. 
	pointeur_jeu->concepteurs.elements = new Concepteur*[jeu.concepteurs.nElements];
	pointeur_jeu->concepteurs.capacite = jeu.concepteurs.nElements;
	pointeur_jeu->concepteurs.nElements = 0;

	// Afficher un message lorsque
	// l'allocation du jeu est réussie.
	cout << "\n"<<"l'allocation du jeu est réussie et son nom est : "<< pointeur_jeu->titre << endl;

	for ([[maybe_unused]] size_t i : iter::range(jeu.concepteurs.nElements)) {
		//TODO: Mettre le concepteur dans la liste des concepteur du jeu.
		pointeur_jeu->concepteurs.elements[i] = lireConcepteur(structure_de_Listejeux, fichier);
		//cout <<"\n"<<  pointeur_jeu->concepteurs.elements[i]->nom << ", " << pointeur_jeu->concepteurs.elements[i]->anneeNaissance << endl;

		//TODO: Ajouter le jeu à la liste des jeux auquel a participé le concepteur.
		pointeur_jeu->concepteurs.nElements++;
		ajouter_jeu(pointeur_jeu->concepteurs.elements[i]->jeuxConcus, *pointeur_jeu);
		//cout << "\t" <<"liste des jeux de ce concepteur est: avec une taille de : " << pointeur_jeu->concepteurs.elements[i]->jeuxConcus.nElements <<endl;
		/*for (int j = 0; j < pointeur_jeu->concepteurs.elements[i]->jeuxConcus.nElements; j++) {
			cout << "\t" << pointeur_jeu->concepteurs.elements[i]->jeuxConcus.elements[j]->titre << endl;
		}*/	
	}
	return pointeur_jeu; //TODO: Retourner le pointeur vers le nouveau jeu.
}

ListeJeux creerListeJeux(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	size_t nElements = lireUintTailleVariable(fichier);
	ListeJeux listeJeux = {};
	listeJeux.nElements = 0;
	listeJeux.capacite = nElements;
	listeJeux.elements = new Jeu * [nElements];
	for([[maybe_unused]] size_t n : iter::range(nElements))
	{
		listeJeux.elements[n] = lireJeu(listeJeux, fichier); //TODO: Ajouter le jeu à la ListeJeux.
		listeJeux.nElements++;
	}
	return listeJeux; //TODO: Renvoyer la ListeJeux.
}

//TODO: Fonction pour détruire un concepteur (libération de mémoire allouée).
// Lorsqu'on détruit un concepteur, on affiche son nom pour fins de débogage.
void detruire_concepteur(Concepteur* concepteur)
{
	string nom = concepteur->nom;				
	delete[] concepteur->jeuxConcus.elements;
	delete concepteur;
	cout << "le nom du concepteur détruit est: " << nom << endl;
				
}

//TODO: Fonction qui détermine si un concepteur participe encore à un jeu.
bool verifier_concepteur_existe(Concepteur concepteur)
{
	if (concepteur.jeuxConcus.nElements == 0)
		return false;
	else
		return true;		
}


//TODO: Fonction pour détruire un jeu (libération de mémoire allouée).
// Attention, ici il faut relâcher toute les cases mémoires occupées par un jeu.
// Par conséquent, il va falloir gérer le cas des concepteurs (un jeu contenant
// une ListeConcepteurs). On doit commencer par enlever le jeu à détruire des jeux
// qu'un concepteur a participé (jeuxConcus). Si le concepteur n'a plus de
// jeux présents dans sa liste de jeux participés, il faut le supprimer.  Pour
// fins de débogage, affichez le nom du jeu lors de sa destruction.
void detruire_un_jeu(Jeu* jeu)
{
	for (int i = 0; i < jeu->concepteurs.nElements; i++)
	{
		for (int j = 0; j < jeu->concepteurs.elements[i]->jeuxConcus.nElements; j++) {
			if (jeu->concepteurs.elements[i]->jeuxConcus.elements[j]->titre == jeu->titre) {
				enlever_jeu(jeu->concepteurs.elements[i]->jeuxConcus, *jeu);
				break;
			}	
		}
		if (!verifier_concepteur_existe(*jeu->concepteurs.elements[i]))
			detruire_concepteur(jeu->concepteurs.elements[i]);
		jeu->concepteurs.elements[i] = jeu->concepteurs.elements[jeu->concepteurs.nElements - 1];
	}
	delete[] jeu->concepteurs.elements;
	delete jeu;
}

//TODO: Fonction pour détruire une ListeJeux et tous ses jeux.b
void detruire_listeJeux(ListeJeux& structure_de_Listejeux)
{
	for (int i = 0; i < structure_de_Listejeux.nElements; i++)
	{
		detruire_un_jeu(structure_de_Listejeux.elements[i]);
	}
	delete[] structure_de_Listejeux.elements;
}

void afficherConcepteur(const Concepteur& d)
{
	cout << "\t" << d.nom << ", " << d.anneeNaissance << ", " << d.pays
		<< endl;
}

//TODO: Fonction pour afficher les infos d'un jeu ainsi que ses concepteurs.
// Servez-vous de la fonction afficherConcepteur ci-dessus.
void afficherJeu(const Jeu& jeu, int nombre)
{
	cout << "Jeu " << nombre + 1 << endl;
	cout << "\t" << "titre: " << jeu.titre << ", annee de sortie: " << jeu.anneeSortie << ", developpeur: " << jeu.developpeur
		<< endl;
	cout << "la liste des concepteurs :" << endl;
	for (int i = 0; i < jeu.concepteurs.nElements; i++)
	{
		afficherConcepteur(*jeu.concepteurs.elements[i]);
	}
}

//TODO: Fonction pour afficher tous les jeux de ListeJeux, séparés par un ligne.
// Servez-vous de la fonction d'affichage d'un jeu crée ci-dessus. Votre ligne
// de séparation doit être différent de celle utilisée dans le main.
void afficherlistejeux(const ListeJeux& structure_de_Listejeux)
{
	static const string ligneDeSeparation = "------------------------------------------------------------------------------------";

	for (int i = 0; i < structure_de_Listejeux.nElements; i++)
	{
		cout << "\n" << ligneDeSeparation << endl;
		afficherJeu(*structure_de_Listejeux.elements[i], i);
	}
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	#pragma region "Bibliothèque du cours"
	// Permet sous Windows les "ANSI escape code" pour changer de couleur
	// https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac
	// les supportent normalement par défaut.
	bibliotheque_cours::activerCouleursAnsi(); 
	#pragma endregion

	int* fuite = new int;  // Pour vérifier que la détection de fuites fonctionne; un message devrait dire qu'il y a une fuite à cette ligne.

	ListeJeux listeJeux=creerListeJeux("jeux.bin"); //TODO: Appeler correctement votre fonction de création de la liste de jeux.

	static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
	cout << ligneSeparation << endl;
	cout << "Premier jeu de la liste :" << endl;
	//TODO: Afficher le premier jeu de la liste (en utilisant la fonction).  Devrait être Chrono Trigger.
	afficherJeu(*listeJeux.elements[0],0);
	cout << ligneSeparation << endl;

	//TODO: Appel à votre fonction d'affichage de votre liste de jeux.
	afficherlistejeux(listeJeux);
	//TODO: Faire les appels à toutes vos fonctions/méthodes pour voir qu'elles fonctionnent et avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.

	//TODO: Détruire tout avant de terminer le programme.  Devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	detruire_listeJeux(listeJeux);
}
