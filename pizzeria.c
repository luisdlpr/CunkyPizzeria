// pizzeria.c
//
// This program was written by LUIS VICENTE DE LA PAZ REYES (z5206766)
// on 9.11.21
//
// Contains functions and struct definitions for the game Cunky Pizzeria:
// A pizza restaurant management simulator.
//
// Version 1.0.0: Release

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pizzeria.h"

#define ORDER_REQ 1
#define STOCK 2
#define ADDED 1

struct ingredient {
    struct ingredient *next;
    char name[MAX_STR_LENGTH];
    int amount;
    double price;
};

struct order {
    struct order *next;
    char customer_name[MAX_STR_LENGTH];
    char pizza_name[MAX_STR_LENGTH];
    double price;
    int time;
    struct ingredient *ingredients;
};

struct pizzeria {
    struct order *selected;
    struct order *orders;
    struct ingredient *stock;
};

//////////////////////////////////////////////////////////////////////

// Prints a single order
void print_order(
    int num,
    char *customer,
    char *pizza_name,
    double price,
    int time_allowed
);

// Prints an ingredient given the name, amount and price in the required format.
void print_ingredient(char *name, int amount, double price);

// adds to linked list (to order requirements or store stock: ORDER_REQ or STOCK)
// alphabetically based on ingredient name.
int add_ingredient_ordered(struct ingredient *new, struct pizzeria *pizzeria, 
                           int order_or_stock);

////////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                          //
////////////////////////////////////////////////////////////////////////


struct pizzeria *create_pizzeria() {
    // Allocates memory to store a `struct pizzeria` and returns a pointer to
    // it. The variable `new` holds this pointer!
    struct pizzeria *new = malloc(sizeof(struct pizzeria));
    //initialize data
    new->orders = NULL;
    new->selected = NULL;
    new->stock = NULL;
    return new;
}

// Adds order node of specified data and places it under pizzeria.
int add_order(
    struct pizzeria *pizzeria,
    char *customer,
    char *pizza_name,
    double price,
    int time_allowed
) {
    //Check for valid input.
    if (price < 0) {
        return INVALID_PRICE;
    }
    if (time_allowed <= 0) {
        return INVALID_TIME;
    }
    //Declare holder variables
    struct order *new_order = malloc(sizeof(struct order));
    struct order *first_order;
    //Fill order with data.
    strcpy(new_order->customer_name, customer);
    strcpy(new_order->pizza_name, pizza_name);
    new_order->price = price;
    new_order->time = time_allowed;
    new_order->next = NULL;
    new_order->ingredients = NULL;
    //If no current orders, make new order first order (head).
    //Else place at end of list.  
    first_order = pizzeria->orders;
    if (first_order == NULL) {
        pizzeria->orders = new_order;
    } 
    else {
        //Go to end of list
        while (first_order->next != NULL) {
            first_order = first_order->next;
        }
        first_order->next = new_order;
    }
    return SUCCESS;
}

//prints all orders in pizzeria.
void print_all_orders(struct pizzeria *pizzeria) {
    //declare variables
    struct order *head = pizzeria->orders;
    int order_number;
    //initialize order counter.
    order_number = 1;
    //Exit function if order list is empty
    if (head == NULL) {
        print_selected_order(pizzeria);
        return;
    }
    //Print linked list from start until 2nd last entry
    while (head->next != NULL) {
        print_order(order_number, head->customer_name, head->pizza_name,
                    head->price, head->time);
        order_number ++;
        head = head->next;
    }
    //Print last entry
    print_order(order_number, head->customer_name, head->pizza_name, 
                head->price, head->time);
    print_selected_order(pizzeria);
    return;
}

//returns the time awaiting for the most urgent order in the pizzeria.
int next_deadline(struct pizzeria *pizzeria) {
    //Declare variables
    struct order *head = pizzeria->orders;
    int urgent_time;
    //Exit and error for empty order list.
    if (head == NULL) {
        return INVALID_CALL;
    }
    //initialize most urgent order for head of list
    urgent_time = head->time;
    //compare most urgent order with each entry of linked list and update
    //if more urgent order is found
    while (head != NULL) {
        if (head->time < urgent_time) {
            urgent_time = head->time;
        }
        head = head->next;
    }
    return urgent_time;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                          //
////////////////////////////////////////////////////////////////////////

//order in pizzeria is selected (from left to right)
void select_next_order(struct pizzeria *pizzeria) {
    // if no selection has been made, selects first order (head).
    if (pizzeria->selected == NULL) {
        pizzeria->selected = pizzeria->orders;
    }
    // Otherwise selects next order in linked list
    else {
        pizzeria->selected = pizzeria->selected->next;
    }
    return;
}

//order in pizzeria is selected (from right to left)
void select_previous_order(struct pizzeria *pizzeria) {
    //declare holder structs
    struct order *current;
    struct order *previous;
    //initialize holder structs
    current = pizzeria->selected;
    previous = pizzeria->orders;
    //if current selection is head of list, select NULL next
    if (current == previous) {
        pizzeria->selected = NULL;
        return;
    }
    //if current selection is NULL, 
    //loop will run until last element of list and select.
    //otherwise loop will exit when previous element to current selection is found
    while (previous->next != NULL) {
        if (current != NULL 
            && strcmp(current->customer_name, previous->next->customer_name) == 0 
            && strcmp(current->pizza_name, previous->next->pizza_name) == 0) {
            pizzeria->selected = previous;
            return; 
        }
        previous = previous->next;
    }
    pizzeria->selected = previous;
    return; 
}

//selected order is printed
void print_selected_order(struct pizzeria *pizzeria) {
    //declare holder structs.
    struct order *selected;
    struct ingredient *ingredient;
    //initialize.
    selected = pizzeria->selected;
    //print statement for no selection.
    if (selected == NULL) {
        printf("\nNo selected order.\n");
    }
    //otherwise, print order details...
    else {
        printf("\nThe selected order is %s's %s pizza", selected->customer_name,
                                                        selected->pizza_name);
        printf(" ($%.2lf) due in %d minutes.\n", selected->price, selected->time);
        //and ingredient details through loop that runs through linked list.
        ingredient = selected->ingredients;
        while (ingredient != NULL) {
            print_ingredient(ingredient->name, ingredient->amount, 
                             ingredient->price);
            ingredient = ingredient->next;
        } 
    }
}

//ingredient of specified data is added to selected order.
int add_ingredient(
    struct pizzeria *pizzeria,
    char *ingredient_name,
    int amount,
    double price
) {
    //error checks.
    if (amount <= 0) {
        return INVALID_AMOUNT;
    }
    else if (price < 0) {
        return INVALID_PRICE;
    }
    else if (pizzeria->selected == NULL) {
        return INVALID_ORDER;
    }
    //create ingredient node
    //declare and initialize struct to search through linked list (ingredients)
    struct ingredient *new_ingredient = malloc(sizeof(struct ingredient));
    struct ingredient *place;
    place = pizzeria->selected->ingredients;
    //fill new ingredient node with data.
    strcpy(new_ingredient->name, ingredient_name);
    new_ingredient->amount = amount;
    new_ingredient->price = price;
    //if ingredients list is empty, start list with new ingredient as head.
    if (place == NULL) {
        pizzeria->selected->ingredients = new_ingredient;
        new_ingredient->next = NULL;    
    }
    //else call function to add ingredient in correct alphabetical spot in list.
    else {
        //if ingredient is already on list, free space malloc'd for new ingredient.
        if (add_ingredient_ordered(new_ingredient, pizzeria, ORDER_REQ) == ADDED) {
            free(new_ingredient);
        }
    }
    return SUCCESS;
}

//total profit of order is calculated based on purchase price and ingredient cost.
double calculate_total_profit(struct pizzeria *pizzeria) {
    //error checks
    if (pizzeria->selected == NULL) {
        return INVALID_ORDER;
    }
    //declare and initialize holder structs
    struct order *selected = pizzeria->selected;
    struct ingredient *head = pizzeria->selected->ingredients;
    //declare and initialize (as base price) profit output sum
    double profit;
    profit = selected->price;
    //use loop to subtract cost of each ingredient from profit sum
    while (head != NULL) {
        profit = profit - head->amount*head->price;
        head = head->next;
    }
    return profit;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                          //
////////////////////////////////////////////////////////////////////////

//selected order is removed from list and space is freed
int cancel_order(struct pizzeria *pizzeria) {
    //error checks
    if (pizzeria->selected == NULL) {
        return INVALID_ORDER;
    }
    //initialize holder variables
    struct order *selected = pizzeria->selected;
    struct ingredient *previous = selected->ingredients;
    //clear ingredients list
    if (previous == NULL) {
        
    }
    else if (previous->next == NULL) {
        free(previous);
    }
    else {
        struct ingredient *current = previous->next;
        while (current != NULL) {
            free(previous);
            previous = current;
            current = current->next;
        }
        free(previous);
    }
    //select previous order to reorder order list and selection
    select_previous_order(pizzeria);
    if (pizzeria->selected != NULL) {
        pizzeria->selected->next = pizzeria->selected->next->next;
        pizzeria->selected = pizzeria->selected->next;
    }
    else {
        pizzeria->selected = selected->next;
    }
    //fix head pointer if broken
    if (selected == pizzeria->orders) {
        pizzeria->orders = pizzeria->orders->next;
    }
    //clear selected order
    free(selected);
    return SUCCESS;
}

//pizzeria orders are freed and pizzeria is freed.
void free_pizzeria(struct pizzeria *pizzeria) {
    //selects order list head
    pizzeria->selected = pizzeria->orders;
    //cancels all orders
    while (pizzeria->orders != NULL) {
        cancel_order(pizzeria);
    }
    //deletes all stock
    while (pizzeria->stock != NULL) {
        struct ingredient *next;
        next = pizzeria->stock->next;
        free(pizzeria->stock);
        pizzeria->stock = next;
    }
    //frees final stock and order
    free(pizzeria->stock);
    free(pizzeria);
    return;
}

//fills store stock with ingredient with specified data
int refill_stock(
    struct pizzeria *pizzeria,
    char *ingredient_name,
    int amount,
    double price ) {
    //error checks
    if (price < 0) {
        return INVALID_PRICE;
    }
    else if (amount <= 0) {
        return INVALID_AMOUNT;
    }
    //create new ingredient node and fill with data
    struct ingredient *new = malloc(sizeof(struct ingredient));
    strcpy(new->name, ingredient_name);
    new->amount = amount;
    new->price = price;
    new->next = NULL;
    //if store stock empty, insert new ingredient as head
    if (pizzeria->stock == NULL) {
        pizzeria->stock = new; 
    }
    //otherwise use add ingredient function to add in alphabetical order.
    else {
        //if ingredient already exists in list, free created node.
        if (add_ingredient_ordered(new, pizzeria, STOCK) == ADDED) {
            free(new);
        }
    }
    return SUCCESS;
}

//prints all stock in store
void print_stock(struct pizzeria *pizzeria) {
    //opens store stock list head
    struct ingredient *current;
    current = pizzeria->stock;
    //prints initial statement
    printf("The stock contains: \n");
    //prints details of each element in list
    while (current != NULL) {
        print_ingredient(current->name, current->amount, 
                         current->price);
        current = current->next;
    }     
    return;
}

//checks if store has sufficient stock to fulfill selected order.
int can_complete_order(struct pizzeria *pizzeria) {
    //error checks
    if (pizzeria->selected == NULL || pizzeria->selected->ingredients == NULL) {
        return INVALID_ORDER;
    }
    //declare and initialize holder variables and missing switch
    struct ingredient *required = pizzeria->selected->ingredients;
    struct ingredient *stock_search = pizzeria->stock;
    int missing;
    //loops until end of ingredients list (required ingredients for order)
    while (required != NULL) {
        missing = 0;
        //checks for match between required ingredient and stock list.  
        //If found, missing variable switched and loop starts for next requirements
        //otherwise if no match is found, loop exits and returns insufficient code
        //If loop completes without insufficient code, order can be completed.
        while (stock_search != NULL && missing == 0) {
            if (strcmp(stock_search->name, required->name) == 0 
                && stock_search->amount >= required->amount) {
                missing = 1;
            }
            stock_search = stock_search->next;
        }
        if (missing == 0) {
            return INSUFFICIENT_STOCK;
        }
        required = required->next;
    }
    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////

int complete_order(struct pizzeria *pizzeria) {

    return SUCCESS;
}

int save_ingredients(struct pizzeria *pizzeria, char *file_name) {

    return SUCCESS;
}

int load_ingredients(struct pizzeria *pizzeria, char *file_name) {

    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//                         HELPER FUNCTIONS                           //
////////////////////////////////////////////////////////////////////////

// Prints a single order
//
// `print_order` will be given the parameters:
// - `num` -- the integer that represents which order it is sequentially.
// - `customer` -- the name of the customer for that order.
// - `pizza_name` -- the pizza the customer ordered.
// - `price` -- the price the customer is paying for the pizza.
// - `time_allowed` -- the time the customer will wait for the order.
//
// `print_order` assumes all parameters are valid.
//
// `print_order` returns nothing.
//
// This will be needed for Stage 1.
void print_order(
    int num,
    char *customer,
    char *pizza_name,
    double price,
    int time_allowed
) {
    printf("%02d: %s ordered a %s pizza ($%.2lf) due in %d minutes.\n",
        num, customer, pizza_name, price, time_allowed);
    return;
}

// Prints a single ingredient
//
// `print_ingredient` will be given the parameters:
// - `name` -- the string which contains the ingredient's name.m
// - `amount` -- how many of the ingredient we either need or have.
// - `price` -- the price the ingredient costs.
//
// `print_ingredient` assumes all parameters are valid.
//
// `print_ingredient` returns nothing.
//
// This will be needed for Stage 2.
void print_ingredient(char *name, int amount, double price) {
    printf("    %s: %d @ $%.2lf\n", name, amount, price);
}

//Adds ingredient to linked list alphabetically based on ingredient name.
//
//'add_ingredient_ordered' will be given:
//  'new' -- pointer to data filled ingredient node  
//  'pizzeria' -- pointer to pizzeria
//  'order_or_stock' -- int that follows below 
//      - add to order -> order_or_stock == ORDER_REQ
//      - add to stock -> order_or_stock == STOCK
int add_ingredient_ordered(struct ingredient *new, struct pizzeria *pizzeria,
                         int order_or_stock) {
    //declare and initialize shorthand struct for head of ingredients list
    struct ingredient *head;
    struct ingredient *previous_head;
    if (order_or_stock == ORDER_REQ) {
        head = pizzeria->selected->ingredients;
        previous_head = pizzeria->selected->ingredients;
    }
    else {
        head = pizzeria->stock;
        previous_head = pizzeria->stock;
    }
    //if new ingredient comes before head of current list in alphabet (name)
    //make new ingredient head
    if (strcmp(new->name, head->name) < 0) {
        if (order_or_stock == ORDER_REQ) {
            pizzeria->selected->ingredients = new;
        }
        else {
            pizzeria->stock = new;
        }
        new->next = previous_head;
        return 0;
    }
    //if new ingredient has the same name as the head, add to the previous amount
    else if (strcmp(new->name, head->name) == 0) {
        head->amount = head->amount + new->amount;
        return ADDED;
    }
    //loop through the list to examine elements other than the head.
    //if new ingredient has same name as any other elements, add to prev amount. 
    while (head->next != NULL && strcmp(new->name, head->next->name) >= 0) {
        if (strcmp(new->name, head->next->name) == 0) {
            head->next->amount = head->next->amount + new->amount;
            return ADDED;
        }
        //Otherwise exit loop at correct alphabetical spot
        head = head->next;  
    }
    //insert element into correct spot.
    new->next = head->next;
    head->next = new;
    return 0;
}
