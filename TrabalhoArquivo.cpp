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






