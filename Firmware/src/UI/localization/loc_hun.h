/* Copyright (C) 2018, Attila Kovács
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation  and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of any
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* This file is part of SolderingStation.
 */

#ifndef UI_LOC_HUN_H_
#define UI_LOC_HUN_H_

#include "arch.h"

// Empty
#define HUN_TEXT00001 ""

// Generic texts
#define HUN_TEXT00002 "Be"
#define HUN_TEXT00003 "Ki"
#define HUN_TEXT00004 "Engedélyezve"
#define HUN_TEXT00005 "Letiltva"
#define HUN_TEXT00006 "Forrasztó állomás"
#define HUN_TEXT00007 "©2019 Kovács Attila"
#define HUN_TEXT00008 "24VAC sín:"
#define HUN_TEXT00009 "Hő határolás:"
#define HUN_TEXT00010 "ADC hőmérséklet:"
#define HUN_TEXT00011 "ADC TE referencia:"
#define HUN_TEXT00012 "Táp feszültség:"
#define HUN_TEXT00013 "Csatlakoztasd az USB kábelt és telepítsd az új firmware-t. Good Luck!"
#define HUN_TEXT00014 "Hőmérséklet .1F:"

// Tool states
#define HUN_TEXT01001 "Üres"
#define HUN_TEXT01002 ""
#define HUN_TEXT01003 "Kikapcs."
#define HUN_TEXT01004 "Alszik"
#define HUN_TEXT01005 "Fűtés"
#define HUN_TEXT01006 "Felfűtve"
#define HUN_TEXT01007 "Hiba"
#define HUN_TEXT01008 "Kalibrál"
#define HUN_TEXT01009 "Csere"
#define HUN_TEXT01010 "Beállít"
#define HUN_TEXT01011 "Lehűl"
#define HUN_TEXT01101 "Hdg."

// Units
#define HUN_TEXT00101 "°C"
#define HUN_TEXT00102 "°F"
#define HUN_TEXT00103 "perc"
#define HUN_TEXT00104 "V"
#define HUN_TEXT00105 "A"
#define HUN_TEXT00106 "W"
#define HUN_TEXT00107 "mV"
#define HUN_TEXT00108 ","		// Decimal point

// Menu controls
#define HUN_TEXT00201 "Vissza"
#define HUN_TEXT00202 "Kiválaszt"
#define HUN_TEXT00203 "Módosít"
#define HUN_TEXT00204 "Mégsem"
#define HUN_TEXT00205 "Elfogad"
#define HUN_TEXT00206 "Nyugtáz"
#define HUN_TEXT00207 "Beállít"
#define HUN_TEXT00208 "Előző"
#define HUN_TEXT00209 "Ok"
#define HUN_TEXT00210 "Mód"

// Menu items
#define HUN_TEXT00301 "Beállítások"
#define HUN_TEXT00302 "Rendszer"
#define HUN_TEXT00303 "Valami"
#define HUN_TEXT00304 "Hang: Be"
#define HUN_TEXT00305 "Hang: Ki"
#define HUN_TEXT00306 "Language: Magyar"			// Write name of current language here
#define HUN_TEXT00307 "Páka kalibráció"
#define HUN_TEXT00308 "LCD gamma beállítás"
#define HUN_TEXT00309 "Firmware frissítés"
#define HUN_TEXT00310 "Hang teszt"
#define HUN_TEXT00311 "Újraindítás"
#define HUN_TEXT00312 "Jelentés"
#define HUN_TEXT00313 "Oszcilloszkóp"
#define HUN_TEXT00314 "Alapértelmezett beállítások"

// Alarms, warnings and notifications and their descriptions
#define HUN_TEXT02001 "Hiba!"
#define HUN_TEXT02002 "Figyelmeztetés!"
#define HUN_TEXT02003 "Információ"
#define HUN_TEXT02011 "Elvesztek a rövidtávú beállítások!"
#define HUN_TEXT02012 "Lemerült az elem (CR1220) vagy inkompatibilis szoftververzió"
#define HUN_TEXT02021 "Fejlesztői mód aktív!"
#define HUN_TEXT02022 "A fejlesztői kód rosszabb hatásfokkal működik"
#define HUN_TEXT02031 "Elvesztek a kalibrációs beállítások!"
#define HUN_TEXT02032 "Az ismételt használathoz a forrasztóállomást újra kell kalibrálni"
#define HUN_TEXT02041 "Forrasztópáka kalibrálás"
#define HUN_TEXT02042 "Csatlakoztassa a kalibrálandó pákát az egyes bemenetre"
#define HUN_TEXT02051 "Kalibrálás befejeződött"
#define HUN_TEXT02052 "A kalibrálás sikeresen befejeződött"
#define HUN_TEXT02061 "Kalibrálatlan páka!"
#define HUN_TEXT02062 "A csatlakoztatott páka kalibrálatlan"
#define HUN_TEXT02071 "Hőkorlátozás aktív!"
#define HUN_TEXT02072 "A leadott hőteljesítmény meghaladta a megengedett értéket"
#define HUN_TEXT02081 "Túlfeszültség!"
#define HUN_TEXT02082 "A 24VAC sínen túlfeszültséget érzékeltem, az állomás letiltva"
#define HUN_TEXT02091 "Túláram vagy rövidzár!"
#define HUN_TEXT02092 "A 24VAC sínen túláramot érzékeltem, az állomás letiltva"
#define HUN_TEXT02101 "Hőmérséklet túlfutás!"
#define HUN_TEXT02102 "A páka hőmérséklete meghaladta a megengedett értéket, az állomás letiltva"
#define HUN_TEXT02111 "Magas belső hőmérséklet!"
#define HUN_TEXT02112 "Az állomás hőmérséklete meghaladta a megengedett értéket, az állomás letiltva"
#define HUN_TEXT02121 "A jelentés elküldése sikertelen!"
#define HUN_TEXT02122 "Kapcsold ki a telemetriai adatok küldését"
#define HUN_TEXT02131 "Elvesztek a beállítások!"
#define HUN_TEXT02132 "Inkompatibilis szoftververzióról lett frissítve"
#define HUN_TEXT02141 "Beállítások visszaállítva"
#define HUN_TEXT02142 "A felhasználói beállítások az alapértelmezett értékekre visszaállítva"

// Dialogs
#define HUN_TEXT05011 "Nem érzékelhető páka!"
#define HUN_TEXT05012 "Az egyes bemeneten nem érzékelhető csatlakoztatott páka"

// Settings and their descriptions
#define HUN_TEXT03011 "Altatási hőmérséklet:"
#define HUN_TEXT03012 "A pákahegy hőmérséklete, amikor a tartóra helyezték"
#define HUN_TEXT03021 "Késleltetett kikapcsolás:"
#define HUN_TEXT03022 "A páka automatikus kikapcsolásának késleltetése altatáskor"
#define HUN_TEXT03031 "Beállítás lépésköze:"
#define HUN_TEXT03032 "A kézi finom hőmérsékletbeállítás legkisebb lépésköze"
#define HUN_TEXT03041 "Hőmérséklet korlát:"
#define HUN_TEXT03042 "A hőmérsékletbeállítás legmagasabb engedélyezett értéke"
#define HUN_TEXT03051 "Mértékegység:"
#define HUN_TEXT03052 "A hőmérséklet kijelzéshez használt mértékegység"
#define HUN_TEXT03061 "Telemetria:"
#define HUN_TEXT03062 "Telemetriai adatok küldése USB CDC-n (virtuális soros port)"
#define HUN_TEXT03071 "Diagnosztika:"
#define HUN_TEXT03072 "A főképernyőn megjelenítésre kerülnek a mért paraméterek"
#define HUN_TEXT03081 "Képernyő kikapcsolás:"
#define HUN_TEXT03082 "Képernyő háttérvilágításának kikapcsolása ha nincs aktivitás"
#define HUN_TEXT03091 "Teljesítmény korlátozás:"
#define HUN_TEXT03092 "A forrasztópáka korlátozza a leadott átlagteljesítményt a megadott értéken"

// Iron calibration
#define HUN_TEXT04001 "Első kalibrációs pont:"
#define HUN_TEXT04002 "Második kalibrációs pont:"
#define HUN_TEXT04003 "Harmadik kalibrációs pont:"
#define HUN_TEXT04004 "Állítsd be a pákahegyen mért pontos hőmérsékletet"

#endif /* UI_LOC_HUN_H_ */
