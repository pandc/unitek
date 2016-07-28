/*
 * MenuStrings.c
 *
 *  Created on: 10/apr/2016
 *      Author: Administrator
 */
#include "my_definitions.h"
#include "my_types.h"


//stringhe uguali in ognuno dei 4 linguaggi
const char StringsSubmenuLingua [4][10]={                                               "Italiano",
											"English",
											"Francais",
											"Espanol"};

const char StringsSubmenuSimboliConc   [5][4]={ "%",
											"P",
											"g/l",
											"uS",
											"mS"
													};

const char StringsSubmenuCurvaLavoro  [16][5]={                                        "P-00",
											"P-01",
											"P-02",
												"P-03",
												"P-04",
												"P-05",
												"P-06",
												"P-07",
												"P-08",
												"P-09",
												"P-10",
												"P-11",
												"P-12",
												"P-13",
												"P-14",
												"P-15"
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



const char StringsFustoVuotoUp [4][10]={{ 	                                "Fusto"},//ITALIANO
                                                                                                
                                                                               {"Empty"},//ENGLISH
 
                                                                               {"Baril "},//FRANCAIS

                                                                               {"Barril "}
                                                                               };//ITALIANO	//ENGLISH	//FRANCAIS	//ESPANOL


const char StringsFustoVuotoDn [4][10]={{ 	                               "Vuoto"},//ITALIANO
                                                                                                
                                                                               {"Barrel"},//ENGLISH
 
                                                                               {"Vide "},//FRANCAIS

                                                                               {"Vacio "}//ESPANOL
                                                                               };//ITALIANO	//ENGLISH	//FRANCAIS	//ESPANOL


const char StringsServizio [4][5][22]={{ 	                                "Taratura PT100",//ITALIANO
                                                                                "Compensazione cavo",
                                                                                "Lettura diretta ",
                                                                                "Resist serie cavo",
                                                                                "Aggiorna licenza"},

                                                                               {"Calibrate PT100",//ENGLISH
                                                                                "Cable compensation",
                                                                                "Raw measure",
                                                                                "Cable series res.",
                                                                                "Update License"},

                                                                               {"Etallonage PT100",
                                                                                "Compensation de cable",
                                                                                "Mesure directe",
                                                                                "Res. serie du cable",
                                                                                "Mise a jour licence"},

                                                                               {"Calibracion PT100",
                                                                                "Compensacion cable",
                                                                                "Medicion directa",
                                                                                "Res. en serie cable",
                                                                                "Actualiza licencia"}};


const char StringsCableOpen [4][10]={{ 	           "No Cavo"},//ITALIANO
                                                  {"No Cable"},//ENGLISH
                                                  {"No Cable"},//FRANCAIS
                                                  {"No Cable"}//ESPANOL
                                                    };//ITALIANO	//ENGLISH	//FRANCAIS	//ESPANOL
 
const char Strings_RemovePT100 [4][20]={{               "Rimuovi PT100"},//ITALIANO
                                                  {"Remove  PT100"},//ENGLISH
                                                  {"Retirez PT100"},//FRANCAIS
                                                  {"Retire  PT100"}//ESPANOL
                                                    };//ITALIANO	//ENGLISH	//FRANCAIS	//ESPANOL


const char Strings_InsertCalRes [4][25]={{        "Inser. resist. cal."},//ITALIANO
                                                  {"Insert calibr. res."},//ENGLISH
                                                  {"Entrez res. etallon."},//FRANCAIS
                                                  {"Connect res. calibr."}//ESPANOL
                                                    };//ITALIANO	//ENGLISH	//FRANCAIS	//ESPANOL


const char Strings_RemoveProbe [4][20]={{               "Rimuovi Sonda"},//ITALIANO
                                                  {"Remove  probe"},//ENGLISH
                                                  {"Retirez sonde"},//FRANCAIS
                                                  {"Retire  sonda"}//ESPANOL
                                                    };//ITALIANO	//ENGLISH	//FRANCAIS	//ESPANOL

const char StringsCableCalibOK [4][20]={{          "Calibr. Cavo   OK"},//ITALIANO
                                                  {"Cable Calibr.  OK"},//ENGLISH
                                                  {"Etallon. Cable OK"},//FRANCAIS
                                                  {"Calibr.  Cable OK"}//ESPANOL
                                                    };

/*
const char StringsMisDiretta [4][20]={{            "Misura diretta"},//ITALIANO
                                                  {"Direct measurem."},//ENGLISH
                                                  {"Measure directe"},//FRANCAIS
                                                  {"Medicion directa"}//ESPANOL
                                                    };*/
const char Strings_GGMMAA [4][10]={{               "gg/mm/aa"},//ITALIANO
                                                  {"dd/mm/yy"},//ENGLISH
                                                  {"jj/mm/aa"},//FRANCAIS
                                                  {"dd/mm/aa"}//ESPANOL
                                                    };                                                      
                                                      