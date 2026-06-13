Interpretacja tych pól (CC1101) jest taka:

carrier=0
Odbiornik nie widzi nośnej (Carrier Sense nieaktywny).

pqt=1
Próg jakości preambuły został chwilowo przekroczony (PQT reached). To może się zdarzyć nawet od szumu lub zakłóceń, jeśli warunki są graniczne.

sfd=0
Nie wykryto słowa synchronizacji (Sync Word Detect = 0), więc ramka nie została poprawnie „złapana”.

max_rssi=-107 dBm
Bardzo słaby sygnał, blisko poziomu szumu tła.

Co oznacza ten zestaw razem:

Radio widzi coś podobnego do preambuły, ale nie widzi stabilnej nośnej i nie trafia w sync word.
Najczęściej to nie jest „za silny sygnał”, tylko raczej:
niezgodna konfiguracja TX/RX (częstotliwość, bitrate, dewiacja, sync word, modulacja),
duży offset częstotliwości,
problem anteny/dopasowania,
za krótka preambuła.
Praktycznie:

Przy realnie bliskim nadajniku RSSI zwykle byłby dużo wyższy (np. około -70 do -30 dBm), a nie -107 dBm.
Ten log bardziej wskazuje na brak poprawnego „zestrojenia” linku niż na przesterowanie odbiornika
