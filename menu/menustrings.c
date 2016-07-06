/*
 * MenuStrings.c
 *
 *  Created on: 10/apr/2016
 *      Author: Administrator
 */
#include "my_definitions.h"
#include "my_types.h"


//stringhe uguali in ognuno dei 4 linguaggi
const char StringsSubmenuLingua [4][10]={  	   "Italiano",
												"English",
												"Francais",
												"Espanol"};

const char StringsSubmenuSimboliConc   [5][4]={ "%",
												"P",
												"g/l",
												"uS",
												"mS"
													};

const char StringsSubmenuCurvaLavoro  [16][5]={                                                 "Pt00",
												"Pt01",
												"Pt02",
												"Pt03",
												"Pt04",
												"Pt05",
												"Pt06",
												"Pt07",
												"Pt08",
												"Pt09",
												"Pt10",
												"Pt11",
												"Pt12",
												"Pt13",
												"Pt14",
												"Pt15"
													};


//Stringhe in linguaggi diversi

const char StringsMenuProg [4][6][20]={{   "IN-OUT (ON-OFF)",  //ITALIANO
                                                                                        "Seleziona Programma",
                                                                                        "Comunicazioni",
                                                                                        "Imposta Orologio",
                                                                                        "Seleziona Lingua",
                                                                                        "Servizio"},

                                                                                { 	"IN-OUT (ON-OFF)",//ENGLISH
                                                                                        "Select Program",
                                                                                        "Communications",
                                                                                        "Set Clock",
                                                                                        "Select Language",
                                                                                        "Service"},

                                                                                { 	"IN-OUT (ON-OFF)",//FRANCAIS
                                                                                        "Choix Program",
                                                                                        "Communications",
                                                                                        "Choix Horloge",
                                                                                        "Choix Langue",
                                                                                        "Service"},

                                                                                { 	"IN-OUT (ON-OFF)",//ESPANOL
                                                                                        "Selecc Program",
                                                                                        "Communicaciones",
                                                                                        "Arregla reloj",
                                                                                        "Languaje",
                                                                                        "Servicio"}


};

const char StringsSubmenuSelezProg[4][5][20]={{"Imposta Simboli",//ITALIANO
                                                                                                "TK",
                                                                                                "Imposta Soglie",
                                                                                                "Curva di Lavoro",
                                                                                                "Imposta Timer"},

                                                                                          { "Set Symbols",	//ENGLISH
                                                                                                "TK",
                                                                                                "Set Curve",
                                                                                                "Set Thresholds",
                                                                                                "Set timers"},

                                                                                          { "Choix Symbols",//FRANCAIS
                                                                                                "TK",
                                                                                                "Choix Curvon",
                                                                                                "Choix Seuils",
                                                                                                "Choix Minuteur"},

                                                                                                { "Selecc Symbols",//ESPANOL
                                                                                                  "TK",
                                                                                                  "Selecc Curva",
                                                                                                  "Selecc Umbrales",
                                                                                                  "Establ Temporizador"}
                                                                                                };




const char StringsSubmenuImpostaSimboli [4][5][20]={{                                           "Simbolo   %",	//ITALIANO
                                                                                                "Simbolo    P",
                                                                                                "Simbolo    g/l",
                                                                                                "Simbolo    uS",
                                                                                                "Simbolo    mS"},

                                                                                              { "Symbol    %",	//ENGLISH
                                                                                                "Symbol    P",
                                                                                                "Symbol    g/l",
                                                                                                "Symbol    uS",
                                                                                                "Symbol    mS"},

                                                                                              { "Symbole  %", //FRANCAIS
                                                                                                "Symbole    P",
                                                                                                "Symbole    g/l",
                                                                                                "Symbole    uS",
                                                                                                "Symbole    mS"},

                                                                                              { "Simbolo  %", //ESPANOL
                                                                                                "Simbolo    P",
                                                                                                "Simbolo    g/l",
                                                                                                "Simbolo    uS",
                                                                                                "Simbolo    mS"}
                                                                                                };//ITALIANO	//ENGLISH	//FRANCAIS	//ESPANOL


const char StringsSubmenuImpostaSoglie[4][8][20]={{ "Set.Conc.", 	                                        //ITALIANO
                                                                                                "All.Conc.Min",
                                                                                                "All.Conc.Max",
                                                                                                "Isteresis.Conc",
                                                                                                "Set Temp",
                                                                                                "All. Temp.Min",
                                                                                                "All. Temp.Max",
                                                                                                "Ister. Temp"},

                                                                                           {"Set.Conc.",	//ENGLISH
                                                                                                "All.Conc.Min",
                                                                                                "All.Conc.Max",
                                                                                                "Hysteresis.Conc",
                                                                                                "Set Temp",
                                                                                                "All. Temp.Min",
                                                                                                "All. Temp.Max",
                                                                                                "Hysteresis.Temp"},

                                                                                           {"Choix.Conc.",	//FRANCAIS
                                                                                                "All.Conc.Min",
                                                                                                "All.Conc.Max",
                                                                                                "Hysteresis.Conc",
                                                                                                "Set Temp",
                                                                                                "All. Temp.Min",
                                                                                                "All. Temp.Max",
                                                                                                "Hysteresis.Temp"},

                                                                                           {"Selecc.Conc.",	//ESPANOL
                                                                                                "Al.Conc.Min",
                                                                                                "Al.Conc.Max",
                                                                                                "Histeresis.Conc",
                                                                                                "Set Temp",
                                                                                                "All. Temp.Min",
                                                                                                "All. Temp.Max",
                                                                                                "Histeresis.Temp"}
                                                                                                };//ITALIANO	//ENGLISH	//FRANCAIS	//ESPANOL

const char StringsSubmenuImpostaTimer [4][9][20]={{	                                        "T1 Rit.CH C",	//ITALIANO
                                                                                                "T2 Dosaggio",
                                                                                                "T3 Rit.Dos.",
                                                                                                "T4 All.Min. C.",
                                                                                                "T5 All.Max. C.",
                                                                                                "T6 Rit.CH T.",
                                                                                                "T7 Riscald.",
                                                                                                "T8 All.Min. T.",
                                                                                                "T9 All.Max. T."},

                                                                                           {    "T1 Rit.CH C",	//ENGLISH
                                                                                                "T2 Dosing",
                                                                                                "T3 Rit.Dos.",
                                                                                                "T4 All.Min. C.",
                                                                                                "T5 All.Max. C.",
                                                                                                "T6 Rit.CH T.",
                                                                                                "T7 Heater.",
                                                                                                "T8 All.Min. T.",
                                                                                                "T9 All.Max. T."},

                                                                                           {    "T1 Rit.CH C",	//FRANCAIS
                                                                                                "T2 Dosage",
                                                                                                "T3 Rit.Dos.",
                                                                                                "T4 All.Min. C.",
                                                                                                "T5 All.Max. C.",
                                                                                                "T6 Rit.CH T.",
                                                                                                "T7 Chauffage.",
                                                                                                "T8 All.Min. T.",
                                                                                                "T9 All.Max. T."},

                                                                                           {    "T1 Rit.CH C",	//ESPANOL
                                                                                                "T2 Dosific",
                                                                                                "T3 Rit.Dos.",
                                                                                                "T4 All.Min. C.",
                                                                                                "T5 All.Max. C.",
                                                                                                "T6 Rit.CH T.",
                                                                                                "T7 Calefaccion.",
                                                                                                "T8 All.Min. T.",
                                                                                                "T9 All.Max. T."}

                                                                                                };//ITALIANO	//ENGLISH	//FRANCAIS	//ESPANOL



//anche per 2 stringhe strutturo così perchè poi ci saranno diverse lingue
const char StringsSubmenuTipoCurvLav [4][3][20]={{ 	                                                "Curva 1 pt",//ITALIANO
                                                                                                        "Curva 2 pt",
                                                                                                        "Curva 3 pt"},

                                                                                                        {"Curve 1 pt",//ENGLISH
                                                                                                         "Curve 2 pt",
                                                                                                         "Curve 3 pt"},

                                                                                                        {"Courbe 1 pt",
                                                                                                         "Courbe 2 pt",
                                                                                                         "Courbe 3 pt"},

                                                                                                        {"Curva 1 pt",
                                                                                                         "Curva 2 pt",
                                                                                                         "Curva 3 pt"}
                                                                                                };//ITALIANO	//ENGLISH	//FRANCAIS	//ESPANOL









