#include <stdio.h>
#include <conio2.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

struct TpProduto    //32 B
{
    int CodProd, Estoque;
    char Descr[20];
    float Preco;
};

int BuscaSeqInd(FILE *Ptr,int codigo){
	TpProduto RP;
	rewind(Ptr);
	fread(&RP,sizeof(TpProduto),1,Ptr);
	while(!feof(Ptr) && codigo>RP.CodProd){
		fread(&RP,sizeof(TpProduto),1,Ptr);
	}
	if(!feof(Ptr) && codigo==RP.CodProd)
		return ftell(Ptr)-sizeof(TpProduto);
	else return -1;
}

void InsertionSort(FILE *Ptr){
	TpProduto Aux,RP[2];
	int ind=(ftell(Ptr)/sizeof(TpProduto))-2;
	if(ind>=0){
		fseek(Ptr,ind*sizeof(TpProduto),SEEK_SET);
		fread(&RP,sizeof(TpProduto),2,Ptr);
		while(ind>=0 && RP[1].CodProd<RP[0].CodProd){
			Aux=RP[0];
			RP[0]=RP[1];
			RP[1]=Aux;
			fseek(Ptr,ind*sizeof(TpProduto),SEEK_SET);
			fwrite(&RP,sizeof(TpProduto),2,Ptr);
			ind--;
			if(ind>=0){
				fseek(Ptr,ind*sizeof(TpProduto),SEEK_SET);
				fread(&RP,sizeof(TpProduto),2,Ptr);
			}
		}
	}
}

int BuscaBinaria(FILE *Ptr,int codigo){
	TpProduto RP;
	fseek(Ptr,0,SEEK_END);
	int I=0,F=(ftell(Ptr)/sizeof(TpProduto))-1,M=(I+F)/2;
	
	fseek(Ptr,M*sizeof(TpProduto),SEEK_SET);
	fread(&RP,sizeof(TpProduto),1,Ptr);
	while(I<=F && codigo!=RP.CodProd){
		if(codigo>RP.CodProd)
			I=M+1;
		else F=M-1;
		
		if(I<=F){
			M=(F+I)/2;
			fseek(Ptr,M*sizeof(TpProduto),SEEK_SET);
			fread(&RP,sizeof(TpProduto),1,Ptr);
		}
	}
	if(codigo==RP.CodProd)
		return M*sizeof(TpProduto);
	else return -1;
}

void CadastroProd(void){
	TpProduto Prod;
	FILE *PtrProd = fopen("Produtos.dat","rb+");
	if(PtrProd == NULL)
		PtrProd = fopen("Produtos.dat","wb+");
		
	clrscr();
	printf("*** Cadastro de Produtos ***");
	printf("Insira o codigo do produto: ");
	scanf("%d",&Prod.CodProd);
	while(Prod.CodProd > 0){
		if(BuscaSeqInd(PtrProd,Prod.CodProd) == -1){
			printf("Descricao: "); fflush(stdin);
			gets(Prod.Descr);
			printf("Preco: R$");
			scanf("%f",&Prod.Preco);
			printf("Estoque: ");
			scanf("%d",&Prod.Estoque);
			
			fseek(PtrProd,0,SEEK_END);
			fwrite(&Prod,sizeof(TpProduto),1,PtrProd);
			InsertionSort(PtrProd);
		}
		else
			printf("Codigo ja foi cadastrado!");
		
		printf("Insira o codigo do produto: ");
		scanf("%d",&Prod.CodProd);
	}
	fclose(PtrProd);
}

void ConsultaProd(void){
	TpProduto RP;
	int pos;
	FILE *Ptr = fopen("Produtos.dat","rb");
	clrscr();
	fseek(Ptr,0,SEEK_END);
	if(!ftell(Ptr))
		printf("Nenhum Produto foi Cadastrado!\n");
	else{
		printf("*** Consulta Produto ***");
		printf("Insira o codigo: ");
		scanf("%d",&RP.CodProd);
		while(RP.CodProd > 0){
			pos=BuscaBinaria(Ptr,RP.CodProd);
			if(pos == -1)
				printf("Produto nao encontrado!");
			else{
				fseek(Ptr,pos,SEEK_SET);
				fread(&RP,sizeof(TpProduto),1,Ptr);
				printf("Codigo: %d\t Estoque: %d\t Preco: %f\nDescricao: %s\n",RP.CodProd,RP.Estoque,RP.Preco,RP.Descr);
			}
			
			getch();
			clrscr();
			printf("*** Consulta Produto ***");
			printf("Insira o codigo: ");
			scanf("%d",&RP.CodProd);
		}
	}
	fclose(Ptr);	
}
int BuscaSentinela(FILE *Ptr, char Descr[],char nomearq[])
{
	TpProduto RP;
	int pos,TL;
	fseek(Ptr,0,2);
	TL = ftell(Ptr);
	strcpy(RP.Descr,Descr);
	fwrite(&RP,sizeof(RP),1,Ptr);
	fseek(Ptr,0,0);
	fread(&RP,sizeof(RP),1,Ptr);
	while(strcmp(Descr,RP.Descr)!= 0)
		fread(&RP,sizeof(RP),1,Ptr);
	pos = ftell(Ptr) - sizeof(RP);
	
	FILE *aux = fopen("aux.dat","wb+");
	fseek(Ptr,0,0);
	while(ftell(Ptr) != TL)
	{
		fread(&RP,sizeof(RP),1,Ptr);
		fwrite(&RP,sizeof(RP),1,aux);
	}
	fclose(aux);
	fclose(Ptr);
	remove(nomearq);
	rename("aux.dat",nomearq);
	if(pos < TL)
		return pos;
	else
		return -1;
}
void SelecaoDireta(FILE *Ptr,FILE *aux)
{
	TpProduto RP,arm[2];
	int posmaior,TL;
	fseek(Ptr,0,2);
	TL = ftell(Ptr);
	fseek(Ptr,0,0);
	while(ftell(Ptr) != TL)
	{
		fread(&RP,sizeof(RP),1,Ptr);
		fwrite(&RP,sizeof(RP),1,aux);			
	}
	while(TL>0)
	{
		posmaior = MaiorPos(aux,TL);
		if(posmaior<TL-sizeof(RP))
		{
			fseek(aux,posmaior,0);
			fread(&arm[0],sizeof(arm[0]),1,aux);
			fseek(aux,TL-sizeof(RP),0);
			fread(&arm[1],sizeof(arm[1]),1,aux); 
			fseek(aux,posmaior,0);
			fwrite(&arm[1],sizeof(arm[1]),1,aux);
			fseek(aux,TL-sizeof(RP),0);
			fwrite(&arm[0],sizeof(arm[0]),1,aux);
		}
		TL = TL - sizeof(RP);
	}
}
int MaiorPos(FILE *Ptr,int TL)
{
	TpProduto RP;
	int posmaior;
	float maior;
	fseek(Ptr,0,0);
	fread(&RP,sizeof(RP),1,Ptr);
	maior = RP.Preco;
	posmaior = 0;
	while(ftell(Ptr) < TL)
	{
		fread(&RP,sizeof(RP),1,Ptr);
		if(maior<RP.Preco)
		{
			maior = RP.Preco;
			posmaior = ftell(Ptr) - sizeof(RP);
		}
	}
	return posmaior;
}
void RelatorioPreco(FILE *Ptr)
{
	FILE *aux = fopen("aux.dat","wb+");
	SelecaoDireta(Ptr,aux);
	TpProduto RP;
	fseek(aux,0,2);
	int TL = ftell(aux);
	while(TL>0)
	{
		fseek(aux,TL - sizeof(RP),0);
		fread(&RP,sizeof(RP),1,aux);
		printf("Codigo: %d\tEstoque: %d\nDescricao: %s\nPreco: %.2f",RP.CodProd,RP.Estoque,RP.Descr,RP.Preco);
		TL = TL - sizeof(RP);
	}
	fclose(aux);
}
void BubbleSort(FILE *Ptr, FILE *aux2)
{
    TpProduto RA,RB,RP;
    int a,b,qtd;
    fseek(Ptr,0,2);
    qtd=ftell(Ptr)/sizeof(TpProduto);
    rewind(Ptr);
    while(ftell(Ptr) < qtd*sizeof(TpProduto))
    {
        fread(&RP,sizeof(RP),1,Ptr);
        fwrite(&RP,sizeof(RP),1,aux2);
    }
    for(a=0;a<qtd-1;a++)
    {
        for(b=0;b<qtd-1-a;b++)
        {
            fseek(aux2,b*sizeof(TpProduto),0);
            fread(&RA,sizeof(TpProduto),1,aux2);
            fread(&RB,sizeof(TpProduto),1,aux2);

            if(RA.Estoque>RB.Estoque)
            {
                fseek(aux2,b*sizeof(TpProduto),0);
                fwrite(&RB,sizeof(TpProduto),1,aux2);
                fwrite(&RA,sizeof(TpProduto),1,aux2);
            }
        }
    }
}
void RelatorioEstoque(FILE *Ptr)
{
	FILE *aux2 = fopen("aux2.dat","wb+");
	BubbleSort(Ptr,aux2);
	TpProduto RP;
	fseek(aux2,0,2);
	int TL = ftell(aux2);
	rewind(aux2);
	while(TL>0)
	{
		fread(&RP,sizeof(RP),1,aux2);
		printf("Codigo: %d\tEstoque: %d\nDescricao: %s\nPreco: %.2f",RP.CodProd,RP.Estoque,RP.Descr,RP.Preco);
		TL = TL - sizeof(RP);
	}
	fclose(aux2);
}
int MaiorPosDescr(FILE *Ptr,int TL)
{
	TpProduto RP;
	int posmaior;
	char maior[30];
	fseek(Ptr,0,0);
	fread(&RP,sizeof(RP),1,Ptr);
	strcpy(maior,RP.Descr);
	posmaior = 0;
	while(ftell(Ptr) < TL)
	{
		fread(&RP,sizeof(RP),1,Ptr);
		if(stricmp(maior,RP.Descr)<0)
		{
			strcpy(maior,RP.Descr);
			posmaior = ftell(Ptr) - sizeof(RP);
		}
	}
	return posmaior;
}
void SelecaoDiretaDescr(FILE *Ptr,FILE *aux2)
{
	TpProduto RP,arm[2];
	int posmaior,TL;
	fseek(Ptr,0,2);
	TL = ftell(Ptr);
	fseek(Ptr,0,0);
	while(ftell(Ptr) != TL)
	{
		fread(&RP,sizeof(RP),1,Ptr);
		fwrite(&RP,sizeof(RP),1,aux2);			
	}
	while(TL>0)
	{
		posmaior = MaiorPosDescr(aux2,TL);
		if(posmaior<TL-sizeof(RP))
		{
			fseek(aux2,posmaior,0);
			fread(&arm[0],sizeof(arm[0]),1,aux2);
			fseek(aux2,TL-sizeof(RP),0);
			fread(&arm[1],sizeof(arm[1]),1,aux2); 
			fseek(aux2,posmaior,0);
			fwrite(&arm[1],sizeof(arm[1]),1,aux2);
			fseek(aux2,TL-sizeof(RP),0);
			fwrite(&arm[0],sizeof(arm[0]),1,aux2);
		}
		TL = TL - sizeof(RP);
	}
}
void RelatorioDescr(FILE *Ptr)
{
	FILE *aux2 = fopen("aux2.dat","wb+");
	SelecaoDiretaDescr(Ptr,aux2);
	TpProduto RP;
	fseek(aux2,0,2);
	int TL = ftell(aux2);
	while(TL>0)
	{
		fseek(aux2,TL - sizeof(RP),0);
		fread(&RP,sizeof(RP),1,aux2);
		printf("Codigo: %d\tEstoque: %d\nDescricao: %s\nPreco: %.2f",RP.CodProd,RP.Estoque,RP.Descr,RP.Preco);
		TL = TL - sizeof(RP);
	}
	fclose(aux2);
}
