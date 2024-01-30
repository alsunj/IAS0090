#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "DateTime.h"
#include "Objects.h"
#include "Headers.h"
#include "Structs.h"


#define O 8
#define N 35

unsigned long int randomnr(int l, int r)
{
	return (rand() % (r - l + 1)) + l;
}
Object8* CreateObject8(char* pNewID, unsigned long int NewCode) {
	Object8* pObjectNew = (Object8*)malloc(sizeof(Object8));
	if (pObjectNew == NULL) {
		printf("Memory not allocated for a new object!");
		exit(EXIT_FAILURE);
	}

	pObjectNew->Code = NewCode;
	time_t RawTime = time(NULL);

	Date2 result;
	if (GetDate2(RawTime, &result) == 1) {
		pObjectNew->sDate2.Day = result.Day;
		if (strcpy_s(pObjectNew->sDate2.Month, sizeof(pObjectNew->sDate2.Month), result.Month) != 0) {
			printf("Error copying month to structure.");
			exit(EXIT_FAILURE);
		}
		pObjectNew->sDate2.Year = result.Year;
	}
	else {
		printf("Date not acquired!");
		exit(EXIT_FAILURE);
	}

	pObjectNew->pID = (char*)malloc(strlen(pNewID) + 1);
	if (strcpy_s(pObjectNew->pID, strlen(pNewID) + 1, pNewID) != 0) {
		printf("Error copying ID to structure.");
		exit(EXIT_FAILURE);
	}

	pObjectNew->pNext = NULL;

	return pObjectNew;
}

HeaderB* CreateHeaderB(char* pNewID, HeaderB* pStructNext) {
	HeaderB* pStructNew = (HeaderB*)malloc(sizeof(HeaderB));
	if (pStructNew == NULL) {
		printf("Memory for headerB not allocated!");
		exit(EXIT_FAILURE);
	}

	pStructNew->pHeaderA = NULL;
	pStructNew->pNext = pStructNext;
	pStructNew->cBegin = *pNewID;

	return pStructNew;
}

HeaderA* CreateHeaderA(char* pNewID, HeaderA* pStructNext) {
	HeaderA* pStructNew = (HeaderA*)malloc(sizeof(HeaderA));
	if (pStructNew == NULL) {
		printf("Memory for headerA not allocated!");
		exit(EXIT_FAILURE);
	}

	pStructNew->pNext = pStructNext;
	pStructNew->cBegin = *pNewID;
	pStructNew->pObject = NULL;

	return pStructNew;
}

bool ObjectExists(HeaderB* pHeaderB, char* pNewID) {
	HeaderA* pHeaderA = pHeaderB->pHeaderA;

	while (pHeaderA) {
		Object8* pObject8 = (Object8*)pHeaderA->pObject;

		while (pObject8) {
			if (strcmp(pObject8->pID, pNewID) == 0) {
				return true;
			}

			pObject8 = pObject8->pNext;
		}

		pHeaderA = pHeaderA->pNext;
	}

	return false;
}

int InsertNewObject(HeaderB** pStruct3, char* pNewID, int NewCode) {
	HeaderB* pTemp = NULL;
	HeaderA* pTempA = NULL;
	Object8* pTempObj;

	if (pNewID == NULL || pNewID[0] == '\0') {
		puts("String empty\n");
		return 0;
	}

	int i;
	int spaceCount = 0;
	int firstWord = 0;
	int secondWord = 0;
	bool space = false;

	if (!isupper(pNewID[0])) {
		return 0;
	}   // kui leitakse kohe, et esitäht pole suur, siis pole mõtet süsteemi ressursi kasutada

	for (i = 1; pNewID[i] != '\0'; i++) {
		if (isalpha(pNewID[i]) || (pNewID[i] == ' ')) {
			if (isupper(pNewID[i]) && !secondWord) {
				return 0;
			}
			else if (pNewID[i] == ' ') {  // mõlemad järgnevad funktsioonid kindlustavad, et on ainult üks tühik mõlema sõna vahel
				spaceCount++;
				if (spaceCount > 1) {
					return 0;  // võtta see tingimus ära, et saaks salvestada rohkem kui ühe sõnaga ID-sid ja testida lahutamist (ei tohiks lahutada)
				}
				space = true;
				if (isupper(pNewID[i + 1])) {
					secondWord = i + 1;         //salvesta teise sõna esisuurtäht
					i++;
				}
				else {
					return 0;
				}
			}
		}
		else {
			return 0;
		}
	}

	if (space != 1) {       // vajalik selleks, et ühesõnalised ID-d ei salvestuks
		return 0;
	}


	i = 0;

	for (pTemp = *pStruct3; pTemp; pTemp = pTemp->pNext) { //käime läbi HeaderB
		i++;
		if (pTemp == NULL) {								//juhul kui Nullväärtuse pTemp
			pTemp = CreateHeaderB(&pNewID[firstWord], NULL);
			break;
		}
		else if (pNewID[firstWord] == pTemp->cBegin) {  //Leidis õige järjekorra
			break;
		}
		else if (pTemp->cBegin > pNewID[firstWord]) {  // juhul kui esimene täht on suurem
			pTemp = CreateHeaderB(&pNewID[firstWord], pTemp);
			if (i == 1) {
				*pStruct3 = pTemp;
			}
			break;
		}	
		else if (pTemp->pNext == NULL) {									//Juhul kui järgnevat objekti pole
			pTemp->pNext = CreateHeaderB(&pNewID[firstWord], NULL);
			pTemp = pTemp->pNext;
			break;
		}
		else if (pTemp->pNext->cBegin > pNewID[firstWord]) {				//juhul kui on viimane objekt
			pTemp->pNext = CreateHeaderB(&pNewID[firstWord], pTemp->pNext);
			pTemp = pTemp->pNext;
			break;
		}
	}

	if (pTemp->pHeaderA == NULL) {			//juhul kui Nullväärtuse pTemp 
		pTemp->pHeaderA = CreateHeaderA(&pNewID[secondWord], NULL); 
	}

	i = 0;

	for (pTempA = pTemp->pHeaderA; pTempA; pTempA = pTempA->pNext) { //käime läbi HeaderA
		i++;
		if (pNewID[secondWord] == pTempA->cBegin) {					//Leidis õige järjekorra
			break;
		}
		else if (pTempA->cBegin > pNewID[secondWord]) {				//esitäht on väiksem kui praeguse HeaderA ID oma
			pTempA = CreateHeaderA(&pNewID[secondWord], pTempA);	
			if (i == 1) {
				pTemp->pHeaderA = pTempA;
			}
			break;
		}
		else if (pTempA->pNext == NULL) {								//Juhul kui järgnevat objekti pole
			pTempA->pNext = CreateHeaderA(&pNewID[secondWord], NULL);
			pTempA = pTempA->pNext;
			break;
		}
		else if (pTempA->pNext->cBegin > pNewID[secondWord]) {				//juhul kui on viimane objekt
			pTempA->pNext = CreateHeaderA(&pNewID[secondWord], pTempA->pNext);
			pTempA = pTempA->pNext;
			break;
		}
	}

	if (pTempA->pObject == NULL) {	//juhul kui HeaderA-s pole objekte
		pTempA->pObject = CreateObject8(pNewID, NewCode);
		goto skip;
	}

	for (pTempObj = (Object8*)pTempA->pObject; pTempObj; pTempObj = pTempObj->pNext) { //vaatan kas sama id eksisteerib
		if (strcmp(pTempObj->pID, pNewID) == 0) {
			return 0;
		}
		else if (pTempObj->pNext == NULL) {
			pTempObj->pNext = CreateObject8(pNewID, NewCode);				//loon objekti
			break;
		}
	}

skip:
	return 1;
}
Object8* RemoveExistingObject(HeaderB** pStruct3, char* pExistingID) {
	HeaderB* pTemp = NULL;
	HeaderA* pTempA = NULL;

	HeaderA* pPrevA = NULL;
	HeaderB* pPrevB = NULL;

	Object8* pTempObj = NULL;
	Object8* pPrevObj = NULL;

	if (pExistingID == NULL || pExistingID[0] == '\0') { //kontrollin kas kaasa anti uus ID
		puts("String empty\n");
		return NULL; 
	}

	int i;
	int spaceCount = 0;
	int firstWord = 0;
	int secondWord = 0;
	bool space = false;

	if (!isupper(pExistingID[0])) {
		return 0;
	}   // kui leitakse kohe, et esitäht pole suur, siis pole mõtet süsteemi ressursi kasutada

	for (i = 1; pExistingID[i] != '\0'; i++) {
		if (isalpha(pExistingID[i]) || (pExistingID[i] == ' ')) {
			if (isupper(pExistingID[i]) && !secondWord) {
				return 0;
			}
			else if (pExistingID[i] == ' ') {  // mõlemad järgnevad funktsioonid kindlustavad, et on ainult üks tühik mõlema sõna vahel
				spaceCount++;
				if (spaceCount > 1) {
					return 0;  
				}
				space = true;
				if (isupper(pExistingID[i + 1])) {
					secondWord = i + 1;         //salvesta teise sõna esisuurtäht
					i++;
				}
				else {
					return 0;
				}
			}
		}
		else {
			return 0;
		}
	}

	if (space != 1) {       // vajalik selleks, et ühesõnalised ID-d ei salvestuks
		return 0;
	}
	if (pStruct3 == NULL) {
		// No HeaderB
		puts("No HeaderB in memory!");
		return NULL;
	}

	i = 0;
	bool HeaderBFirst = false;

	// Find the correct HeaderB
	for (pTemp = *pStruct3; pTemp; pTemp = pTemp->pNext) {
		i++;
		if (pExistingID[firstWord] == pTemp->cBegin) {  // leian algustähega HeaderB
			if (i == 1) {
				HeaderBFirst = true;
			}
			break;
		}
		else if (pTemp->pNext == NULL) {				// ei leidnud õiget
			puts("No HeaderB found where to delete!");
			return NULL;
		}
		pPrevB = pTemp;									//salvestan õige tähega HeaderB viite
	}

	
	if (pTemp->pHeaderA == NULL) {					
		puts("No HeaderA exists in HeaderB!");
		return NULL;
	}

	bool HeaderAFirst = false;
	i = 0;

	// Find the correct HeaderA
	for (pTempA = pTemp->pHeaderA; pTempA; pTempA = pTempA->pNext) {		//Käin läbi HeaderA-st
		i++;
		if (pExistingID[secondWord] == pTempA->cBegin) {			//leian sama algustähega ID
			if (i == 1) {
				HeaderAFirst = true;							
			}
			break;
		}
		else if (pTempA->pNext == NULL) {						//ei leidnud
			puts("No correct HeaderA found!");
			return NULL;
		}
		pPrevA = pTempA;												//salvestan õige tähega HeaderA viite
	}

	i = 0;

	if (pTempA->pObject == NULL) {										
		puts("Given HeaderA does not have any objects inside!");
		return NULL;
	}

	for (pTempObj = (Object8*)pTempA->pObject; pTempObj; pTempObj = pTempObj->pNext) {	//käin läbi kõik objektid
		i++;
		if (!strcmp(pTempObj->pID, pExistingID)) {										//vaatan üle kas järgmise objekti ID on sama
			if (i == 1) {															
				if (pTempObj->pNext == NULL){											//kontrollib, et ei oleks järgmist 
					if (HeaderAFirst == true) {											//kontrollib, et HeaderA oleks olemas
						if (pTempA->pNext == NULL) {									//kontrollib, et ei oleks järgmist
							if (HeaderBFirst == true) {									//kontrollib, et HeaderB oleks olemas
								*pStruct3 = pTemp->pNext;								//annab NULL pointeri väärtuse
							}
							else {
								pPrevB->pNext = pTemp->pNext;							//kui ei ole HeaderBFirsti
							}
						}
						else {
							pTemp->pHeaderA = pTempA->pNext;						//kui on veel HeaderA elemente
						}
					}
					else {
						pPrevA->pNext = pTempA->pNext;							//kui on veel HeaderA elemente
					}
				}
				else {
					pTempA->pObject = pTempObj->pNext;							//kui on veel objekte
				}
			}
			else {
				pPrevObj->pNext = pTempObj->pNext;								//ei ole esimene listise 
			}
			break;
		}
		else if (pTempObj->pNext == NULL) {											//ei leia objekti
			puts("Object to delete not found!");
			return NULL;
		}
		pPrevObj = pTempObj;
	}

	return pTempObj;																//tagastame ajutise pointeri objektile
}

void PrintObjects(HeaderB* pStruct3) {
	int i = 0, j = 0;
	HeaderB* pStructTemp = pStruct3;			//viit pStruct3le

	while (pStructTemp != NULL) {
		printf("\n[HdrB %d] %c\n", i + 1, pStructTemp->cBegin);

		HeaderA* pHeaderATemp = pStructTemp->pHeaderA;			//Viit HeaderB-s olevale HeaderA-le
		

		while (pHeaderATemp != NULL) {
			Object8* pObjectTemp = (Object8*)pHeaderATemp->pObject;				//viit objektile
			printf("(Obj %d) %s %lu %02d %s %04d\n", j + 1, pObjectTemp->pID, pObjectTemp->Code,
				pObjectTemp->sDate2.Day, pObjectTemp->sDate2.Month, pObjectTemp->sDate2.Year);
			pHeaderATemp = pHeaderATemp->pNext;				//järgmine objekt
			j++;
		}

		pStructTemp = pStructTemp->pNext;
		i++;
	}
}





/*																						Teine osa																						 */	
/*																						Teine osa																						 */
/*																						Teine osa																						 */
/*																						Teine osa																						 */
/*																						Teine osa																						 */
/*																						Teine osa																						 */


 






Node* CreateBinaryTree(HeaderB* pStruct3)
{

	int i = 1;
	Node* pTree = NULL;
	HeaderB* pTemp;
	HeaderA* pTempA;

	Object8* pTempObj;
	Node* pNew = NULL;

	if (pStruct3) //Check if Header exists
	{
		for (pTemp = pStruct3; pTemp; pTemp = pTemp->pNext) //Go through HeaderB 
		{

			if (pTemp->pHeaderA) //Check if HeaderA exists in HeaderB
			{

				for (pTempA = pTemp->pHeaderA; pTempA; pTempA = pTempA->pNext) //Go through HeaderA in HeaderB
				{

					if (pTempA->pObject)
					{

						for (pTempObj = (Object8*)pTempA->pObject; pTempObj; pTempObj = pTempObj->pNext) //Go through objects in HeaderA
						{
							if (i == 1)
							{
								pTree = (Node*)malloc(sizeof(Node)); //Uus tipp
								pTree->pObject = pTempObj;
								pTree->pLeft = pTree->pRight = NULL;
							}
							else
							{
								pNew = (Node*)malloc(sizeof(Node)); //Uus tipp
								pNew->pObject = pTempObj;
								pNew->pLeft = pNew->pRight = NULL;
							}


							for (Node* p = pTree; p != NULL; )
							{
								if (p->pObject->Code > pTempObj->Code) //Compares current node to the node we want to add
								{
									if (!p->pLeft) //Found empty spot
									{
										p->pLeft = pNew;
										break;
									}
									else
									{
										if (p->pLeft == NULL)
										{
											break;
										}
										p = p->pLeft; //Move left, since no free spot
									}
								}
								else
								{
									if (!p->pRight) //Found empty spot
									{
										p->pRight = pNew;
										break;
									}
									else
									{
										if (p->pRight == NULL)
										{
											break;
										}
										p = p->pRight;// Move right, since no free spot

									}
								}
							}
							i++;
						}
					}
				}
			}
		}

	}
	else //Provided list is empty
	{
		fprintf(stderr, "Provided Header is empty!");
	}
	return pTree;

}


Stack* Push(Stack* pStack, void* pRecord)
{
	Stack* pNew;
	if (!pRecord)
	{
		return pStack;
	}
	pNew = (Stack*)malloc(sizeof(Stack));
	if (pNew == NULL)
	{
		printf("Push(): malloc failed!");
		exit(EXIT_FAILURE);
	}
	pNew->pObject = pRecord;
	pNew->pNext = pStack;
	return pNew;
}

Stack* Pop(Stack* pStack, void** pResult)
{
	Stack* p;
	if (!pStack)
	{
		*pResult = 0;
		return pStack;
	}
	*pResult = (Object8*)pStack->pObject;
	p = pStack->pNext;
	free(pStack);
	return p;
}

void ProcessNode(Node* pNode)
{
	printf("%s  %lu\n", ((Object8*)pNode->pObject)->pID, ((Object8*)pNode->pObject)->Code);
}

void TreeTraversal(Node* pTree, void(*pProcess)(Node*))
{
	printf("\n\n\n--------Printing binary tree:----------\n\n\n");
	int i = 1;
	Stack* pStack = 0;
	Node* p1 = pTree, * p2;
	if (!pTree)
		return;
	do
	{
		while (p1)
		{
			pStack = Push(pStack, p1);
			p1 = p1->pLeft;
		}
		pStack = Pop(pStack, (void**)&p2);
		printf("Node %d: ", i);
		i++;
		(pProcess)(p2);
		p1 = p2->pRight;
	} while (!(!pStack && !p1));
}


Node* DeleteTreeNodeNone(Node* root, Node* par, Node* ptr)
{
	if (par == NULL) /*root node to be deleted*/
	{
		root = NULL;
	}
	else if (ptr == par->pLeft)
	{
		par->pLeft = NULL;
	}
	else
	{
		par->pRight = NULL;
	}
	free(ptr);
	return root;
}

Node* DeleteTreeNodeOne(Node* root, Node* par, Node* ptr)
{
	Node* child;

	if (ptr->pLeft != NULL) // node has left child
	{
		child = ptr->pLeft;
	}
	else                // node has right child 
	{
		child = ptr->pRight;
	}

	if (par == NULL)   // node is root node
	{
		root = child;
	}
	else if (ptr == par->pLeft) // node is left child of its parent
	{
		par->pLeft = child;
	}
	else                  // node is right child of its parent
	{
		par->pRight = child;
	}
	free(ptr);
	return root;
}

Node* DeleteTreeNodeBoth(Node* root, Node* par, Node* ptr)
{
	Node* successor, * parentSuccessor;

	// Find successor and its parent
	parentSuccessor = ptr;
	successor = ptr->pRight;
	while (successor->pLeft != NULL)
	{
		parentSuccessor = successor;
		successor = successor->pLeft;
	}

	ptr->pObject = successor->pObject;

	if (successor->pLeft == NULL && successor->pRight == NULL)
	{
		root = DeleteTreeNodeNone(root, parentSuccessor, successor);
	}
	else
	{
		root = DeleteTreeNodeOne(root, parentSuccessor, successor);
	}
	return root;
}


Node* DeleteTreeNode(Node* pTree, unsigned long int Code)
{
	printf("\nDeleting %d\n", Code);
	Node* par, * ptr;

	ptr = pTree;
	par = NULL;
	while (ptr != NULL)
	{

		if (Code == ((Object4*)ptr->pObject)->Code)
		{
			break;
		}
		par = ptr;
		if (Code < ((Object4*)ptr->pObject)->Code)
		{
			ptr = ptr->pLeft;
		}
		else
		{
			ptr = ptr->pRight;
		}
	}
	if (ptr == NULL)
	{
		printf("Key not present in tree\n");
	}
	else if (ptr->pLeft != NULL && ptr->pRight != NULL) // 2 children
	{
		pTree = DeleteTreeNodeBoth(pTree, par, ptr);
	}
	else if (ptr->pLeft != NULL) // Only left child 
	{
		pTree = DeleteTreeNodeOne(pTree, par, ptr);
	}
	else if (ptr->pRight != NULL) // Only right child
	{
		pTree = DeleteTreeNodeOne(pTree, par, ptr);
	}
	else // No child
	{
		pTree = DeleteTreeNodeNone(pTree, par, ptr);
	}

	return pTree;
}

int main() {

	
	HeaderB* pStruct3 = GetStruct3(O, N);
	
	printf("---------------------------ULESANNE 1----------------------------------------\n");
	PrintObjects(pStruct3);
	printf("---------------------------ULESANNE 2----------------------------------------\n");

	//const char* strings[] = { "Ax Tz", "Gr Ge", "Da Fd", "mK Mn", "3e Ta", "Aa Bb Ff", "Bb", "Tg_K-k", "Fa AR", "Aj_Kl", "Uf Rt","Mt Mn", "fe nv", "Wf De", "Fr Io", "Me Jw", "Am", "Gf rE", "Aaa Peso", "Baa Beso Bee", "Boo 7" , "1", "Pscl Lmx" };
	const char* strings[] = { "Dx Gz", "Dx Ga", "Db Aa", "Dk Za", "Dr Wa", "Aa Aa", "Ab Ba", "Za Aa", "Za Ab", "Za Ba", "Wx Xa", "Wx Aa", "zb Kk", "Zc ca", "Dr Wa", "ZB kk", "Fa", "Fa_Fa" };
	int m = sizeof(strings) / sizeof(strings[0]);
	printf("Sisestada järgnevad %d Id-d d:\n\n", m);

	HeaderB** ppStruct3 = &pStruct3;

	for (int i = 0; i < m; i++) {
		if (InsertNewObject(ppStruct3, (char*)strings[i], randomnr(0, 100000)) == 0) {
			printf("ei saanud lisada: %s\n", strings[i]);
		}
		else {
			printf("lisatud ID: %s \n", strings[i]);
		}
	}


	PrintObjects(pStruct3);
	printf("---------------------------ULESANNE 3----------------------------------------\n");

	Object8* RemoveObjects;
	for (int i = 0; i < m; i++) {
		RemoveObjects = RemoveExistingObject(ppStruct3, (char*)strings[i]);

		if (RemoveObjects == NULL) {
			printf("Ei saanud Eemaldada ID: %s\n", strings[i]);
		}
		else {
			printf("Eemaldatud ID: %s\n", strings[i]);
			free(RemoveObjects); //vabastab mälu
		}
	}

	PrintObjects(pStruct3);
	
	printf("-------------------------------TEINE OSA-------------------------------\n");
	int juuretipp = 316985719;
	pStruct3 = GetStruct3(O, N);
	PrintObjects(pStruct3);
	Node* pBinaryTree = CreateBinaryTree(pStruct3);
	TreeTraversal(pBinaryTree, ProcessNode);
	printf("\n\n\n--------Eemaldan juuretipu:----------\n\n\n");
	pBinaryTree = DeleteTreeNode(pBinaryTree, juuretipp);
	TreeTraversal(pBinaryTree, ProcessNode);
	pStruct3 = GetStruct3(O, 10);
	PrintObjects(pStruct3);
	// 7. Moodustada kahendpuu ja käies läbi kõik tema tipud väljastada tippude juurde kuuluvad objektid.
	pBinaryTree = CreateBinaryTree(pStruct3);
	TreeTraversal(pBinaryTree, ProcessNode);
	// 9. Eemaldada kahendpuust juhendaja näidatud võtmetega tipud ja käies läbi kõik uue puu tipud väljastada tippude juurde kuuluvad objektid.
	printf("\n\n\n--------Eemaldan tipu:----------\n\n\n");
	pBinaryTree = DeleteTreeNode(pBinaryTree, 36666300);  // 422218 4455511 16533168 36666300 64866018 101110100 145409657 197775800 258186307 326652289
	TreeTraversal(pBinaryTree, ProcessNode);
	// 10. Teha katse eemaldada tipp, mida ei olegi olemas. 
	pBinaryTree = DeleteTreeNode(pBinaryTree, 258186307);
	TreeTraversal(pBinaryTree, ProcessNode);
	

	return 0;
}
