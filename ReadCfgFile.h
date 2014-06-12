//============================================================================
//
//  MODULE NAME:           ReadConfigFile.h
//
//  MODULE DESCRIPTION:
//
//   
//
//
//  SPECIAL CONSIDERATIONS:
//
//
//****************************************************************************
//
//  REVISION HISTORY:
//
//  DATE      			DESIGNER      				REASON         
//  -------- 		    ------------ 	  --------------------------------------
//  09/05/2011  		JiMeixiang         				 Original
//
//============================================================================
#ifndef _ReadCfgFile_H
#define _ReadCfgFile_H

#define MAX_LINE_LEN	512
#define  MAX_ITEM_LEN	32
#define MAX_ONEL_ITEM	5
#define MAX_LINE		5

typedef struct {
	char content[MAX_ITEM_LEN];
}_ITEM;
typedef struct {
//	int line;
	_ITEM item[MAX_ONEL_ITEM];
}_LINE_ITEMS;

#ifndef MIN
#define MIN(a, b)       ((a) < (b)? (a): (b))
#endif
#ifndef MAX
#define MAX(a, b)       ((a) > (b)? (a): (b))
#endif
extern int readCfgItems(char* pfilename);
extern _LINE_ITEMS* itemsptr();
#endif
