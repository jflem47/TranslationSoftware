/**
 * \file Dictionnaire.h
 * \brief Ce fichier contient l'interface d'un dictionnaire.
 * \author IFT-2008, Jean-Francois Lemieux
 * \version 0.1
 * \date juillet 2020
 *
 */


#ifndef DICO_H_
#define DICO_H_

#include <iostream>
#include <fstream> // pour les fichiers
#include <string>
#include <utility>
#include <vector>
#include <queue>

namespace TP3
{

//classe représentant un dictionnaire des synonymes
class Dictionnaire
{
public:

	//Constructeur
	Dictionnaire();

	//Constructeur de dictionnaire à partir d'un fichier
	//Le fichier doit être ouvert au préalable
	Dictionnaire(std::ifstream &fichier);

	//Destructeur.
	~Dictionnaire();

	//Ajouter un mot au dictionnaire et l'une de ses traductions en équilibrant l'arbre AVL
	void ajouteMot(const std ::string& motOriginal, const std ::string& motTraduit);

	//Supprimer un mot et équilibrer l'arbre AVL
	//Si le mot appartient au dictionnaire, on l'enlève et on équilibre. Sinon, on ne fait rien.
	//Exception	logic_error si l'arbre est vide
	//Exception	logic_error si le mot n'appartient pas au dictionnaire
	void supprimeMot(const std ::string& motOriginal);

	//Quantifier la similitude entre 2 mots (dans le dictionnaire ou pas)
	//Ici, 1 représente le fait que les 2 mots sont identiques, 0 représente le fait que les 2 mots sont complètements différents
	//On retourne une valeur entre 0 et 1 quantifiant la similarité entre les 2 mots donnés
	//Vous pouvez utiliser par exemple la distance de Levenshtein, mais ce n'est pas obligatoire !
	double similitude(const std ::string& mot1, const std ::string& mot2);


	//Suggère des corrections pour le mot motMalEcrit sous forme d'une liste de mots, dans un vector, à partir du dictionnaire
	//S'il y a suffisament de mots, on redonne 5 corrections possibles au mot donné. Sinon, on en donne le plus possible
	//Exception	logic_error si le dictionnaire est vide
	std::vector<std::string> suggereCorrections(const std ::string& motMalEcrit);

	//Trouver les traductions possibles d'un mot
	//Si le mot appartient au dictionnaire, on retourne le vecteur des traductions du mot donné.
	//Sinon, on retourne un vecteur vide
	std::vector<std::string> traduit(const std ::string& mot);

	//Vérifier si le mot donné appartient au dictionnaire
	//On retourne true si le mot est dans le dictionnaire. Sinon, on retourne false.
	bool appartient(const std::string &data);

	//Vérifier si le dictionnaire est vide
	bool estVide() const;

	//Affiche à l'écran l'arbre niveau par niveau de façon à voir si l'arbre est bien balancé.
	//Ne touchez pas s.v.p. à cette méthode !
    friend std::ostream& operator<<(std::ostream& out, const Dictionnaire& d)
    {
  	  if (d.racine != 0)
  	  {
  		  std::queue<NoeudDictionnaire*> file;
  		  std::queue<std::string> fileNiveau;

  		  NoeudDictionnaire * noeudDicotemp;
  		  std::string niveauTemp;

  		  int hg = 0, hd = 0;

  		  file.push(d.racine);
  		  fileNiveau.push("1");
  		  while (!file.empty())
  		  {
  			  noeudDicotemp = file.front();
  			  niveauTemp = fileNiveau.front();
  			  out << noeudDicotemp->mot;
  			  if (noeudDicotemp->gauche == 0) hg = -1; else hg = noeudDicotemp->gauche->hauteur;
  			  if (noeudDicotemp->droite == 0) hd = -1; else hd = noeudDicotemp->droite->hauteur;
  			  out << ", " << hg - hd;
  			  out << ", " << niveauTemp;
  			  out << std::endl;
  			  file.pop();
  			  fileNiveau.pop();
  			  if (noeudDicotemp->gauche != 0)
  			  {
  				  file.push(noeudDicotemp->gauche);
  				  fileNiveau.push(niveauTemp + ".1");
  			  }
  			  if (noeudDicotemp->droite != 0)
  			  {
  				  file.push(noeudDicotemp->droite);
  				  fileNiveau.push(niveauTemp + ".2");
  			  }
  		  }
  	  }
  	  return out;
    }


private:

	// Classe interne représentant un noeud dans l'arbre AVL constituant le dictionnaire de traduction.
	class NoeudDictionnaire
	{
	public:

		std::string mot;						// Un mot (en anglais)

		std::vector<std::string> traductions;	// Les différentes traductions possibles en français du mot en anglais
												// Par exemple, la liste française { "contempler", "envisager" et "prévoir" }
												// pourrait servir de traduction du mot anglais "contemplate".

	    NoeudDictionnaire *gauche, *droite;		// Les enfants du noeud

	    int hauteur;							// La hauteur de ce noeud (afin de maintenir l'équilibre de l'arbre AVL)

		// Vous pouvez ajouter ici un constructeur de NoeudDictionnaire
		NoeudDictionnaire(const std::string &mot, const std::string &traduction)
		: mot(mot), gauche(), droite(), hauteur(0) {
		    traductions.push_back(traduction);
		}
	};
    
	NoeudDictionnaire * racine;		// La racine de l'arbre des mots
    
    int cpt;						// Le nombre de mots dans le dictionnaire

	//Vous pouvez ajouter autant de méthodes privées que vous voulez

	// Insertion d'un noeud dans l'arbre
	void _auxInsererAVL(NoeudDictionnaire *&arbre, const std::string &mot, const std::string &traduction);

	// Balancement de l'arbre
	void _balancer(NoeudDictionnaire *&arbre);

	// Avoir la hauteur
	int _hauteur(NoeudDictionnaire *arb) const;

	// Vérifier si débalancement à gauche
	bool _debalancementGauche(NoeudDictionnaire *arbre) const;

	// Vérifier si débalancement à droite
	bool _debalancementDroite(NoeudDictionnaire *arbre) const;

	// Vérifier si le sous arbre penche à droite
	bool _sousArbrePencheDroite(NoeudDictionnaire *arbre) const;

	// Vérifier si le sous arbre penche à gauche
	bool _sousArbrePencheGauche(NoeudDictionnaire *arbre) const;

	// ZigZig Gauche
	void _zigZigGauche(NoeudDictionnaire *&K2);

	// ZigZig Droite
	void _zigZigDroite(NoeudDictionnaire *&K2);

	// ZigZag Gauche
	void _zigZagGauche(NoeudDictionnaire *&K3);

	// ZigZag Droite
	void _zigZagDroite(NoeudDictionnaire *&K3);

	// Retirer un mot de l'arbre
	void _auxEnlever(NoeudDictionnaire *&arbre, const std::string &mot);

	// Enlever le successeur de droite si nécessaire
	void _enleverSuccMinDroite(NoeudDictionnaire *arbre);

	// Vérifier l'appartenance du mot
	NoeudDictionnaire* _auxAppartient(NoeudDictionnaire *arbre, const std::string &mot) const;

};
    
}

#endif /* DICO_H_ */
