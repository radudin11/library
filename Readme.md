# Tema 4 Protocaole de comunicatii: Client Web. Comunicaţie cu REST API.

## Obiectiv
Crearea unui client Web care să comunice cu un REST API. Familiarizarea cu 
concepte precum:
- protocolul HTTP
- JSON
- JWT

## Descriere generala
Datele de conectare
HOST: 34.254.242.81 PORT: 8080

Serverul expune un API ce simuleaza o biblioteca virtuala. Acesta este considerat
implementat.

Clientul trebuie sa comunice cu serverul folosind protocolul HTTP. Acesta accepta
comenzi de la tastatura si trimite cereri catre server.

## Implementare

### Functiile implementate sunt urmatoarele:
### - `register` - inregistrarea unui nou utilizator
Parametrii:
    - `username` - numele de utilizator, fara spatii
    - `password` - parola
On success: Va aparea raspuunsul serverului si `Registered successfully!`

### - `login` - logarea unui utilizator
Parametrii:
    - `username` - numele de utilizator, fara spatii
    - `password` - parola

On success: Va aparea raspuunsul serverului si `Logged in successfully!`. De asemenea,
se va salva un cookie pentru a pastra sesiunea.

### - `enter_library` - intrarea in biblioteca
Parametrii: Niciunul
On success: Va aparea raspuunsul serverului si `Entered library!`. De asemenea,
se va salva un JWT pentru a tine cont de intrarea in biblioteca.

### - `get_books` - afisarea cartilor din biblioteca
Parametrii: Niciunul
On success: Va aparea raspuunsul serverului si lista de carti din biblioteca.

### - `get_book` - afisarea unei carti din biblioteca
Parametrii:
    - `id` - id-ul cartii
On success: Va aparea raspuunsul serverului si cartea ceruta.

### - `add_book` - adaugarea unei carti in biblioteca
Parametrii:
    - `title` - titlul cartii
    - `author` - autorul cartii
    - `publisher` - editura cartii
    - `genre` - genul cartii
    - `page_count` - numarul de pagini al cartii
On success: Va aparea raspuunsul serverului si cartea adaugata.

### - `delete_book` - stergerea unei carti din biblioteca
Parametrii:
    - `id` - id-ul cartii
On success: Va aparea raspuunsul serverului.

### - `logout` - delogarea unui utilizator
Parametrii: Niciunul
On success: Va aparea raspuunsul serverului si `Logged out successfully!`. De asemenea,
se va sterge cookie-ul si JWT-ul.

### - `exit` - iesirea din aplicatie

## Rulare
Pentru a rula aplicatia, se va rula `make run` in directorul sursa.

## JSON parsing
Pentru parsarea JSON-urilor, am folosit `parson.c`, o biblioteca open-source
disponibila la https://github.com/kgabis/parson. Aceasta este inclusa in directorul sursa.
Am ales aceasta biblioteca deoarece este foarte simplu de folosit si nu necesita
o documentatie foarte lunga. Are un singur fisier header si unul sursa, iar API-ul
este foarte friendly si human readable.


## Detalii de implementare.

Cererile HTTP sunt create folosind functiile din `requests.h`. Acestea sunt
inspirate din laboratorul 9 si sunt modificate pentru a se potrivi cu cerintele
acestei teme.

In header-ul `helpers.h` se afla functii ajutatoare pentru a crea conexiunea
cu serverul, pentru a trimite cererile si pentru a primi raspunsurile. Acestea
sunt copiate din laboratorul 9.

Functiile din `client.c` interogheaza utilizatorul pentru a primi parametrii
necesari, creeaza cererea HTTP si o trimite la server. Daca este necesar, 
raspunsul este interpretat folosind functiile din `parson.c`. Acestea afiseaza
raspunsul si mesaje ajutatoare pentru a indica succesul sau esecul cererii.


