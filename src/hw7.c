#include "hw7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


static matrix_sf* new_matrix(char name, unsigned int num_rows, unsigned int num_cols) 
{
    //base case return null if num_rows or num_cols is 0
    if (num_rows ==0 || num_cols == 0) 
    {
        return NULL;
    }

    //allocate memory for matrix 
    matrix_sf *m = malloc(sizeof(matrix_sf)+num_rows*num_cols*sizeof(int));
    //check malloc worked
    if (!m) 
    {
        return NULL;
    }
    //initialize matrix fields
    m->name = name;
    m->num_rows = num_rows;
    m->num_cols = num_cols;
   //find num of total elements
    unsigned int n = num_rows * num_cols;

    //initialize all values to 0
    for (unsigned int i = 0; i < n; i++) 
    {
        m->values[i] = 0;
    }
    return m;
}

//insert matrix into bst
bst_sf* insert_bst_sf(matrix_sf *mat, bst_sf *root) 
{
    //base cae reject null matrix
    if(!mat)
    {
        return root;
    } 

    //create new node in empty tree
    if(!root) 
    {
        //memory for new node
        bst_sf *node = malloc (sizeof(bst_sf));
        //check malloc worked
        if(!node) 
        {
            free(mat);
            return NULL;
        }
        //initialize new node
        node->mat = mat;
        node->left_child = NULL;
        node->right_child = NULL;
        return node;
    }
    //if it's not empty, traverse left subtree if name is alphabetically less
    if (mat->name < root->mat->name) 
    {
       root->left_child = insert_bst_sf(mat, root->left_child);
    } 
    //traverse right subtree if name is alphabetically greater
    else if (mat->name > root->mat->name) 
    {
        root->right_child = insert_bst_sf(mat, root->right_child);
    } 
    //otherwise already exists
    else 
    {
        free(mat);
    }
    return root;
}

//search bst for matrix with given name
matrix_sf* find_bst_sf(char name, bst_sf *root) 
{
    //base case reject null root
    if(!root) 
    {
        return NULL;
    }
    //if target name alphabetically less than node name, traverse left subtree 
    if(name < root->mat->name) 
    {
        return find_bst_sf(name, root->left_child);
    } 
    //if target name alphabetically greater than node name, traverse right subtree
    else if (name > root->mat->name) 
    {
        return find_bst_sf(name, root->right_child);
    }
    //found target name
    return root->mat;
}

//recuresively free bst
void free_bst_sf(bst_sf *root) 
{
    //base case for empty tree
    if(!root)
    {
        return;
    } 
    //free all nodes in left and right subtrees
    free_bst_sf(root->left_child);
    free_bst_sf(root->right_child);

    //free matrix and node itself
    free(root->mat);
    free(root);
}

//add two matrices
matrix_sf* add_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) 
{
    //base case reject null matrices
    if (!mat1 || !mat2) 
    {
        return NULL;
    }
    //check dimensions match
    if(mat1->num_rows != mat2-> num_rows || mat1->num_cols != mat2->num_cols) 
    {
        return NULL;
    }
    //get rows and cols
    unsigned r = mat1->num_rows;
    unsigned c = mat1->num_cols;
    matrix_sf *out = new_matrix(0, r, c);
    //check malloc worked
    if(!out) 
    {
        return NULL;
    }

    //add corresponding elements, iterate over rows then columns then add corresponding elements
    for(unsigned int i = 0; i < mat1->num_rows; i++) 
    {
        //row offset to track position in 1D array
        unsigned int base = i * mat1->num_cols;
        for(unsigned int j = 0; j < mat1->num_cols; j++) 
        {
            unsigned int idx = base + j;
            out->values[idx] = mat1->values[idx] + mat2->values[idx];
        }
    }
    return out;
}

//multiply two matrices
matrix_sf* mult_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) 
{
    //base case reject null matrices
    if(!mat1 || !mat2) 
    {
        return NULL;
    }
    //check dimensions are valid for multiplication
    if(mat1->num_cols != mat2-> num_rows) 
    {
        return NULL;
    }
    unsigned m1_row = mat1->num_rows;
    unsigned m2_col = mat2->num_cols;
    unsigned m1_col = mat1->num_cols;

    matrix_sf *out = new_matrix(0, m1_row, m2_col);
    if(!out) 
    {
        return NULL;
    }

    //iterate over rows of mat1
    for(unsigned int r = 0; r < m1_row; r++) 
    {
        //iterate over columns of mat2
        for(unsigned int c = 0; c < m2_col; c++) 
        {
            int sum = 0;
            //perform dot product of row from mat1 and column from mat2
            for(unsigned int k = 0; k < m1_col; k++) 
            {
                sum += mat1->values[r * m1_col + k] * mat2->values[k * m2_col + c];
            }
            out->values[r * out->num_cols + c] = sum;
        }
    }
    return out;
}

//transpose matrix
matrix_sf* transpose_mat_sf(const matrix_sf *mat) 
{
    //base case reject null matrix
    if(!mat) 
    {
        return NULL;
    }   
    unsigned int rows = mat->num_rows;
    unsigned int cols = mat->num_cols;

    matrix_sf *out = new_matrix(0, cols, rows);
    if(!out) 
    {
        return NULL;
    }

    //iterate over rows and columns, swapping row and column indices
    for(unsigned int r = 0; r < rows; r++) 
    {
        unsigned int base = r * cols;
        for(unsigned int c = 0; c < cols; c++) 
        {
            out->values[c * rows + r] = mat->values[base + c];
        }
    }
    return out;
}

//create matrix from expression string
matrix_sf* create_matrix_sf(char name, const char *expr) 
{
    //base case reject null expression
    if(!expr) 
    {
        return NULL;
    }

    //pointer to parse rows and columns from expression string, nread for sscanf
    const char *p = expr;
    int nread = 0;
    unsigned rows = 0, cols = 0;

    //parse rows
    while(*p)
    {
        //reads unsigned integer from string, assigns to rows through pointer, nread gets number of characters read
        if(sscanf(p, "%u%n", &rows, &nread) == 1) 
        {
            p+= nread;
            break;
        }
        ++p;
    }
    if(rows == 0)
    {
        return NULL;
    }
    
    //parse columns
    while(*p)
    {
        if(sscanf(p, "%u%n", &cols, &nread) == 1) 
        {
            p+= nread;
            break;
        }
        ++p;
    }
    if(cols == 0)
    {
        return NULL;
    }   

    matrix_sf *mat = new_matrix(name, rows, cols);
    if(!mat) return NULL;

    //total number of elements matrix should have
    unsigned total = rows * cols;
    //index to track number of values read
    unsigned idx = 0;
    //value to hold parsed integer, can hold negative values too
    int val = 0;

    //parse values until all read or string ends
    while(idx< total && *p) 
    {
        //read unsigned integer from string
        if(sscanf(p, "%d%n", &val, &nread) == 1) 
        {
            mat->values[idx++] = val;
            p+= nread;
        } 
        else 
        {
            ++p;
        }
    }
    //if not all values read, free matrix and return null
    if(idx < total) 
    {
        free(mat);
        return NULL;
    }
    return mat;

}

//maximum capacity for stack
#define MAX_STACK 100

//order of precedence for operators
static int prec(char c) 
{
    if (c == '\'') return 3; 
    if (c == '*') return 2;
    if (c == '+') return 1;
    return 0;
}


static int is_alphanumeric(char c)
{
    return (c >= 'A' && c <= 'Z') ||   (c >= 'a' && c <= 'z') ||   (c >= '0' && c <= '9');
}

//convert infix to postfix
char* infix2postfix_sf(char *infix) 
{
    //stores length and allocates memory for postfix
    int len = strlen(infix);
    char *postfix = malloc((len + 1) * sizeof(char));
    if (!postfix) return NULL;

    //fixed stack size with empty stack
    char stack[MAX_STACK];
    int top = -1;
    int k = 0;

    //iterate over each character
    for (int i = 0; i < len; i++) 
    {
        char c = infix[i];

        //append letter or number to postfix string
        if (is_alphanumeric(c)) 
        {
            postfix[k++] = c;
        } 
        //push open parantheses
        else if (c == '(') 
        {
            stack[++top] = c;
        } 
        //pop operators from the stack and append to postfix until an open parenthesis is found.
        else if (c == ')') 
        {
            while (top >= 0 && stack[top] != '(')
                postfix[k++] = stack[top--];
            if (top >= 0) top--; 
        } 
        //pop operators from the stack if higher or equal precedence than the current operator
        else 
        { 
            while (top >= 0 && prec(c) <= prec(stack[top]))
            {
                //append to postfix
                postfix[k++] = stack[top--];
            }
            stack[++top] = c;
        }
    }

    while (top >= 0)
    {
        postfix[k++] = stack[top--];
    }

    postfix[k] = '\0';
    return postfix;
}

matrix_sf* evaluate_expr_sf(char name, char *expr, bst_sf *root)
{
    if (!expr || !root) 
    {
        return NULL;
    }
    return NULL;
}

matrix_sf *execute_script_sf(char *filename) {
   return NULL;
}

// This is a utility function used during testing. Feel free to adapt the code to implement some of
// the assignment. Feel equally free to ignore it.
matrix_sf *copy_matrix(unsigned int num_rows, unsigned int num_cols, int values[]) {
    matrix_sf *m = malloc(sizeof(matrix_sf)+num_rows*num_cols*sizeof(int));
    m->name = '?';
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    memcpy(m->values, values, num_rows*num_cols*sizeof(int));
    return m;
}

// Don't touch this function. It's used by the testing framework.
// It's been left here in case it helps you debug and test your code.
void print_matrix_sf(matrix_sf *mat) {
    assert(mat != NULL);
    assert(mat->num_rows <= 1000);
    assert(mat->num_cols <= 1000);
    printf("%d %d ", mat->num_rows, mat->num_cols);
    for (unsigned int i = 0; i < mat->num_rows*mat->num_cols; i++) {
        printf("%d", mat->values[i]);
        if (i < mat->num_rows*mat->num_cols-1)
            printf(" ");
    }
    printf("\n");
}
