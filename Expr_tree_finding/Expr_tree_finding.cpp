#include<iostream>
#include<cmath>
using namespace std;


struct expr_tree
{
    char value;
    expr_tree* left, * right;
    expr_tree* prev;

	expr_tree()
	{
        left = NULL;
        right = NULL;
        prev = NULL;
	}
};

struct tree_stack
{
    expr_tree* st[100];
    int n;
    int top;

    tree_stack()
    {
        n = 100;
        top = -1;
    }

    void push(expr_tree* val)
    {

        if (top >= n - 1)
            cout << "Stack Overflow" << endl;
        else {
            top++;
            st[top] = val;
        }
    }

    expr_tree* pop()
    {
        if (top <= -1)
        {
            cout << "Stack Underflow" << endl;
            exit(0);
            return NULL;
        }
        else {
            top--;
            return st[top + 1];
        }
    }

    void display()
    {
        if (top >= 0) {
            for (int i = top; i >= 0; i--)
                cout << st[i] << " ";
            cout << endl;
        }
        else
            cout << "Stack is empty";
    }
};

bool isOperator(char c)
{
    if (c == '+' || c == '-' ||
        c == '*' || c == '/' ||
        c == '^' || c == '%')
        return true;
    return false;
}

// Recursive displaying of unfix order
void infix_order(expr_tree* t)
{
    if (t)
    {
        infix_order(t->left);
        printf("%c ", t->value);
        infix_order(t->right);
    }
}

expr_tree* new_node(char v)
{
    expr_tree* temp = new expr_tree;
    //temp->left = temp->right = NULL;
    temp->value = v;
    return temp;
};

// Returns root of the constructed tree
expr_tree* constructTree(char postfix[])
{
    tree_stack st;
    expr_tree* t, * t1, * t2;

    for (int i = 0; i < strlen(postfix); i++)
    {
        // If operand, simply push into stack
        if (!isOperator(postfix[i]))
        {
            t = new_node(postfix[i]);
            st.push(t);
        }
        else // operator
        {
            t = new_node(postfix[i]);

            t1 = st.pop(); 
            t2 = st.pop();

            t->right = t1;
            t->left = t2;
            t->left->prev = t;

            st.push(t);
        }
    }

   //root of the tree
    t = st.pop();
    // The stack will contain operators with first value as an operation, right as a number and left as an operation (in the end - both are numbers)
    return t;
}

int group_of_operation(char oper) // 1 - highest priority; 3 - lowest priority
{
    if (oper == '%' || oper == '^')
        return 1;
    if (oper == '*' || oper == '/')
        return 2;
    if (oper == '+' || oper == '-')
        return 3;
}

char perf_operation(char n1, char n2, char oper)
{
    int a = n1 - '0';
    int b = n2 - '0';

	switch(oper)
	{
    case '+':
        return a + b + '0';
        break;
    case '-':
        return a - b + '0';
        break;
    case '*':
        return a * b + '0';
        break;
    case '/':
        return a / b + '0';
        break;
    case '%':
        return a % b + '0';
        break;
    case '^':
        return pow(a, b) + '0';
        break;
	default:
        cout << "wrong operation!";
        return NULL;
        break;
	}
}

expr_tree* dublication(expr_tree* tree)
{
    expr_tree* temp_tree = tree;
    expr_tree* new_tree = new expr_tree;
    expr_tree* prev_node_for_new;
	
    new_tree->value = temp_tree->value;
    prev_node_for_new = NULL;
	
	while(temp_tree)
	{
        if(temp_tree->left)
        new_tree->left = new_node(temp_tree->left->value);
        if (temp_tree->right)
        {
            new_tree->right = new_node(temp_tree->right->value);
            new_tree->right->prev = new_tree;
        }
        new_tree->prev = prev_node_for_new;

        prev_node_for_new = new_tree;
        new_tree = new_tree->left;
        temp_tree = temp_tree->left;
	}

    new_tree = prev_node_for_new;

	while(new_tree->prev!=NULL)
	{
        new_tree = new_tree->prev;
	}

    return new_tree;
}

int calculating(expr_tree* original_tree)
{
	//not to change the main tree:
    expr_tree* tree = dublication(original_tree);
    expr_tree* tr1 = tree;
	
	//for group1:

    int count_same_priority_in_one_seq = 0;
    while(tr1!=NULL)
    {
        if(group_of_operation(tr1->value) == 1 )
        {
            if(group_of_operation(tr1->left->value) !=1 && tr1->left->left != NULL) // if there are 1st priorities from the start, we can just remain them
            {
                
                char number_for_current_operation = tr1->left->right->value; // one node down in the tree

                number_for_current_operation = perf_operation(number_for_current_operation, tr1->right->value, tr1->value);
                tr1->right->value = '1'; // as 1 will not affect the calculations in the future
            	
            	for(int i =0;i<count_same_priority_in_one_seq;i++) // as we go from one node down 
            	{
                    tr1 = tr1->prev;
                    number_for_current_operation = perf_operation(number_for_current_operation, tr1->right->value, tr1->value);
                    tr1->right->value ='1'; // as 1 will not affect the calculations in the future (with changing the operation to '0')
                    tr1->value = '*';
            	}

            	for (int i=0;i<count_same_priority_in_one_seq;i++)
            	{
                    tr1 = tr1->left;
            	}

                tr1->left->right->value = number_for_current_operation;
                tr1->value = '*';
                count_same_priority_in_one_seq = 0;
            }
            else
            {
                count_same_priority_in_one_seq++;
            }
        }

        tr1 = tr1->left;
    }

    //for group2:
    tr1 = tree;
    count_same_priority_in_one_seq = 0;
	while (tr1 != NULL)
    {
        if (group_of_operation(tr1->value) == 2)
        {
            if (group_of_operation(tr1->left->value) != 2 && tr1->left->left != NULL)//if 2nd priorities are from the start, we can just omit them and calculate by order later
            {
                char number_for_current_operation = tr1->left->right->value; // one node down in the tree

                number_for_current_operation = perf_operation(number_for_current_operation, tr1->right->value, tr1->value);
                tr1->right->value = '1'; // as 1 will not affect the calculations in the future

                for (int i = 0;i < count_same_priority_in_one_seq;i++) // as we go from one node down 
                {
                    tr1 = tr1->prev;
                    number_for_current_operation = perf_operation(number_for_current_operation, tr1->right->value, tr1->value);
                    tr1->right->value = '1'; // as 1 will not affect the calculations in the future

                }

                for (int i = 0;i < count_same_priority_in_one_seq;i++)
                {
                    tr1 = tr1->left;
                }

                tr1->left->right->value = number_for_current_operation;

                count_same_priority_in_one_seq = 0;
            }
            else
            {
                count_same_priority_in_one_seq++;
            }
        }
		
        tr1 = tr1->left;
    }
	
	//finding the result (group3):
    char res;

    tr1 = tree;
	while(tr1->left!=NULL) //moving to the last left node of the tree
	{
        tr1 = tr1->left;
	}
	
    if (tr1->right == NULL) // it usually has to be after the previous steps
    {
        tr1 = tr1->prev;
        tr1->right->value = perf_operation(tr1->left->value, tr1->right->value, tr1->value);
        tr1 = tr1->prev;
    }
    while(tr1!=NULL)
    {
        if (tr1->left)
            tr1->right->value = perf_operation(tr1->left->right->value, tr1->right->value, tr1->value);
    	
        tr1 = tr1->prev;
    }
	
    res = tree->right->value;
    delete tree;
    return res;
}


void print_cons_util(expr_tree* root, int space)
{
    // Base case 
    if (root == NULL)
        return;

    // Increase distance between levels 
    space += 3;

    print_cons_util(root->left, space);

    for (int i = 3; i < space; i++)
        cout << " ";
    cout << root->value << "\n";

    print_cons_util(root->right, space);
}
 
void print_cons(expr_tree* root)
{
    // Pass initial space count as 0 
    print_cons_util(root, 0);
}


int main()
{
    //char postfix[] = "ab+ef*g*-";                                  //
    char postfix[] = "53%2^2-4-2*8/3+7*5%";                          //
    expr_tree* r = constructTree(postfix);
    cout << "infix expression is \n";
    infix_order(r);
    cout << endl;
    cout << "result: " << calculating(r) - '0' << endl;              //
    print_cons(r);                                                   //
    return 0;
}