//João Miguel Sapucci
//Leonardo Alessio Biscassi
//Luiz Otávio Passoni Souza Estevam

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
int BuscaSentinela(FILE *Ptr, char Descr[]);
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
	if(!ftell(Ptr))
		return -1;
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
	printf("\n*** Cadastro de Produtos ***\n");
	printf("Insira o codigo do produto: ");
	scanf("%d",&Prod.CodProd);
	while(Prod.CodProd > 0){
		if(BuscaSeqInd(PtrProd,Prod.CodProd) == -1){
			printf("Descricao: ");
			fflush(stdin); gets(Prod.Descr);
			printf("Preco: R$");
			scanf("%f",&Prod.Preco);
			printf("Estoque: ");
			scanf("%d",&Prod.Estoque);
			
			fseek(PtrProd,0,SEEK_END);
			fwrite(&Prod,sizeof(TpProduto),1,PtrProd);
			InsertionSort(PtrProd);
		}
		else
			printf("\nCodigo ja foi cadastrado!\n");
		
		printf("Insira o codigo do produto: ");
		scanf("%d",&Prod.CodProd);
	}
	fclose(PtrProd);
}

void ConsultaProd(void){
	TpProduto RP;
	int pos;
	FILE *Ptr = fopen("Produtos.dat","rb");
	if (Ptr == NULL)
	{
	    printf("Arquivo inexistente!\n");
	    return;
	}
	
	fseek(Ptr, 0, SEEK_END);
	
	if (ftell(Ptr) == 0)
	{
	    printf("Nenhum produto foi cadastrado!\n");
	    fclose(Ptr);
	    return;
	}
	else{
		clrscr();
		fseek(Ptr,0,SEEK_END);
		printf("*** Consulta Produto ***\n");
		printf("Insira o codigo: ");
		scanf("%d",&RP.CodProd);
		while(RP.CodProd > 0){
			pos=BuscaBinaria(Ptr,RP.CodProd);
			if(pos == -1)
				printf("Produto nao encontrado!");
			else{
				fseek(Ptr,pos,SEEK_SET);
				fread(&RP,sizeof(TpProduto),1,Ptr);
				printf("==========================================================================");
				printf("\nCodigo: %d\tDescricao: %s\t Estoque: %d\t Preco: %.2f\n",RP.CodProd,RP.Descr,RP.Estoque,RP.Preco);
				printf("==========================================================================");
			}
			
			getch();
			clrscr();
			printf("*** Consulta Produto ***\n");
			printf("Insira o codigo: ");
			scanf("%d",&RP.CodProd);
		}
	}
	fclose(Ptr);	
}
int BuscaSentinela(FILE *Ptr, char Descr[])
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
	remove("Produtos.dat");
	rename("aux.dat","Produtos.dat");
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
	if (Ptr == NULL)
	{
	    printf("Arquivo inexistente!\n");
	    return;
	}
	
	fseek(Ptr, 0, SEEK_END);
	
	if (ftell(Ptr) == 0)
	{
	    printf("Nenhum produto foi cadastrado!\n");
	    fclose(Ptr);
	    return;
	}
	else
	{
		system("cls");
		printf("\n*** Relatorio por Preco ***\n");
		FILE *aux = fopen("aux.dat","wb+");
		SelecaoDireta(Ptr,aux,0);
		TpProduto RP;
		fseek(aux,0,2);
		int TL = ftell(aux);
		while(TL>0)
		{
			fseek(aux,TL - sizeof(RP),0);
			fread(&RP,sizeof(RP),1,aux);
			printf("Codigo: %d\tDescricao: %s\tEstoque: %d\tPreco: %.2f\n",RP.CodProd,RP.Descr,RP.Estoque,RP.Preco);
			TL = TL - sizeof(RP);
		}
		fclose(aux);
	}
	fclose(Ptr);
	getch();
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
	if (Ptr == NULL)
	{
	    printf("Arquivo inexistente!\n");
	    return;
	}
	
	fseek(Ptr, 0, SEEK_END);
	
	if (ftell(Ptr) == 0)
	{
	    printf("Nenhum produto foi cadastrado!\n");
	    fclose(Ptr);
	    return;
	}
	else
	{
		system("cls");
		printf("\n*** Relatorio por Estoque ***\n");
		FILE *aux = fopen("aux.dat","wb+");
		BubbleSort(Ptr,aux);
		TpProduto RP;
		fseek(aux,0,2);
		int TL = ftell(aux);
		rewind(aux);
		while(TL>0)
		{
			fread(&RP,sizeof(RP),1,aux);
			printf("Codigo: %d\tDescricao: %s\tEstoque: %d\tPreco: %.2f\n",RP.CodProd,RP.Descr,RP.Estoque,RP.Preco);
			TL = TL - sizeof(RP);
		}
		fclose(aux);
	}
	fclose(Ptr);
	getch();
}
void RelatorioDescr(void)
{
	FILE *Ptr = fopen("Produtos.dat","rb+");
	if (Ptr == NULL)
	{
	    printf("Arquivo inexistente!\n");
	    return;
	}
	
	fseek(Ptr, 0, SEEK_END);
	
	if (ftell(Ptr) == 0)
	{
	    printf("Nenhum produto foi cadastrado!\n");
	    fclose(Ptr);
	    return;
	}
	system("cls");
	printf("\n*** Relatorio por Descricao ***\n");
	FILE *aux = fopen("aux.dat","wb+");
	SelecaoDireta(Ptr,aux,1);
	TpProduto RP;
	fseek(aux,0,2);
	int TL = ftell(aux);
	fseek(aux,0,0);
	while(ftell(aux)!=TL)
	{
		fread(&RP,sizeof(RP),1,aux);
		printf("Codigo: %d\tDescricao: %s\tEstoque: %d\tPreco: %.2f\n",RP.CodProd,RP.Descr,RP.Estoque,RP.Preco);
	}
	fclose(aux);
	fclose(Ptr);
	getch();
}
void ConsultaDescr(void)
{
	FILE *Ptr = fopen("Produtos.dat","rb+");
	if (Ptr == NULL)
	{
	    printf("Arquivo inexistente!\n");
	    return;
	}
	
	fseek(Ptr, 0, SEEK_END);
	
	if (ftell(Ptr) == 0)
	{
	    printf("Nenhum produto foi cadastrado!\n");
	    fclose(Ptr);
	    return;
	}	
	else
	{
		TpProduto RP;
		int pos;
		char descr[20];
		system("cls");
		printf("*** Cosulta por Descricao ***\n");
		printf("Insira a descricao do produto a ser consultado!\nR: ");
		fflush(stdin); gets(descr);
		pos = BuscaSentinela(Ptr,descr);
		if(pos>=0)
		{
			Ptr = fopen("Produtos.dat", "rb");
			fseek(Ptr,pos,0);
			fread(&RP,sizeof(RP),1,Ptr);
			printf("Codigo: %d\tEstoque: %d\nDescricao: %s\nPreco: %.2f",RP.CodProd,RP.Estoque,RP.Descr,RP.Preco);
			fclose(Ptr);
			
		}
		else
			printf("Produto não encontrado!");
	}
	getch();
}


void ComplexidadeAlgoritmos(void){
    system("cls");
    printf("*** Complexidade dos Algoritmos ***\n\n");

    printf("BUSCA EXAUSTIVA\n");
    printf("CASOS: Melhor: O(1) | Medio: O(n) | Pior: O(n)\n");
    printf("Percorre os elementos ate encontrar o valor.\n\n");

    printf("BUSCA EXAUSTIVA COM SENTINELA\n");
    printf("CASOS: Melhor: O(1) | Medio: O(n) | Pior: O(n)\n");
    printf("Utiliza uma sentinela para reduzir verificacoes.\n\n");

    printf("BUSCA SEQUENCIAL INDEXADA\n");
    printf("CASOS: Melhor: O(1) | Medio/Pior: O(n)\n");
    printf("Percorre os elementos ate encontrar um valor maior ou igual ao desejado.\n\n");

    printf("BUSCA BINARIA\n");
    printf("CASOS: Melhor: O(1) | Medio/Pior: O(log n)\n");
    printf("Divide a busca pela metade a cada etapa.Exige dados ordenados.\n\n");

    printf("INSERTION SORT\n");
    printf("CASOS: Melhor: O(n) | Medio/Pior: O(n^2)\n");
    printf("Insere cada elemento na posicao correta.\n\n");

    printf("BUBBLE SORT\n");
    printf("CASOS: Melhor: O(n)* | Medio/Pior: O(n^2)\n");
    printf("Compara elementos vizinhos e realiza trocas.\n");
    printf("*Melhor caso com parada antecipada.\n\n");

    printf("SELECTION SORT\n");
    printf("CASOS: Melhor/Medio/Pior: O(n^2)\n");
    printf("Seleciona o menor elemento a cada passagem.");

    getch();
    system("cls");
}

char Menu(void){
    system("cls");
    printf("*** MENU ***\n");
    printf("1 - Cadastrar produtos\n");
    printf("2 - Consultar por codigo\n");
    printf("3 - Relatorio por estoque\n");
    printf("4 - Relatorio por descricao\n");
    printf("5 - Consultar por descricao\n");
    printf("6 - Relatorio por preco\n");
    printf("7 - Complexidade dos Algoritmos\n");
    printf("0 - Sair\n");
    printf("Escolha uma opcao: ");
	return getche();
}

int main(void)
{
    char opcao;
    do
    {
        opcao = Menu();
        switch (opcao)
        {
            case '0': break;
            case '1': CadastroProd(); break;
            case '2': ConsultaProd(); break;
            case '3': RelatorioEstoque(); break;
            case '4': RelatorioDescr(); break;
            case '5': ConsultaDescr(); break;
            case '6': RelatorioPreco(); break;
            case '7': ComplexidadeAlgoritmos(); break;
            default: printf("Opcao invalida!\n");
        }
    
	} while (opcao != '0');

    return 0;
}
