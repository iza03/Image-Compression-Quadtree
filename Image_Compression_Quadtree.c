/*Apostu Izabela-Elena 313CC*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// structura pentru arbore
typedef struct quadtree
{
    unsigned char node_type;
    unsigned char red, blue, green;
    struct quadtree *c1, *c2, *c3, *c4, *parent;
} quadtree;
// structura pentru matricea de culori
typedef struct
{
    unsigned char red, green, blue;
} img;
// functia aloca memorie pentru radacina arborelui
struct quadtree *init_tree(unsigned char type)
{
    quadtree *node = (quadtree *)malloc(sizeof(quadtree));
    node->node_type = 0;
    node->c1 = NULL;
    node->c2 = NULL;
    node->c3 = NULL;
    node->c4 = NULL;
    node->parent = NULL;
    return node;
}
// functia calculeaza media pentru cate o culoare
unsigned long long medie(img **a, int size, int x, int y, int color_nr)
{
    unsigned long long sum = 0;
    int i, j;
    for (i = x; i < x + size; i++)
        for (j = y; j < y + size; j++)
            if (color_nr == 0)
                sum += a[i][j].red;
            else if (color_nr == 1)
                sum += a[i][j].green;
            else
                sum += a[i][j].blue;
    return (unsigned long long)sum / (size * size);
}
// functia calculeaza factorul mean
unsigned long long det_mean(img **a, int size, int x, int y, int red, int green, int blue)
{
    unsigned long long mean = 0;
    int i, j;
    for (i = x; i < x + size; i++)
        for (j = y; j < y + size; j++)
        {
            mean += (red - a[i][j].red) * (red - a[i][j].red);
            mean += (green - a[i][j].green) * (green - a[i][j].green);
            mean += (blue - a[i][j].blue) * (blue - a[i][j].blue);
        }
    return (unsigned long long)mean / (3 * size * size);
}
/*
functia calculeaza valorile medii si factorul mean pentru blocul care are coltul din stanga sus la
pozitia x,y si creeaza arborele, nodul curent este parent
daca mean > factor se creeaza alte 4 noduri si se apeleaza functia pentru fiecare
daca mean < factor, parent este frunza si doar i se atribuie valorile RGB
*/
struct quadtree *divide(img **a, int size, int x, int y, int factor, int *leaves, struct quadtree *parent)
{
    int red, blue, green;
    red = medie(a, size, x, y, 0);
    green = medie(a, size, x, y, 1);
    blue = medie(a, size, x, y, 2);
    if (det_mean(a, size, x, y, red, green, blue) > factor)
    {
        parent->node_type = 0;
        /*
        parent->red = red;
        parent->green = green;
        parent->blue = blue;*/
        parent->c1 = (struct quadtree *)malloc(sizeof(struct quadtree));
        parent->c2 = (struct quadtree *)malloc(sizeof(struct quadtree));
        parent->c3 = (struct quadtree *)malloc(sizeof(struct quadtree));
        parent->c4 = (struct quadtree *)malloc(sizeof(struct quadtree));
        parent->c1->parent = parent;
        parent->c2->parent = parent;
        parent->c3->parent = parent;
        parent->c4->parent = parent;
        parent->c1 = divide(a, size / 2, x, y, factor, leaves, parent->c1);
        parent->c2 = divide(a, size / 2, x, y + size / 2, factor, leaves, parent->c2);
        parent->c3 = divide(a, size / 2, x + size / 2, y + size / 2, factor, leaves, parent->c3);
        parent->c4 = divide(a, size / 2, x + size / 2, y, factor, leaves, parent->c4);
    }
    else if (det_mean(a, size, x, y, red, green, blue) <= factor || size == 1)
    {
        (*leaves)++;
        parent->node_type = 1;
        // parent->parent->node_type = 0;
        parent->red = red;
        parent->green = green;
        parent->blue = blue;
        parent->c1 = NULL;
        parent->c2 = NULL;
        parent->c3 = NULL;
        parent->c4 = NULL;
    }
    return parent;
}
/*
functia parcurge arborele pentru a afla distanta pana la frunza cea mai apropiata de radacina
si pana la ultima radacina
*/
void min_max(struct quadtree *node, int depth, int *mini, int *max_depth)
{
    if (node == NULL)
    {
        return;
    }
    if (depth > *max_depth)
    {
        *max_depth = depth;
    }
    if (node->c1 == NULL && node->c2 == NULL && node->c3 == NULL && node->c4 == NULL)
        if (depth < *mini)
        {
            *mini = depth;
        }
    min_max(node->c1, depth + 1, mini, max_depth);
    min_max(node->c2, depth + 1, mini, max_depth);
    min_max(node->c3, depth + 1, mini, max_depth);
    min_max(node->c4, depth + 1, mini, max_depth);
}
// functia elibereaza memoria pentru arbore
void free_quadtree(struct quadtree *node)
{
    if (node == NULL)
    {
        return;
    }
    free_quadtree(node->c1);
    free_quadtree(node->c2);
    free_quadtree(node->c3);
    free_quadtree(node->c4);
    free(node);
}
// functia elibereaza memoria pentru matricea RGB
void free_img(img **a, int dim)
{
    int i;
    for (i = 0; i < dim; i++)
    {
        free(a[i]);
    }
    free(a);
}
// printeaza in fisierul f nivelul level din arbore
void print_level(struct quadtree *node, int level, FILE *f)
{
    if (node == NULL)
    {
        return;
    }
    if (level == 0)
    {
       //printeaza tipul nodului - frunza/nod intern
        fwrite(&node->node_type, sizeof(unsigned char), 1, f);
        //pentru frunze printeaza si valorile RGB 
        if (node->node_type == 1)
        {
            fwrite(&node->red, sizeof(unsigned char), 1, f);
            fwrite(&node->green, sizeof(unsigned char), 1, f);
            fwrite(&node->blue, sizeof(unsigned char), 1, f);
        }
    }
    // daca nu se afla la nivelul care trebuie, se autoapeleaza
    else
    {
        print_level(node->c1, level - 1, f);
        print_level(node->c2, level - 1, f);
        print_level(node->c3, level - 1, f);
        print_level(node->c4, level - 1, f);
    }
}
//apeleaza functia print_level pentru fiecare nivel
void print_quadtree(struct quadtree *root, int nr_niv, FILE *f)
{
    int i;
    for ( i = 0; i < nr_niv; i++)
    {
        print_level(root, i, f);
    }
}
int main(int argc, char *argv[])
{
    if(argv[1][2] == '3')
        return 0;
    char c[15];
    int i,j, nr=1;
    FILE *f, *out;
    //se deshide fisierul cu imaginea si se citeste antetul
    f = fopen(argv[3], "rb");
    fgets(c, 15, f);
    fgets(c, 15, f);
    int dim = atoi(c);
    fgets(c, 10, f);
    //se declara matricea si i se aloca memore apoi se citeste imaginea
    img **a = (img **)malloc((dim + 1) * sizeof(img *));
    for (i = 0; i < dim; i++)
    {
        a[i] = (img *)malloc((dim + 1) * sizeof(img));
    }
    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
        {
            fread(&a[i][j].red, sizeof(unsigned char), 1, f);
            fread(&a[i][j].green, sizeof(unsigned char), 1, f);
            fread(&a[i][j].blue, sizeof(unsigned char), 1, f);
        }
    /*
        daca avem cerinta 1 sau 2 se creeaza arborele, exceptand cazul cand imaginea 
        se comprima intr-o singura culoare, atunci se scrie in fisier rezultatul
    */
    if (argv[1][2] == '1' || argv[1][2] == '2')
    {
        int factor = atoi(argv[2]);
        int red = medie(a, dim, 0, 0, 0);
        int green = medie(a, dim, 0, 0, 1);
        int blue = medie(a, dim, 0, 0, 2);
        int mean = det_mean(a, dim, 0, 0, red, green, blue);
        if (mean <= factor && argv[1][2] == '1')
        {
            out = fopen(argv[4], "w");
            fprintf(out, "%d\n%d\n%d\n", 1, 1, dim);
            free_img(a, dim);
        }
        else
        {   
            struct quadtree *root, *node = init_tree(0);
            int nr_niv = 1, leaves = 0, dist_min = 0, mini = 999;
            // functia divide creeazaa arborele si returneaza ultima frunza
            root = divide(a, dim, 0, 0, factor, &leaves, node);
            // se parcurge arborele pana la radacina
            while (root->parent != NULL)
                root = root->parent;
            // se calculeaza distanta minima si maxima pana la o frunza
            min_max(root, nr, &mini, &nr_niv);
            /*
            numarul de blocuri din imagine pentru care scorul similaritatii pixelilor este mai
            mic sau egal decat factorul furnizat este 
            dimensiunea/(2^distanta minima pana la o frunza-1)
            */
            int div = 1;
            for (i = 1; i <= mini - 1; i++)
                div *= 2;
            // pentru cerinta1 scriu in fisier text valorile calculate
            if (argv[1][2] == '1')
            {
                out = fopen(argv[4], "w");
                fprintf(out, "%d\n%d\n%d\n", nr_niv, leaves, dim / div);
            }
            // iar pentru cerinta2 scriu dimensiunea si apelez functia care afiseaza arborele
            else
                {
                    out = fopen(argv[4], "wb");
                    fwrite(&dim,sizeof(int),1,out);
                    print_quadtree(root, nr_niv, out);
                }
            // eliberez memoria pentru arbore si matrice
            free_quadtree(root);
            free_img(a, dim);
        }
        fclose(f);
        fclose(out);
    }

    return 0;
}