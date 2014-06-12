//============================================================================
//
//  MODULE NAME:           ReadConfigFile.cpp
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


#include <stdio.h>
#include <string.h>
#include <assert.h>
//#include <strstream>
#include <iostream>
#include "ReadCfgFile.h"

char lineBuf[MAX_LINE_LEN];
FILE * openCfgfile(char* pFilename)
{
	FILE *pFile=NULL;
	if(!(pFile=fopen(pFilename,"r")))
	{
 	 printf(">>fail to open file>%s\n\n",pFilename);
 	 return NULL;
	}
	printf(">> open file>%s\n",pFilename);
	return pFile;
}


int readLine(int line, _LINE_ITEMS* pLineItems)
{
	int i,n;
	int pos,offset;
	char* pbuf=lineBuf;
	for(i=0,n=0; i<MAX_LINE_LEN; n++) {
		for(;i<MAX_LINE_LEN; i++) {
			if(!isspace(pbuf[i])) break;
		}
		pos = i;
		for(;i<MAX_LINE_LEN; i++) {
			if(isspace(pbuf[i])) break;
		}
		offset = i-1;
		if (offset <= pos) break;
		memset(pLineItems[line].item[n].content,0,MAX_ITEM_LEN);
		memcpy(pLineItems[line].item[n].content, &pbuf[pos], MIN((offset-pos+1),MAX_ITEM_LEN));
		//printf("Get line[%d] item[%d]:%s\n",line,n, pLineItems[line].item[n].content);
	}
	return n;
}

_LINE_ITEMS* itemsptr()
{
	static _LINE_ITEMS* pLineItems = NULL;
	if (!pLineItems) {
		pLineItems = (_LINE_ITEMS*)calloc(sizeof(_LINE_ITEMS),MAX_LINE);
	}
	return pLineItems;
}

int readCfgItems(char* pfilename)
{
	FILE *fp=NULL;
	int line;
	memset(lineBuf,' ',MAX_LINE_LEN);
	_LINE_ITEMS* pItems = itemsptr();
	if (NULL == (fp = openCfgfile(pfilename))) {
		exit(-1);
	}
	//fseek(fp, 0, SEEK_SET);
	line = 0;
	while (NULL != fgets(lineBuf, MAX_LINE_LEN, fp)) {
		printf("readline %d:%s",line,lineBuf);
		if (readLine(line, pItems)) {
		    line++;
		}
		else {
			printf("this is a blank line,ignore\n");
		}
		memset(lineBuf,' ',MAX_LINE_LEN);
	}
	fclose(fp);	
	return (line>0?(line-1):0);
}


#if 0
int main(int argc, char ** argv)
{

	if(readCfgItems(CFG_FILE_PATH)){
		return 0;
	}
	//printf("capNum: %d\n", capNum);
	printf(">>Misson complete!\n");
	return 0;
}
#endif



