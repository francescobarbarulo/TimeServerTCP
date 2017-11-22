# Time Server TCP
I client connessi inviano periodicamente richieste al server, aspettano il
dato, e lo stampano a video.

#### Server
Il server utilizza il protocollo TCP e invia ai client l'ora in hh:mm:ss.
Inoltre utilizza la primitiva select() per gestire richieste multiple da parte
dei client.

#### Client
Il client si connette al server e periodicamente richiede l'ora.
