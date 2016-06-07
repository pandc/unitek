/*
 * MenuStrings.c
 *
 *  Created on: 10/apr/2016
 *      Author: Administrator
 */
#include "my_definitions.h"
#include "my_types.h"


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

const char StringsSubmenuCurvaLavoro  [16][4]={ "osX",
												"01X",
												"02X",
												"03X",
												"04X",
												"05X",
												"06X",
												"07X",
												"08X",
												"09X",
												"10X",
												"11X",
												"12X",
												"13X",
												"14X",
												"15X"
													};


		//		***ITALIANO***ITALIANO***ITALIANO***ITALIANO***ITALIANO***ITALIANO***ITALIANO***

		const char StringsMenuProg [4] [6][20]={{   "IN-OUT (ON-OFF)",
													"Seleziona Programma",
													"Comunicazioni",
													"Imposta Orologio",
													"Seleziona Lingua",
													"Servizio"},

												{ 	"IN-OUT (ON-OFF)",
													"Select Program",
													"Communications",
													"Set Clock",
													"Select Language",
													"Service"},

												{ 	"IN-OUT (ON-OFF)",
													"Choix Program",
													"Communications",
													"Choix oralojon",
													"Choix Languedoc",
													"Service"},

												{ 	"IN-OUT (ON-OFF)",
													"Selecc Program",
													"Communicacions",
													"Arregla reloj",
													" Languaje",
													"Servicio"}


		};

		const char StringsSubmenuSelezProg [5][20]={"Imposta Simboli",
													"TK",
													"Curva di Lavoro",
													"Imposta Soglie",
													"Imposta Timer"};




		const char StringsSubmenuImpostaSimboli [5][20]={"Simbolo   %",
														"Simbolo    P",
														"Simbolo    g/l",
														"Simbolo    uS",
														"Simbolo    mS"};


		const char StringsSubmenuImpostaSoglie [8][20]={"Set.Conc.",
														"All.Conc.Min",
														"All.Conc.Max",
														"Isteresis.Conc",
														"Set Temp",
														"All. Temp.Min",
														"All. Temp.Max",
														"Ister. Temp"};

		const char StringsSubmenuImpostaTimer [9][20]={	"T1 Rit.CH C",
														"T2 Dosaggio",
														"T3 Rit.Dos.",
														"T4 All.Min. C.",
														"T5 All.Max. C.",
														"T6 Rit.CH T.",
														"T7 Riscald.",
														"T8 All.Min. T.",
														"T9 All.Max. T."};



		//anche per 2 stringhe strutturo così perchè poi ci saranno diverse lingue
		const char StringsSubmenuTipoCurvLav [2][20]={ 	"Calibrazione Centr",
															"Calibrazione L-C-H"
														};


		//		***ENGLISH***ENGLISH***ENGLISH***ENGLISH***ENGLISH***ENGLISH***ENGLISH***ENGLISH***

		const char StringsMenuProgEngl [6][20]={ 	"IN-OUT (ON-OFF)",
													"Select Program",
													"Communications",
													"Set Clock",
													"Select Language",
													"Service"};

		const char StringsSubmenuSelezProgEngl [5][20]={ "Set Symbols",
														"TK",
														"Set Curve",
														"Set Thresholds",
														"Set timers"};




		const char StringsSubmenuImpostaSimboliEngl [5][20]={"Symbol   %",
														"Symbol    P",
														"Symbol    g/l",
														"Symbol    uS",
														"Symbol    mS"};


		const char StringsSubmenuImpostaSoglieEngl [8][20]={"Set.Conc.",
														"All.Conc.Min",
														"All.Conc.Max",
														"Hysteresis.Conc",
														"Set Temp",
														"All. Temp.Min",
														"All. Temp.Max",
														"Hysteresis.Temp"};

		const char StringsSubmenuImpostaTimerEngl [9][20]={	"T1 Rit.CH C",
														"T2 Dosing",
														"T3 Rit.Dos.",
														"T4 All.Min. C.",
														"T5 All.Max. C.",
														"T6 Rit.CH T.",
														"T7 Heater.",
														"T8 All.Min. T.",
														"T9 All.Max. T."};


		//anche per 2 stringhe strutturo così perchè poi ci saranno diverse lingue
		const char StringsSubmenuTipoCurvLavEngl [2][20]={ 	"Calibration Centr",
															"Calibration L-C-H"
														};


		//		***FRANCAIS***FRANCAIS***FRANCAIS***FRANCAIS***FRANCAIS***FRANCAIS***FRANCAIS***FRANCAIS***

		const char StringsMenuProgFran [6][20]={ 	"IN-OUT (ON-OFF)",
													"Choix Program",
													"Communications",
													"Choix oralojon",
													"Choix Languedoc",
													"Servis"};

		const char StringsSubmenuSelezProgFran [5][20]={ "Choix Symbols",
														"TK",
														"Choix Curvon",
														"Choix Seuils",
														"Choix timeron"};




		const char StringsSubmenuImpostaSimboliFran [5][20]={"Symbole  %",
														"Symbole    P",
														"Symbole    g/l",
														"Symbole    uS",
														"Symbole    mS"};


		const char StringsSubmenuImpostaSoglieFran [8][20]={"Choix.Conc.",
														"All.Conc.Min",
														"All.Conc.Max",
														"Hysteresis.Conc",
														"Set Temp",
														"All. Temp.Min",
														"All. Temp.Max",
														"Hysteresis.Temp"};

		const char StringsSubmenuImpostaTimerFran [9][20]={	"T1 Rit.CH C",
														"T2 Dosage",
														"T3 Rit.Dos.",
														"T4 All.Min. C.",
														"T5 All.Max. C.",
														"T6 Rit.CH T.",
														"T7 Chauffage.",
														"T8 All.Min. T.",
														"T9 All.Max. T."};


		//anche per 2 stringhe strutturo così perchè poi ci saranno diverse lingue
		const char StringsSubmenuTipoCurvLavFran [2][20]={ 	"Calibration Centr",
															"Calibration L-C-H"
														};


		//		***ESPANOL***ESPANOL***ESPANOL***ESPANOL***ESPANOL***ESPANOL***ESPANOL***ESPANOL***

		const char StringsMenuProgEsp [6][20]={ 	"IN-OUT (ON-OFF)",
													"Selecc Program",
													"Communicacions",
													"Arregla reloj",
													"Languaje",
													"Servicio"};

		const char StringsSubmenuSelezProgEsp [5][20]={ "Selecc Symbols",
														"TK",
														"Selecc Curva",
														"Selecc Umbrales",
														"Selecc timer"};




		const char StringsSubmenuImpostaSimboliEsp [5][20]={"Simbolo  %",
														"Simbolo    P",
														"Simbolo    g/l",
														"Simbolo    uS",
														"Simbolo    mS"};


		const char StringsSubmenuImpostaSoglieEsp [8][20]={"Selecc.Conc.",
														"Al.Conc.Min",
														"Al.Conc.Max",
														"Histeresis.Conc",
														"Set Temp",
														"All. Temp.Min",
														"All. Temp.Max",
														"Histeresis.Temp"};

		const char StringsSubmenuImpostaTimerEsp [9][20]={	"T1 Rit.CH C",
														"T2 Dosific",
														"T3 Rit.Dos.",
														"T4 All.Min. C.",
														"T5 All.Max. C.",
														"T6 Rit.CH T.",
														"T7 Calefaccion.",
														"T8 All.Min. T.",
														"T9 All.Max. T."};


		//anche per 2 stringhe strutturo così perchè poi ci saranno diverse lingue
		const char StringsSubmenuTipoCurvLavEsp [2][20]={ 	"Calibraccion Centr",
															"Calibraccion L-C-H"
														};










