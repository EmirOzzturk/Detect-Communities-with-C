#include <stdio.h>
#include <malloc.h>

#define max_n 100
#define number_of_nodes 8
#define sonsuz 999

// Queue implementation for BFS
struct Queue{
    int items[1000];
    int front;
    int rear;
};

struct Node{
	int data;
	int *adj_list;
	int distance;//fonksiyonda aramaya ba�lanan kaynak de�ere uzakl��� tutar
	int prev;//grafta gezerken �nceki node'u tutar
	int visited;
};

void initializeQueue(struct Queue *q);
int isEmpty(struct Queue *q);
void enqueue(struct Queue *q, int item);
int dequeue(struct Queue *q);
int find_node_from_data(int data, struct Node** Graf);
struct Node** input_from_file(struct Node **Graf, char *filename);
struct Node** shortest_path_bfs(struct Node** Graf, struct Node* src, struct Node* dest);
struct Node** find_betweenness_and_delete_edge(struct Node** Graf);
int DFS(struct Node** Graf, struct Node* node);
int DFS_Print(struct Node** Graf, struct Node* node, int com_num);
int print_Communities(struct Node** Graf, int* com_num);


/* **kodu �al��t�rmadan �nce number_of_nodes macrosunu girin** */
int main(){
	int i,j,t,k,myT,number_of_com=1,tmp=100;
	
	//Graf tan�m�
	struct Node **Graf = (struct Node**) malloc(max_n * sizeof(struct Node*));
	for (i = 0; i < max_n; i++){
		Graf[i] = (struct Node*) malloc(max_n * sizeof(struct Node));
		Graf[i]->adj_list = (int*) malloc(sizeof(int) * max_n);
	}

	
	Graf = input_from_file(Graf, "input.txt");
	
	printf("t degerini giriniz (Bir grubun minumum �ye say�s�n�n e�ik de�eri) :");
	scanf("%d", &t);
	printf("k degerini giriniz (ayni grup sayisinda iterasyon sayisi) :");
	scanf("%d", &k);
	printf("\n");
	
	while(myT > t && k > 1){
		find_betweenness_and_delete_edge(Graf);	
		tmp = number_of_com;
		myT = print_Communities(Graf, &number_of_com);	
		if(number_of_com == tmp) k--;	
		//printf("%d ", k);
	}
	
}

//DFS algoritma mant���n� kullanarak grafta gezer ve topluluk �ye say�s� bulur
int DFS(struct Node** Graf, struct Node* node) {
	int j=1;
    printf("%d ", node->data);
    node->visited = 1;
    
	int i = 0;;
	//Node'un visited=0 olan b�t�n kom�ular� i�in tekrar d�n�yor.
	while(node->adj_list[i] != -1){
		if(Graf[find_node_from_data(node->adj_list[i], Graf)]->visited == 0 ){
			j+=DFS(Graf, Graf[find_node_from_data(node->adj_list[i], Graf)]);
		}
		i++;
	}
    return j;
}

//DFS fonksiyonunu �a��rarak topluluklar� ekrana yazd�r�r
int DFS_Print(struct Node** Graf, struct Node* node, int com_num) {
	int i=0;
    printf("Topluluk %d: ", com_num);
    i = DFS(Graf, node);
    printf("\n");
    return i;
}

//en az �ye i�eren toplulu�u d�nd�r�r
int print_Communities(struct Node** Graf, int* com_num){
	printf("Topluluklar: \n");
	int i;
	
	for(i = 0; i < number_of_nodes;i++){
		Graf[i]->visited = 0;
	}
	
	(*com_num) = 1;
	int j = 999; int tmp;
	
	for(i = 0; i < number_of_nodes; i++){
		if(Graf[i]->visited == 0){
			tmp = DFS_Print(Graf, Graf[i], (*com_num));
			if(tmp < j){
				j = tmp;
			}
			(*com_num)++;
		}
	}
	return j;
}

//shortest_path_bfs fonksiyonunu �a��rarak en k�sa yollar� hesaplar ve ge�ilen yollar�n de�erini artt�r�r
//sonras�nda en y�ksek de�erli kenar� siler
struct Node** find_betweenness_and_delete_edge(struct Node** Graf){
	int i, j, tmp, prevtmp, maxi ,maxj;
	//Kenarlar�n betweenness de�erini tutan matris
	int **matrix_for_edges = (int**) malloc(max_n * sizeof(int*));
	for (i = 0; i < max_n; i++){
		matrix_for_edges[i] = (int*) malloc(sizeof(int) * max_n);
	}
	for(i = 0; i < number_of_nodes; i++){
		for(j = 0;j < number_of_nodes; j++){
			matrix_for_edges[i][j] = 0;
		}
	}
		
	//Kenar matrisini d�zenler. Yol ge�iyorsa 1 artt�r�r.
	for(i = 0; i < number_of_nodes; i++){
		for(j = 0;j < number_of_nodes; j++){
			if(i != j){
				shortest_path_bfs(Graf, Graf[i], Graf[j]);
				tmp = j;
				while(Graf[tmp]->prev != -1){
					prevtmp = find_node_from_data(Graf[tmp]->prev,Graf);
					matrix_for_edges[tmp][prevtmp] += 1;
					matrix_for_edges[prevtmp][tmp] += 1;	
					tmp = prevtmp;				
				}
			}
		}
	}
	int max = 0;
	for(i = 0; i < number_of_nodes; i++){
		for(j = 0;j < number_of_nodes; j++){
			if(matrix_for_edges[i][j] > max){
				max = matrix_for_edges[i][j];
				maxi = i;
				maxj = j;
			}
		}
	}	
	
	j = 0;
	while(Graf[maxi]->adj_list[j+1] != -1){
		if(Graf[maxj]->data == Graf[maxi]->adj_list[j]){
			tmp = Graf[maxi]->adj_list[j];
			Graf[maxi]->adj_list[j] = Graf[maxi]->adj_list[j+1];
			Graf[maxi]->adj_list[j+1] = tmp;
		}
		j++;
	}
	Graf[maxi]->adj_list[j] = -1;
	
	j = 0;
	while(Graf[maxj]->adj_list[j+1] != -1){
		if(Graf[maxi]->data == Graf[maxj]->adj_list[j]){
			tmp = Graf[maxj]->adj_list[j];
			Graf[maxj]->adj_list[j] = Graf[maxj]->adj_list[j+1];
			Graf[maxj]->adj_list[j+1] = tmp;
		}
		j++;
	}
	Graf[maxj]->adj_list[j] = -1;
	
	for (i = 0; i < max_n; i++){
		free(matrix_for_edges[i]);
	}
	free(matrix_for_edges);
}

//bfs algoritmas� kullanarak (src -> dest) en k�sa yolu bulur
struct Node** shortest_path_bfs(struct Node** Graf, struct Node* src, struct Node* dest){
	int i, tmp, tmp2;
	struct Queue *q = (struct Queue*) malloc(sizeof(struct Queue));
	initializeQueue(q);
	
	//aradaki mesefaleri sonsuza ayarl�yoruz.
	// Nereden gelece�ini g�steren d���m�n �zelli�ini -1 yap�yoruz.
	for(i = 0; i < number_of_nodes; i++){
		Graf[i]->distance = sonsuz;
		Graf[i]->prev = -1;
	}
	src->distance = 0;
	enqueue(q, src->data);
	while(isEmpty(q) == 0){
		//s�radan bir node �ek
		tmp = find_node_from_data(dequeue(q), Graf);
		if(Graf[tmp]->data == dest->data){
			return Graf;
		}
		/*o node'un kom�ular�n� s�raya koy ve 
		distance de�erlerini bu node'un distance de�erinin 1 fazlas� yap*/
		i = 0;
		while(Graf[tmp]->adj_list[i] != -1){
			tmp2 = find_node_from_data(Graf[tmp]->adj_list[i], Graf);
			//distance de�eri daha �nceden de�i�tiyse demek ki gezilmi�
			if(Graf[tmp2]->distance == sonsuz && Graf[tmp2]->data != Graf[tmp]->data){
				Graf[tmp2]->distance = Graf[tmp]->distance + 1;
				Graf[tmp2]->prev = Graf[tmp]->data;
				enqueue(q, Graf[tmp2]->data);				
			}
			i++;
		}
	}	
}

//verilen dosyadan graf� okur ve struct yap�s�na kaydeder.
struct Node** input_from_file(struct Node **Graf, char *filename){
	int i,j;
	char buffer[1024];
	FILE* fptr;
	char *token = (char*) malloc(1024 * sizeof(char));
	fptr = fopen(filename, "r+");
	i = 0; j = 0;
	while(i < number_of_nodes){
		fgets(buffer, 1024, fptr);
		token = strtok(buffer, ":");
		Graf[i]->data = atoi(token);
		j=0;
		//**kom�uluk listesinin sonunda -1 olur**
		while(atoi(token) != -1){
			token = strtok(NULL, ",");
			Graf[i]->adj_list[j] = atoi(token);
			j++;
		}
		i++;		
	}
	fclose(fptr);
	return Graf;
}

int find_node_from_data(int data, struct Node** Graf){
		int i = 0;
		while(Graf[i]->data != data && i < number_of_nodes){
			i++;
		}
		if(i == number_of_nodes){
			printf("Aradaiginiz dugum grafta yok\n");
			return 0;
		}else{
			return i;
		}
}

void initializeQueue(struct Queue *q) {
    q->front = -1;
    q->rear = -1;
}

int isEmpty(struct Queue *q) {
	if(q->front == -1){
		return 1;
	}else{
		return 0;
	}
}

void enqueue(struct Queue *q, int item) {
    if (q->rear == max_n - 1) {
        printf("Queue is full\n");
        return;
    }
    if (isEmpty(q)) {
        q->front = 0;
		q->rear = 0;
    } else {
        q->rear++;
    }
    q->items[q->rear] = item;
}

int dequeue(struct Queue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return 0;
    }
    int item = q->items[q->front];
    if (q->front == q->rear) {
        q->front = -1;
		q->rear = -1;
    } else {
        q->front++;
    }
    return item;
}
