/*************************************
* Lab 1 Exercise 2
* Name: Chew Ee Jia
* Student No: A0217981W
* Lab Group: 13
*************************************/

#include "node.h"
#include <stdlib.h>

// Add in your implementation below to the respective functions
// Feel free to add any headers you deem fit 


// Inserts a new node with data value at index (counting from head
// starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data) {
    node *new_node = (node *)malloc(sizeof(node));
    new_node->data = data;
    node *curr = lst->head;

    if (index == 0) {
        lst->head = new_node;
        new_node->next = curr;
    } else {
        for (int i = 0; i < index - 1; i++) {
            curr = curr->next;
        }
        node *temp = curr->next;
        curr->next = new_node;
        new_node->next = temp;
    }
}

// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.
void delete_node_at(list *lst, int index) {
    node *curr = lst->head;
    for (int i = 0; i < index - 1; i++) {
        curr = curr->next;
    }

    if (index < 1) {
        lst->head = curr->next;
        free(curr);
    } else {
        node *next_node = curr->next->next;
        node *deleted_node = curr->next;
        curr->next = next_node;
        free(deleted_node);
    }
}

// Search list by the given element.
// If element not present, return -1 else return the index. If lst is empty return -2.
//Printing of the index is already handled in ex2.c
int search_list(list *lst, int element) {
    if (lst->head == NULL) {
        return -2;
    }

    node *curr = lst->head;
    int index = 0;
    while (curr != NULL) {
        if (curr->data == element) {
            return index;
        }
        curr = curr->next;
        index++;
    } 
    
    return -1;
}

// Reverses the list with the last node becoming the first node.
void reverse_list(list *lst) {

    node *curr = lst->head;
    node *next = NULL;
    node *prev = NULL;
    while (curr != NULL) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    lst->head = prev;

}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
    node *curr = lst->head;
    node *temp = curr;
    while (curr != NULL) {
        temp = curr->next;
        free(curr);
        curr = temp; 
    }
    lst->head = NULL;
}
