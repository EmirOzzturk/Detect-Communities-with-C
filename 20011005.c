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
	int distance;//fonksiyonda aramaya baþlanan kaynak deðere uzaklýðý tutar
	int prev;//grafta gezerken önceki node'u tutar
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


/* **kodu çalýþtýrmadan önce number_of_nodes macrosunu girin** */
int main(){
	int i,j,t,k,myT,number_of_com=1,tmp=100;
	
	//Graf tanýmý
	struct Node **Graf = (struct Node**) malloc(max_n * sizeof(struct Node*));
	for (i = 0; i < max_n; i++){
		Graf[i] = (struct Node*) malloc(max_n * sizeof(struct Node));
		Graf[i]->adj_list = (int*) malloc(sizeof(int) * max_n);
	}

	
	Graf = input_from_file(Graf, "input.txt");
	
	printf("t degerini giriniz (Bir grubun minumum üye sayýsýnýn eþik deðeri) :");
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

//DFS algoritma mantýðýný kullanarak grafta gezer ve topluluk üye sayýsý bulur
int DFS(struct Node** Graf, struct Node* node) {
	int j=1;
    printf("%d ", node->data);
    node->visited = 1;
    
	int i = 0;;
	//Node'un visited=0 olan bütün komþularý için tekrar dönüyor.
	while(node->adj_list[i] != -1){
		if(Graf[find_node_from_data(node->adj_list[i], Graf)]->visited == 0 ){
			j+=DFS(Graf, Graf[find_node_from_data(node->adj_list[i], Graf)]);
		}
		i++;
	}
    return j;
}

//DFS fonksiyonunu çaðýrarak topluluklarý ekrana yazdýrýr
int DFS_Print(struct Node** Graf, struct Node* node, int com_num) {
	int i=0;
    printf("Topluluk %d: ", com_num);
    i = DFS(Graf, node);
    printf("\n");
    return i;
}

//en az üye içeren topluluðu döndürür
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

//shortest_path_bfs fonksiyonunu çaðýrarak en kýsa yollarý hesaplar ve geçilen yollarýn deðerini arttýrýr
//sonrasýnda en yüksek deðerli kenarý siler
struct Node** find_betweenness_and_delete_edge(struct Node** Graf){
	int i, j, tmp, prevtmp, maxi ,maxj;
	//Kenarlarýn betweenness deðerini tutan matris
	int **matrix_for_edges = (int**) malloc(max_n * sizeof(int*));
	for (i = 0; i < max_n; i++){
		matrix_for_edges[i] = (int*) malloc(sizeof(int) * max_n);
	}
	for(i = 0; i < number_of_nodes; i++){
		for(j = 0;j < number_of_nodes; j++){
			matrix_for_edges[i][j] = 0;
		}
	}
		
	//Kenar matrisini düzenler. Yol geçiyorsa 1 arttýrýr.
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

//bfs algoritmasý kullanarak (src -> dest) en kýsa yolu bulur
struct Node** shortest_path_bfs(struct Node** Graf, struct Node* src, struct Node* dest){
	int i, tmp, tmp2;
	struct Queue *q = (struct Queue*) malloc(sizeof(struct Queue));
	initializeQueue(q);
	
	//aradaki mesefaleri sonsuza ayarlýyoruz.
	// Nereden geleceðini gösteren düðümün özelliðini -1 yapýyoruz.
	for(i = 0; i < number_of_nodes; i++){
		Graf[i]->distance = sonsuz;
		Graf[i]->prev = -1;
	}
	src->distance = 0;
	enqueue(q, src->data);
	while(isEmpty(q) == 0){
		//sýradan bir node çek
		tmp = find_node_from_data(dequeue(q), Graf);
		if(Graf[tmp]->data == dest->data){
			return Graf;
		}
		/*o node'un komþularýný sýraya koy ve 
		distance deðerlerini bu node'un distance deðerinin 1 fazlasý yap*/
		i = 0;
		while(Graf[tmp]->adj_list[i] != -1){
			tmp2 = find_node_from_data(Graf[tmp]->adj_list[i], Graf);
			//distance deðeri daha önceden deðiþtiyse demek ki gezilmiþ
			if(Graf[tmp2]->distance == sonsuz && Graf[tmp2]->data != Graf[tmp]->data){
				Graf[tmp2]->distance = Graf[tmp]->distance + 1;
				Graf[tmp2]->prev = Graf[tmp]->data;
				enqueue(q, Graf[tmp2]->data);				
			}
			i++;
		}
	}	
}

//verilen dosyadan grafý okur ve struct yapýsýna kaydeder.
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
		//**komþuluk listesinin sonunda -1 olur**
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
