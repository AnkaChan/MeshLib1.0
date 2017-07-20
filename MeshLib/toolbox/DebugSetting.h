#pragma once
#define SHOWDIALOG

#ifdef  NDEBUG
	#define ShowInDebug(EXPRESION) 
#else
	#define ShowInDebug(EXPRESION) EXPRESION
#endif 


#ifdef  SHOWDIALOG
	#define ShowDialog(EXPRESION) EXPRESION
#else
	#define ShowDialog(EXPRESION) 
#endif 