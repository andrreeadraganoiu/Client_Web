322CD Draganoiu Andreea - Tema 3

	Pentru rezolvarea temei am pornit de la solutia scheletului de laborator, 
singura modificare fiind adaugarea JWT ca parametru pentru functiile 
compute_get_request si compute_post_request.
	Pentru parsarea campurilor de tip JSON folosesc biblioteca "parson",
deoarece tema este implementata in limbajul C si a fost recomandata 
in enuntul temei.

Comenzi:

- register : folosesc functia get_user_json_format_string() din helpers.c 
             in care astept utilizatorul sa introduca username-ul si parola.
             Acestea sunt asezate in format json si se convertesc la string
             pentru a fi trimise la server. Verific daca mesajul este de 
             succes, iar daca nu, verific daca mesajul este de eroare, in
             cazul acesta extrag eroarea afisata de server. Daca nu apare
             eroare in corpul raspunsului, iar mesajul nu este de succes
             serverul este prea solicitat.

- login    : folosesc functia get_user_json_format_string() din helpers.c 
             in care astept utilizatorul sa introduca username-ul si parola.
             Analog ca mai sus si ca in toate celelalte comenzi voi verifica
             daca raspunsul contine cuvantul cheie de succes, daca are eroare
             sau serverul nu raspunde. Daca utilizatorul s-a logat cu succes 
             se afiseaza un mesaj corespunzator si se extrage cookie-ul care va
             fi folosit la celelalte operatii in care trebuie sa dovedim ca
             utilizatorul este logat. Daca cookie-ul nu este null, inseamna
             ca utilizatorul este deja conectat si se va afisa acest lucru pe
             ecran.

- entry_library : in aceasta comanda trebuie verificat daca utilizatorul este
                  logat, in caz contrat nu are acces in biblioteca. Daca este logat,
                  adica cookie-ul sau nu este null, extrag jwt-ul care va
                  fi folosit pentru operatiile la care ne este necesar sa 
                  dovedim ca utilizatorul are acces in biblioteca.

- get_books : clientul primeste raspuns la aceasta comanda doar daca a intrat
              cu succes in biblioteca, verificand existenta acelui jwt. Daca
              nu este nici logat cu siguranta nu are acces la aceasta comanda.

- get_book : sunt necesare aceleasi conditii ca mai sus. Se introduce id-ul
             cartii cautate si se verifica daca acesta este valid(numar nat, nu string).
             Ii este cerut utilizatorul sa introduca id-ul pana acesta este
             numar. Daca o carte nu exista acest caz este tratat de extragerea
             mesajului erorii care va aparea.

- add_book : aceleasi restrictii. Folosesc functia get_book_json_format_string() din 
	     helpers.c in care astept utilizatorul sa introduca titlul, autorul, genul,
             editorul si numarul de pagini. De asemenea numarul de pagini trebuie
             sa fie un numar natural, deci se verifica aceasta conditie si ii
             este solicitat utilizatorului sa introduca numarul de pagini pana
             este corect. Se trimite un mesaj de succes daca operatia reuseste.

- delete_book : aceleasi restrictii. Se introduce id-ul cartii de sters, si
                analog se verifica daca acesta este un numar natural. Folosesc
                o functie compute_delete_request, identica cu compute_get_request,
                dar care va trimite la server DELETE.

- logout : similar cu celelalte comenzi. Daca este logat si comanda de delogare
           reuseste se elibereaza cookie-ul si jwt-ul si se afiseaza mesaj de
           succes. 

- exit : se iese din while(1) si prin urmare se termina si executia programului.
