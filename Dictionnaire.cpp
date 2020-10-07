/**
 * \file Dictionnaire.cpp
 * \brief Ce fichier contient une implantation des méthodes de la classe Dictionnaire
 * \author IFT-2008, Jean-Francois Lemieux
 * \version 0.1
 * \date juillet 2020
 *
 */

#include "Dictionnaire.h"

// Limite du nombre de suggestions
#define LIMITE_SUGGESTIONS 5
#define RATIO_MINIMUM 0.75

namespace TP3 {

    /**
     * \fn Dictionnaire::Dictionnaire(std::ifstream &fichier)
     * \brief Constructeur de dictionnaire à partir d'un fichier
     * \pre Le fichier doit être ouvert au préalable
     * \param[in] fichier Le fichier à partir duquel on construit le dictionnaire
     */
    Dictionnaire::Dictionnaire(std::ifstream &fichier) : racine(nullptr), cpt(0) {
        if (fichier) {
            for (std::string ligneDico; getline(fichier, ligneDico);) {
                if (ligneDico[0] != '#') //Élimine les lignes d'en-tête
                {
                    // Le mot anglais est avant la tabulation (\t).
                    std::string motAnglais = ligneDico.substr(0, ligneDico.find_first_of('\t'));

                    // Le reste (définition) est après la tabulation (\t).
                    std::string motTraduit = ligneDico.substr(motAnglais.length() + 1, ligneDico.length() - 1);

                    //On élimine tout ce qui est entre crochets [] (possibilité de 2 ou plus)
                    std::size_t pos = motTraduit.find_first_of('[');
                    while (pos != std::string::npos) {
                        std::size_t longueur_crochet = motTraduit.find_first_of(']') - pos + 1;
                        motTraduit.replace(pos, longueur_crochet, "");
                        pos = motTraduit.find_first_of('[');
                    }

                    //On élimine tout ce qui est entre deux parenthèses () (possibilité de 2 ou plus)
                    pos = motTraduit.find_first_of('(');
                    while (pos != std::string::npos) {
                        std::size_t longueur_crochet = motTraduit.find_first_of(')') - pos + 1;
                        motTraduit.replace(pos, longueur_crochet, "");
                        pos = motTraduit.find_first_of('(');
                    }

                    //Position d'un tilde, s'il y a lieu
                    std::size_t posT = motTraduit.find_first_of('~');

                    //Position d'un tilde, s'il y a lieu
                    std::size_t posD = motTraduit.find_first_of(':');

                    if (posD < posT) {
                        //Quand le ':' est avant le '~', le mot français précède le ':'
                        motTraduit = motTraduit.substr(0, posD);
                    } else {
                        //Quand le ':' est après le '~', le mot français suit le ':'
                        if (posT < posD) {
                            motTraduit = motTraduit.substr(posD, motTraduit.find_first_of("([,;\n", posD));
                        } else {
                            //Quand il n'y a ni ':' ni '~', on extrait simplement ce qu'il y a avant un caractère de limite
                            motTraduit = motTraduit.substr(0, motTraduit.find_first_of("([,;\n"));
                        }
                    }

                    //On ajoute le mot au dictionnaire
                    ajouteMot(motAnglais, motTraduit);
                    //std::cout<<motAnglais << " - " << motTraduit<<std::endl;
                }
            }
        }
    }


    /**
     * \brief Constructeur
     */
    Dictionnaire::Dictionnaire()
            : racine(), cpt(0) {}

    /**
     * \brief Le destructeur de la classe
     */
    Dictionnaire::~Dictionnaire() {}

    /**
     * \brief Ajouter un mot au dictionnaire et l'une de ses traductions en équilibrant l'arbre AVL
     * @param motOriginal Le mot d'origine
     * @param motTraduit  La traduction du mot d'origine
     */
    void Dictionnaire::ajouteMot(const std::string &motOriginal, const std::string &motTraduit) {
        _auxInsererAVL(racine, motOriginal, motTraduit);
    }

    /**
     * \brief Insertion d'un noeud dans l'arbre
     * @param arbre La racine de l'arbre
     * @param mot Le mot d'origine
     * @param traduction Sa traduction
     */
    void
    Dictionnaire::_auxInsererAVL(Dictionnaire::NoeudDictionnaire *&arbre, const std::string &mot,
                                 const std::string &traduction) {
        // Condition d'arrêt
        if (arbre == nullptr) {
            arbre = new NoeudDictionnaire(mot, traduction);
            cpt++;
            return;
        }
        // Pencher à gauche
        if (mot < arbre->mot) {
            _auxInsererAVL(arbre->gauche, mot, traduction);
        }
            // Pencher à droite
        else if (mot > arbre->mot) {
            _auxInsererAVL(arbre->droite, mot, traduction);
        }
            // Mot présent
        else {
            // Vérification de la présence de la traduction
            for (int i = 0; i == arbre->traductions.size(); i++) {
                // Traduction présente = on quitte la boucle
                if (arbre->traductions[i] == traduction) {
                    break;
                }
                    // Ajout de la nouvelle traduction
                else if (i == arbre->traductions.size()) {
                    arbre->traductions.push_back(traduction);
                }
            }
        }
        // Balancement de l'arbre suite à l'ajout du noeud
        _balancer(*&arbre);
    }

    /**
     * \brief Balancement de l'arbre
     * @param arbre Le noeud à balancer
     */
    void Dictionnaire::_balancer(Dictionnaire::NoeudDictionnaire *&arbre) {
        // Débalancement à gauche
        if (_debalancementGauche(arbre)) {
            // ZigZag gauche
            if (_sousArbrePencheDroite(arbre->gauche)) {
                _zigZagGauche(arbre);
                // ZigZig gauche
            } else (_zigZigGauche(arbre));
        }
            // Débalancement à droite
        else if (_debalancementDroite(arbre)) {
            // ZigZag droite
            if (_sousArbrePencheGauche(arbre->droite)) {
                _zigZagDroite(arbre);
                // ZigZig droite
            } else (_zigZigDroite(arbre));
        }
            // Aucun débalancement = m.a.j des hauteurs
        else {
            if (arbre != nullptr) {
                arbre->hauteur = 1 + std::max(_hauteur(arbre->droite), _hauteur(arbre->gauche));
            }
        }
    }

    /**
     * \brief Avoir la hauteur d'un noeud
     * @param arb Le noeud
     * @return La hauteur
     */
    int Dictionnaire::_hauteur(NoeudDictionnaire *arb) const {
        // Arbre vide
        if (arb == nullptr) {
            return -1;
        }
        return arb->hauteur;
    }

    /**
     * \brief Vérifier s'il y a débalancement à gauche
     * @param arbre Le noeud à vérifier
     * @return True s'il y a débalancement et False si non.
     */
    bool Dictionnaire::_debalancementGauche(Dictionnaire::NoeudDictionnaire *arbre) const {
        if (arbre == nullptr) {
            return false;
        }
        return 1 < _hauteur(arbre->gauche) - _hauteur(arbre->droite);
    }

    /**
     * \brief Vérifier s'il y a débalancement à droite
     * @param arbre Le noeud à vérifier
     * @return True s'il y a débalancement et False si non.
     */
    bool Dictionnaire::_debalancementDroite(Dictionnaire::NoeudDictionnaire *arbre) const {
        if (arbre == nullptr) {
            return false;
        }
        return 1 < _hauteur(arbre->droite) - _hauteur(arbre->gauche);
    }

    /**
     * \brief Vérifier si le sous arbre penche à droite
     * @param arbre Le noeud à vérifier
     * @return True si c'est vrai et false si non.
     */
    bool Dictionnaire::_sousArbrePencheDroite(Dictionnaire::NoeudDictionnaire *arbre) const {
        if (arbre == nullptr) {
            return false;
        }
        return _hauteur(arbre->gauche) < _hauteur(arbre->droite);
    }

    /**
     * \brief Vérifier si le sous arbre penche à gauche.
     * @param arbre Le noeud à vérifier
     * @return True si c'est vrai et false si non.
     */
    bool Dictionnaire::_sousArbrePencheGauche(Dictionnaire::NoeudDictionnaire *arbre) const {
        if (arbre == nullptr) {
            return false;
        }
        return _hauteur(arbre->droite) < _hauteur(arbre->gauche);
    }

    /**
     * \brief Effectuer un Zigzig à gauche
     * @param K2 Le noeud critique
     */
    void Dictionnaire::_zigZigGauche(Dictionnaire::NoeudDictionnaire *&K2) {
        NoeudDictionnaire *K1 = K2->gauche;
        K2->gauche = K1->droite;
        K1->droite = K2;
        K2->hauteur = 1 + std::max(_hauteur(K2->gauche), _hauteur(K2->droite));
        K1->hauteur = 1 + std::max(_hauteur(K1->gauche), K2->hauteur);
        if (K2 == racine) {
            racine = K1;
        } else {
            K2 = K1;
        }
        // K2 = K1; Le problème est ici, l'assignation ne se fait pas... Le noeud reste le même après la méthode
    }

    /**
     * \brief Effectuer un Zigzig à droite
     * @param K2 Le noeud critique
     */
    void Dictionnaire::_zigZigDroite(Dictionnaire::NoeudDictionnaire *&K2) {
        NoeudDictionnaire *K1 = K2->droite;
        K2->droite = K1->gauche;
        K1->gauche = K2;
        K2->hauteur = 1 + std::max(_hauteur(K2->droite), _hauteur(K2->gauche));
        K1->hauteur = 1 + std::max(_hauteur(K1->droite), K2->hauteur);
        if (K2 == racine) {
            racine = K1;
        } else {
            K2 = K1;
        }
        // K2 = K1; Le problème est ici, l'assignation ne se fait pas... Le noeud reste le même après la méthode
    }

    /**
     * \brief Effectuer un Zigzag à gauche
     * @param K3 Le noeud critique
     */
    void Dictionnaire::_zigZagGauche(Dictionnaire::NoeudDictionnaire *&K3) {
        _zigZigDroite(K3->gauche);
        _zigZigGauche(K3);
    }

    /**
     * \brief Effectuer un Zigzag à droite
     * @param K3 Le noeud critique
     */
    void Dictionnaire::_zigZagDroite(Dictionnaire::NoeudDictionnaire *&K3) {
        _zigZigGauche(K3->droite);
        _zigZigDroite(K3);
    }

    /**
     * \brief Retirer un mot du dictionnaire
     * @param motOriginal Le mot à retirer
     * \pre Le dictionnaire ne doit pas être vide
     * \pre Le mot doit appartenir au dictionnaire
     * \post Le mot doit être retiré du dictionnaire
     */
    void Dictionnaire::supprimeMot(const std::string &motOriginal) {
        if (estVide()) {
            throw std::logic_error("Le dictionnaire est vide");
        } else if (appartient(motOriginal)) {
            _auxEnlever(racine, motOriginal);
        } else (throw std::logic_error("Le mot n'appartient pas au dictionnaire."));
    }

    /**
     * \brief Retirer un noeud de l'arbre
     * @param arbre La racine
     * @param mot La valeur à enlever
     * \pre Le mot doit être présent
     */
    void Dictionnaire::_auxEnlever(Dictionnaire::NoeudDictionnaire *&arbre, const std::string &mot) {
        if (arbre == nullptr) {
            throw std::logic_error("Le noeud est inexistant.");
        }
        if (mot < arbre->mot) _auxEnlever(arbre->gauche, mot);
        else if (mot > arbre->mot) _auxEnlever(arbre->droite, mot);
            // On doit enlever ce noeud-ci
            // Pire cas (complexe: 2 enfants) Chercher le successeur min droit et le retirer
        else if (arbre->gauche != nullptr && arbre->droite != nullptr) {
            _enleverSuccMinDroite(arbre);
        }
            // Cas simple (1 ou pas d'enfants)
        else {
            NoeudDictionnaire *vieuxNoeud = arbre;
            arbre = (arbre->gauche != nullptr) ? arbre->gauche : arbre->droite;
            delete vieuxNoeud;
            --cpt;
        }
        // Rebalancement de l'arbre
        _balancer(arbre);
    }

    /**
     * \brief Retrait du successeur minimal droite
     * @param arbre Le noeud
     */
    void Dictionnaire::_enleverSuccMinDroite(Dictionnaire::NoeudDictionnaire *arbre) {
        NoeudDictionnaire *temp = arbre->droite;
        NoeudDictionnaire *parent = arbre;

        while (temp->gauche != nullptr) {
            parent = temp;
            temp = temp->gauche;
        }
        // Écrasement par le succ. minimal droit
        arbre->mot = temp->mot;

        // retrait du noeud (cas simple)
        if (temp == parent->gauche) {
            _auxEnlever(parent->gauche, temp->mot);
        } else (_auxEnlever(parent->droite, temp->mot));
    }

    /**
     * \brief Vérifie la présence d'un noeud dans l'arbre
     * @param data Le mot à trouver
     * @return Un booléen true si le noeud appartient à l'arbre et false si non
     */
    bool Dictionnaire::appartient(const std::string &data) {
        return _auxAppartient(racine, data) != nullptr;
    }

    /**
     * \brief Vérifie l'appartenance d'un mot dans l'arbre
     * @param arbre La racine
     * @param mot Le mot à trouver
     * @return Le noeud en question
     */
    Dictionnaire::NoeudDictionnaire *
    Dictionnaire::_auxAppartient(Dictionnaire::NoeudDictionnaire *arbre, const std::string &mot) const {
        // Le noeud n'est pas dans l'arbre
        if (arbre == nullptr) {
            return nullptr;
        }
        if (mot < arbre->mot) {
            return _auxAppartient(arbre->gauche, mot);
        } else if (arbre->mot < mot) {
            return _auxAppartient(arbre->droite, mot);
        } else {
            return arbre;
        }
    }

    /**
     * \brief Vérifie si l'arbre est vide
     * @return Un booléen true si l'arbre est vide et false si non.
     */
    bool Dictionnaire::estVide() const {
        return cpt == 0;
    }

    /**
     * \brief Cette fonction détermine un ratio de similitude entre deux mots
     * @param mot1 Le mot d'origine
     * @param mot2 Le mot à comparer
     * @return Un ratio entre 0 et 1 de type double
     */
    double Dictionnaire::similitude(const std::string &mot1, const std::string &mot2) {

        // Cette fonction comparera deux mots en terme de longueur et en terme de lettres identiques.
        // Un vecteur fera la collection des lettres identiques et chaque lettre désignée comme identique
        // sera supprimée du mot2 afin de ne pas la recompter plusiqurs fois et fausser le résultat.

        // Un ratio sera alors calculé pour le nombre de lettres identiques vs le nombre de lettres qu'il y
        // a au total dans le mot1.
        // Un autre ratio sera aussi calculé pour comparer la longueur totale des deux mots.

        // Ces deux ratios seront rammenés sur 1, puis une moyenne sera effectuée afin de trouver le ratio
        // final de similitude.

        // Vecteur de lettres présente dans le 2e mot
        std::vector<char> present;

        // Copie du mot2
        std::string tempMot2 = mot2;

        // Taille du mot à comparer (mot1)
        double sizeMot1 = mot1.size();

        // taille du mot comparé (mot2)
        double sizeMot2 = mot2.size();

        // Comparaison des mots en terme de lettres identiques
        for (auto &lettre : mot1) {
            for (int i = 0; i < tempMot2.size(); i++) {
                if (lettre == tempMot2[i]) {
                    // Si la lettre s'y trouve, suppression de celle ci et ajout dans le vecteur
                    present.push_back(tempMot2[i]);
                    tempMot2.erase(i, 1);
                    break; // sortie de la boucle pour atteindre la prochaine lettre
                }
            }
        }

        // Ratio en terme de lettres identiques
        double ratioLettres = present.size() / sizeMot1;

        // Ratio de taille du mot:
        double ratioSize = (std::min(sizeMot1, sizeMot2)) / (std::max(sizeMot1, sizeMot2));

        // Ratio final
        return (ratioLettres + ratioSize) / 2;

    }

    /**
     * \brief Trouver les traductions possibles d'un mot
     * @param mot Le mot dont on doit trouver les traductions
     * @return Un vecteur contenant les traductions
     */
    std::vector<std::string> Dictionnaire::traduit(const std::string &mot) {
        // Le mot n'est pas présent
        if (!appartient(mot)) {
            std::vector<std::string> vide;
            return vide;
        } else {
            NoeudDictionnaire *current = racine;
            // Parcours de l'arbre jusqu'à ce qu'on trouve le mot
            while (current->mot != mot) {
                if (mot < current->mot) {
                    current = current->gauche;
                } else if (mot > current->mot) {
                    current = current->droite;
                }
            }
            // Le mot est trouvé
            return current->traductions;
        }
    }

    /**
     * \brief Suggère des corrections pour le mot motMalEcrit sous forme d'une liste de mots
     * @param motMalEcrit
     * @return Un vecteur contenant des suggestions de mots
     */
    std::vector<std::string> Dictionnaire::suggereCorrections(const std::string &motMalEcrit) {
        if (estVide()) {
            throw std::logic_error("Le dictionnaire est vide.");
        } else {
            // Parcours de l'arbre jusqu'à ce qu'on atteigne un bout ou bien qu'on ait
            // 5 bonnes options.
            std::vector<std::string> suggestions;
            NoeudDictionnaire *current = racine;
            while (suggestions.size() <= LIMITE_SUGGESTIONS) {
                if (similitude(motMalEcrit, current->mot) >= RATIO_MINIMUM) {
                    suggestions.push_back(current->mot);
                    if (motMalEcrit < current->mot) {
                        if (current->gauche == nullptr) {
                            break;
                        } else { current = current->gauche; }
                    } else {
                        if (current->droite == nullptr) {
                            break;
                        } else { current = current->droite; }
                    }
                } else {
                    if (motMalEcrit < current->mot) {
                        if (current->gauche == nullptr) {
                            break;
                        } else { current = current->gauche; }
                    } else {
                        if (current->droite == nullptr) {
                            break;
                        } else { current = current->droite; }
                    }
                }
            }
            return suggestions;
        }
    }

}//Fin du namespace
