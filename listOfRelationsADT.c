

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "handy_list.h"

handy_list list;
//copied from list of objects but corrected for list of relations

//function prototypes
int reflexivity(int);
int symmetry(int,int);


//relation sthat takes one arguement (to be to applied on elements of list)
bool oneArg(int x)
{
	if (x%2==0)
		return 1;
	else return 0;
}
//relation sthat takes two arguement (to be to applied on elements of list)
bool twoArg(int x,int y)
{
	if (x%2==0||y%2==0)
		return true;
	else return false;
}

bool threeArg(int x,int y,int z)
{
	return true;
}

// relations defined as union because the relations are not of thesame same type hence need for an appropriate structure that can hold the various types of relations
union relations
{			
	int (*ref)(int x);
	int (*sym)(int x,int y);
};

union relations R;

 void init( union relations * R )
 {
 	R->ref = reflexivity;
    R->sym = symmetry;
 }



//define relations for just one operation(even) for now
//add other operations later
int reflexivity(int x)
{
  if (x%2==0)   
      return 1; 
      else return 0;
 }
 int symmetry(int x, int y){
  if (x%2==0 && y%2==0)  
      return 1; 
     else return 0;                                       
 }


/*loop on list of elements
when we have just one element in the list then we want to call checkRelation1 hence we loop through the list just once.
same thing happens when we have two elemnents and so on. 
In the end we can come up with a general formular to check relations while we go through the list of elements 
*/
void checkRelation1( int item )
{	
	if( oneArg(item) == true )
	{
		list->add_back(list, item);
        printf("Add success\n");
	}
	else
	{
		printf("ERROR");
	}
}

void checkRelation2(int item)
{	

	if( list->length(list) == 0 )
	{
		checkRelation1(item);
	}
	else
	{
		int any = list->get_back(list);
		
		if( twoArg(any, item) == true )
		{
			list->add_back(list, item);
		}
		else
		{	
			printf("ERROR");
		}
	}
}

void checkRelation3(int item)
{	

	if( list->length(list) == 0 )
	{
		checkRelation1(item);
	}
	else if( list->length(list) == 1 )
	{
		checkRelation2( item );
	}
	else
	{
		int any1 = list->get_back(list);
		int any2 = list->get_front(list);
		
		if( threeArg(any1, any2, item) == true )
		{
			list->add_back(list, item);
		}
		else
		{	
			printf("ERROR");
		}
	}
}

















//This will be in case we want to add a new relation to the list of relations
/*
	if(*head==NULL)
	*head=new_node;
	else{
		//find the last element and set it to new_node;
		 struct node* current = *head;
		 if(current->next!=NULL)
		 current = current->next;
		 else current = new_node;
	}
}

 */

   

//is list empty i.e we have no relation to check
/*
bool isEmpty() {
   return head == NULL;
}
*/


   
//If need be to reverse the order of relations
/*
void reverse(struct node** head_ref) {
   struct node* prev   = NULL;
   struct node* current = *head_ref;
   struct node* next;
	
   while (current != NULL) {
      next  = current->next;
      current->next = prev;   
      prev = current;
      current = next;
   }
	
   *head_ref = prev;
}
*/
