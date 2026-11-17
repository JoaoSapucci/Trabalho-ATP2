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
void ConsultaDescr(void);
void RelatorioDescr(void);
void RelatorioEstoque(void);
void BubbleSort(FILE *Ptr, FILE *aux2);
void RelatorioPreco(void);
int MaiorPos(FILE *Ptr,int TL,int modo);
void SelecaoDireta(FILE *Ptr,FILE *aux,int modo);
int BuscaSentinela(FILE *Ptr, char Descr[],char nomearq[]);
void ConsultaProd(void);
void CadastroProd(void);
int BuscaBinaria(FILE *Ptr,int codigo);
void InsertionSort(FILE *Ptr);
int BuscaSeqInd(FILE *Ptr,int codigo);

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
	if(!ftell(Ptr) || Ptr == NULL )
		printf("Nenhum Produto foi Cadastrado ou Arquivo Inexistente!\n");
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
void SelecaoDireta(FILE *Ptr,FILE *aux,int modo)
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
		posmaior = MaiorPos(aux,TL,modo);
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
int MaiorPos(FILE *Ptr,int TL,int modo)
{
	TpProduto RP;
	int posmaior;
	fseek(Ptr,0,0);
	fread(&RP,sizeof(RP),1,Ptr);
	posmaior = 0;
	if(modo == 0)
	{
	
		float maior;
		maior = RP.Preco;
		while(ftell(Ptr) < TL)
		{
			fread(&RP,sizeof(RP),1,Ptr);
			if(maior<RP.Preco)
			{
				maior = RP.Preco;
				posmaior = ftell(Ptr) - sizeof(RP);
			}
		}
	}
	else
	{
	
		char maior[30];
		strcpy(maior,RP.Descr);
		while(ftell(Ptr) < TL)
		{
			fread(&RP,sizeof(RP),1,Ptr);
			if(stricmp(maior,RP.Descr)<0)
			{
				strcpy(maior,RP.Descr);
				posmaior = ftell(Ptr) - sizeof(RP);
			}
		}
	
	}
	return posmaior;
}
void RelatorioPreco(void)
{
	FILE *Ptr = fopen("Produtos.dat","rb+");
	if(!ftell(Ptr) || Ptr == NULL )
		printf("Nenhum Produto foi Cadastrado ou Arquivo Inexistente!\n");
	else
	{
		FILE *aux = fopen("aux.dat","wb+");
		SelecaoDireta(Ptr,aux,0);
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
	fclose(Ptr);
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
void RelatorioEstoque(void)
{
	FILE *Ptr = fopen("Produtos.dat","rb+");
	if(!ftell(Ptr) || Ptr == NULL )
		printf("Nenhum Produto foi Cadastrado ou Arquivo Inexistente!\n");
	else
	{
		FILE *aux = fopen("aux.dat","wb+");
		BubbleSort(Ptr,aux);
		TpProduto RP;
		fseek(aux,0,2);
		int TL = ftell(aux);
		rewind(aux);
		while(TL>0)
		{
			fread(&RP,sizeof(RP),1,aux);
			printf("Codigo: %d\tEstoque: %d\nDescricao: %s\nPreco: %.2f",RP.CodProd,RP.Estoque,RP.Descr,RP.Preco);
			TL = TL - sizeof(RP);
		}
		fclose(aux);
	}
	fclose(Ptr);
}
void RelatorioDescr(void)
{
	FILE *Ptr = fopen("Produtos.dat","rb+");
	if(!ftell(Ptr) || Ptr == NULL )
		printf("Nenhum Produto foi Cadastrado ou Arquivo Inexistente!\n");
	{
		FILE *aux = fopen("aux.dat","wb+");
		SelecaoDireta(Ptr,aux,1);
		TpProduto RP;
		fseek(aux,0,2);
		int TL = ftell(aux);
		fseek(aux,0,0);
		while(ftell(aux)!=TL)
		{
			fread(&RP,sizeof(RP),1,aux);
			printf("Codigo: %d\tEstoque: %d\nDescricao: %s\nPreco: %.2f",RP.CodProd,RP.Estoque,RP.Descr,RP.Preco);
		}
		fclose(aux);
	}
	fclose(Ptr);
}
void ConsultaDescr(void)
{
	FILE *Ptr = fopen("Produtos.dat","rb+");
	if(!ftell(Ptr) || Ptr == NULL )
		printf("Nenhum Produto foi Cadastrado ou Arquivo Inexistente!\n");
	else
	{
		
		TpProduto RP;
		int pos;
		char descr[20];
		printf("Insira a descricao do produto a ser consultado!\nR: ");
		gets(descr);
		pos = BuscaSentinela(Ptr,descr,"Produtos.dat");
		if(pos>=0)
		{
			fseek(Ptr,pos,0);
			fread(&RP,sizeof(RP),1,Ptr);
			printf("Codigo: %d\tEstoque: %d\nDescricao: %s\nPreco: %.2f",RP.CodProd,RP.Estoque,RP.Descr,RP.Preco);
			
		}
		else
			printf("Produto não encontrado!")
	}
	fclose(Ptr);
}
