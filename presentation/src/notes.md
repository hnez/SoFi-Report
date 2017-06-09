---
title: SoFi presentation notes
lang: de-DE
...

SoFi
====

Worum geht es?

Direction of arrival estimation - Mit mehreren
Empfängern die Ursprungsrichtung von Funksignalen finden

Lange bekanntes Problem, gut erforscht,
verschiedenste Ansätze, z.B. Doppler basiert mit
"drehender" Antenne

Ist aber keine Consumer-Anwendung, also teuer

Wie billig kriegt man das ganze hin? Geht das
unter 50€?

Im Bild angedeutet: es sollen vier Antennen genutzt
werden, die auf einer Fläche positioniert werden.
Abstände ungefähr in der Größenordnung der Wellenlänge.

Für 100MHz also irgendwo im Meter bereich.

----

Concept
=======

Der Plan:

- Für das Geld software defined radios kaufen
- Dafür sorgen, dass die als ein System funktionieren.
  Dafür müssen die "synchronisiert" werden.
- Anschließend Phasendifferenzen nutzen um Richtungen
  zu bestimmen

Diagramme wie das auf dem Bild kommen noch öfter.
Haben was mit Phasendifferenzen zu tun.

----

Get a few SDRs
==============

Q: Wie bekommt man SDRs für vier Antennen für 50€? <br/>
A: Consumer-Geräte missbrauchen!

Dafür kauft man bei einem Online-Händler "RTL SDR" Dongles,
das sind DVB-T sticks, die sich als software defined Radio
benutzen lassen.

Kosten etwa 10€. Für 50€ bekommt man also vier und
noch einen USB-Hub dazu.

----

Auf den Sticks ist ein Realtek 2832U IC verbaut.
In den steckt man auf einer Seite ein I/Q Basisband
Signal rein und auf der anderen Seite kommen MPEG-2
frames über USB raus, wenn man damit DVB-T empfängt.

Um die RF-Signale in das Basisband zu bekommen ist noch
ein R802T Tuner IC verbaut. Der nimmt das Antennensignal,
verstärkt es und mischt es runter.

Der Tuner deckt einen weiten Frequenzbereich ab.
40MHz - 1,8GHz sind möglich.

*Dabei auf die ICs auf dem Bild zeigen*

Linux Entwickler haben Support für die DVB-T Funktion
im Kernel integriert, wie auch schon für einige andere
Chips. Heißt:

- USB-Kommandos reverse-engineered
- Paketformate reverse-engineered
- Ansteuerung des Tuners reverse-engineered

Unter der Windows-Software des Herstellers konnten die
Dongles auch FM-Radio und DAB empfangen.

Das wollten die Linux Kernelentwickler auch implementieren.

Beim reverse-engineering der USB-Kommunikation unter Windows
fiel auf, dass in den Chips gar kein FM-Dekoder ist.

Stattdessen werden direkt I/Q Samples aus den ADCs
an Computer geschickt.

Um DAB empfangen zu können müssen die ~1,5MHz breiten
Multiplexe dekodiert werden können.
Bandbreite ist deshalb recht hoch mit 2MS/s

Günstige Verfügbarkeit hat 2012-2013 eine
"SDR revolution" im Hobby bereich ausgelöst,
weil auf einmal billige (10€) verfügbar waren.

----

Synchronize them
================

SDRs sollen jetzt als ein Messsystem arbeiten,
dafür sind Anpassungen notwendig.

Wie sieht ein SDR allgemein und die RTL_SDRs
im besonderen eigentlich aus?

Zum Bild:
---------

### Tuner:

1. Antennensignal kommt rein und wird verstärkt
2. Mit einen Quarz wird eine Referenzfrequenz erzeugt
   *Quarz auf Foto zeigen und in Diagramm*
3. Quarzfrequenz wird mit PLL multipliziert und
   dividiert, also mit einer raionalen Zahl multipliziert
   um eine LO-Frequenz zu erzeugen.
4. Mit der I/Q LO-Frequenz wird das RF-Signal aus dem
   LNA runtergemischt.
5. Unerwünschte Frequenzen werden weggefiltert.
6. I und Q werden aus dem Tuner geführt.
   Taktsignal wird auch aus dem Tuner geführt.

*Tuner auf dem Foto zeigen*

### ADC:

1. Aus Taktsignal von Tuner wird eine sampling-frequenz erzeugt
2. Basisbandsignal wird abgetastet und digitalisiert.
   RTL_SDR: 8bit
3. Digitalisierte Werte werden in USB-Pakete verspackt und
   an den Computer gesendet.

*ADC auf dem Foto zeigen*

Zum Text:
---------

Dongles sind gebaut um billig zu sein

Quarze sind nicht unbedingt präzise, können ein
paar Hz daneben sein.

Bei mehreren Empfängern scheint das
selbe Signal um ein paar Hz verschoben zu sein,
wenn man das Spektrum anschaut.

Für DOA Schätzung wollen wir Phasen zwischen
den Empfängern messen.
Wenn die sich durch Frequenzunterschiede dreht,
dann wird das nichts.

Sampling-Frequenzen sind auch unterschiedlich.
Die Datenpuffer füllen sich also unterschiedlich schnell.
Zeit zu der "gleichzeitige" Samples aufgenommen wurden
driftet also außeinander.

Also: Frequenzunterschiede sind nicht gut.

Q: Was tun?

----

A: Dongles mit dem selben Basistakt versorgen.
   Die PLLs werden dann dafür sorgen, dass
   die anderen Frequenzen auch gleich sind.
   Und die Phasen nicht so gerne driften.

----

Versuch 1:
----------

Von zwei Dongles den Quarz entfernt

*Zeigen wo die Quarze nicht mehr sind*

Beide Receiver mit einem Pierce-Oszillator versorgt,
der mit einem NAND gate gefrickelt war, weil kein
Inverter im Geraffellager war.

Hat grundsätzlich schon mal funktioniert.

Clock Signal sah aber nicht unbedingt gut aus,
hatte negative Effekte auf Stabilität

----

Versuch 2:
----------

Richtige Platine fräsen und einen Hex-Inverter benutzen

Erste Versuche mit vier Empfängern

Clock Signal sah aber wieder nicht gut aus

*Hardware rumgeben?*

----

Versuch 3:
----------

*Auf Clock-Ausgang zeigen, der zum ADC führt*

Der Clock-Ausgang ist ein wirklich physikalischer
Pin am Tuner.

Wenn man wirklich will kann man da also was anlöten.

----

*Auf Drähte zeigen*

Drähte verbinden jeweils einen Clock-Ausgang mit
dem Eingang des nächsten Dongles

Weil die Signal-Pegel komisch sind ist ein
Kopplungsnetzwerk nötig.

Durch kürzere Leitungen und die dafür
gedachten komponenten sehen die Signale
jetzt gut aus

----

Durch synchronisierte Frequenzen sind drifts
mit der Zeit gelöst.

Sampling Frequenz ist gleich => Puffer
werden gleich schnell voller

LO-Frequenz ist gleich => Signal Phasen drehen
sich nicht mehr zueinander

Aber:

Die Empfänger fangen vielleicht nicht gleichzeit mit Samples
aufnehmen an. Gibt also einen konstanten Zeitunterschied
*Hier wild mit Händen gestikulieren zum Zeitunterschiede zeigen*

Die LO-Phasen drehen sich vielleicht nicht zueinander,
können aber einen konstanten Offset haben.

Beides ist schlecht für DOA, weil vergleichbare Phasen
gebraucht werden.

----

Sample aquisition offset
========================

Bild erklären:

- Zahl zeigt die Position in den Geräte-Puffern an
- Position auf der t-Achse zeigt die Aufnahmezeit an
- Farbe zeigt an, welche Information im Sample steckt
  Weil die Antennen nahe beieinander stehen empfangen
  die auch zur selben Zeit etwa das gleiche

----

Bild erklären:

- Beim Auslesen ist die absolute Zeitinformation weg
- Samples an gleicher Pufferposition haben nichts
  mit einander zu tun
- Im Bild sind das nur ein paar Samples, die daneben sind
  in echt sind das ein paar Tausend

----

Absolute Zeitinformation der Samples ist weg für immer

Aber Inhalt der Samples lässt sich zur Synchronisation nutzen.

Kreuzkorrelation berechnen und so lange Samples aus den Puffern
weg werfen bis zur gleichen Zeit aufgenommene Samples an
der selben Pufferposition stehen.

*Sollte der Vortrag ganz doll zu kurz werden vielleicht auf die Links klicken*

----

*Zurück auf erstes buntes Sync-Diagramm springen*

Start-Zeiten der SDRs halten sich nicht an Sampling-Zeitpunkte.

Bisherige Synchronisation funktioniert nur auf ein Sample genau

Um mit größerer Genauigkeit Samples wegwerfen zu können
müsste man fraktional resamplen. Das ist aber rechenintensiv

Q: Was macht eine Verschiebung im Zeitbereich im Frequenzbereich
A: Sorgt für eine lineare Drehung der Phase

So eine Drehung lässt sich im Frequenzbereich simpel
kompensieren.

----

Frequency domain
================

Deshalb von hier an weiter im Frequenzbereich.

Exkurs dahin, wie das ganze System funktionieren soll.

Zum Bild:
---------

Es werden immer Antennenpaare betrachtet:

Q: Bei 4 Antennen gibt es wie viele Paare
A: 6

Für jedes Antennenpaar wird ein Datenstrom erzeugt:

- Dongle empfängt Funksignale
- Zeitunterschiede werden durch Wegwerfen von Samples
  grob kompensiert
- Per FFT werden die Signale in den Frequenzbereich
  transformiert
- Für jeden FFT-bin wird die Phasendifferenz berechnet
- Weil die Phasendifferenz für empfangene Signale
  einigermaßen konstant bleiben sollte kann man
  die jetzt mitteln.
  Das veringert später den Processing-load
- Weil beide Empfänger eine sich über die
  Frequenz drehende Phase haben (w.g. Sample-Zeit Unterschied).
  Hat auch dieses Ausgangssignal eine drehende Phase.

----

*Folie skippen*

Ist interessant zum klicken um sich die
tatsächliche Implementierung anzugucken

----

Phasendiagramm erklären:

- Zeigt die 6 Phasendifferenzen der 4 Empfänger im
  Frequenzbereich.
- Randbereiche sind zu ignorieren, da machen
  Filter schlimme Sachen
- Wenn man genau hinguckt kann man Signale erkennen
- Krummheit kommt aus den Zeitverschiebungen
- Lässt sich jetzt einfach kompensieren

Intuitive Lösung: Steigung ansehen und von
der Phase abziehen

Ist etwas zweifelhaft: Ich verlasse mich darauf,
dass das Rauschen für alle Empfängerpaare eine
Phasendiffernz von 0 aufweist.

Ob die Annahme stimmt ist nicht klar,
wird aber erstmal genutzt.

Nach Kompensation bleibt noch ein konstanter
Offset aus den Unterschieden der LO-Phasen

----

LO-Phase offset
===============

Substrahiert man diesen konstanten Offset
und zoomt rein fängt man an Phasenunterschiede
zwischen den Antennenpaaren zu sehen für die
verschiedenen Signale.

Bei schwächeren Signalen werden die Phasenunterschiede
durch das Rauschen richtung 0 verkleinert.

Der DOA Algorthmus kann sich also nicht auf die
absoluten Phasenunterschiede verlassen, sondern
die nur zwischen Antennenpaaren vergleichen.

----

Direction estimation
====================

Bild erklären:
--------------

- Signalquellen sind so weit weg, dass "Strahlen" parallel sind
- Phasenlage der Signale hängt vom Abstand zwischen Quelle und
  Empfänger ab
- Im gegebenen Array mit dem Signal is Phasenunterschied zwischen
  Antenne A und Antenne B gleich 0, für D und C gilt das selbe.
  Sind ja gleich weit von der Quelle entfernt.
- Zwischen A und D besteht ein Phasenunterschied, genau wie zwischen
  B und C
- Phasendifferenzen lassen also Rückschlüsse auf
  Ankunftsrichtung zu

----

Bild erklären:
--------------

- Beispiel vorher hatte nur rechte Winkel
- Jetzt trifft ein Signal mit einem anderen Winkel
  auf das Antennenpaar
- Ohne Rauschen lassen sich die Phasendifferenzen direkt
  aus der Wellenlänge, dem Einfallswinkel und
  dem Antennenabstand berechnen
- Genau so kann man das umdrehen und aus der Phasendifferenz
  die Richtung rekonstruieren

----

Mit Rauschen wird es schwieriger, weil Rauschen
die gemessenen Werte beeinflusst

----

Implementation
==============

Ist zweigeteilt, vor dem Downsampling wird
alles in einem selbstgebauten C-Backend gemacht.

Das übernimmt die Kommunikation mit den SDRs,
die grobe, auf ein Sample genaue Zeitsynchronisation,
und die Berechnung der Phasendifferenzen.

Nach dem Downsampling übernimmt Python.
Mit Hilfe von numpy und scipy werden die
Kompensationen der Phaseninformationen durchgeführt,
die DOA informationen berechnet und die
Ergebnisse in einer GUI angezeigt

----

Um die DOA zu berechnen erzeugt das Program einen Testvektor
mit erwarteten Phasenunterscheiden für jede Eintrittsrichtung.

Die Testvektoren werden mit den gemessenen Phaseunterschieden
Skalarmultipliziert.

Wenn beide parallel sind ist das Skalarprodukt maximal.

Der Testvektor für den das Skalarprodukt maximal ist
wurde mit dem Winkel erzeugt, der dem Eintrittswinkel
entspricht.

Nachteil: Das Skalarprodukt erzeugt keine scharfen Kanten,
mehr so das Gegenteil.

----

Die Implementierung nutzt statt mehrerer Testvektoren
eine Testmatrix, verhält sich aber an sich wie erwähnt.

----

Conclusion
==========

Funktioniert nicht so richtig toll:

- Richtungsausgabe ist sehr weich, gute Peaks gibt es da nicht
- Grobe Synchronisation funktioniert größtenteils
- Ob die feine synchronisation durch Hintergrundrauschen
  funktioniert ist nicht wirklich geklärt
- Tests mit Sender in direkter Umgebung zeigen oft komische
  Resultate. Reflexionen/algorithmisch was verkehrt?
- Durch rumprobieren am Ende hat der Code gelitten

----

Outlook
=======

Wenn ich noch einmal neu an das Problem herangehen würde:

- So wenig wie möglich selbst implementieren, GnuRadio nutzen
- Als gut bekannte Algorithmen nutzen:
- MUSIC performt besser als einfachere Algorithmen, aber:
    - Ist schwer zu verstehen
    - Meist nur theoretisch/in Simulation untersucht,
      vieles wird ignoriert: Unterschiedliche Wellenlängen,
      Offsets der Receiver
- Deshalb hybriden Ansatz nutzen:
    - Eingangssignal mit FFT in schmalbandige Untersignale unterteilen,
      wie bei OFDM
    - Empfängeroffsets wie gehabt kompensieren durch Phasendrehen
      (geht, weil schmalbandig)
    - Schmalbandige Untersignale mit MUSIC verarbeiten
    - MUSIC braucht Aussage über Anzahl der Signale,
      ist so ziemlich sicher 1 (für FM-Radio)

----

Quellcode
=========

Alles online, Software, Bericht, Präsentation
und der jeweilige Quellcode

----

*Demo machen*
