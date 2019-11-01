#include<iostream>
#include<bits/stdc++.h> 
using namespace std; 

struct Queue 
{ 
	// Initialize front and rear 
	int rear, front; 

	// Circular Queue 
	int size; 
	int *arr; 

	Queue(int s) 
	{ 
	front = rear = -1; 
	size = s; 
	arr = new int[s]; 
	} 

	void add_item(int value); 
	int remove_itm(); 
	void display_item(); 
}; 


/* Function to create Circular queue */
void Queue::add_item(int value) 
{ 
	if ((front == 0 && rear == size-1) || 
			(rear == (front-1)%(size-1))) 
	{ 
		printf("\nQueue is Full"); 
		return; 
	} 

	else if (front == -1) /* Insert First Element */
	{ 
		front = rear = 0; 
		arr[rear] = value; 
	} 

	else if (rear == size-1 && front != 0) 
	{ 
		rear = 0; 
		arr[rear] = value; 
	} 

	else
	{ 
		rear++; 
		arr[rear] = value; 
	} 
} 

// Function to delete element from Circular Queue 
int Queue::remove_itm() 
{ 
	if (front == -1) 
	{ 
		printf("\nQueue is Empty"); 
		return INT_MIN; 
	} 

	int data = arr[front]; 
	arr[front] = -1; 
	if (front == rear) 
	{ 
		front = -1; 
		rear = -1; 
	} 
	else if (front == size-1) 
		front = 0; 
	else
		front++; 

	return data; 
} 

// Function displaying the elements 
// of Circular Queue 
void Queue::display_item() 
{ 
	if (front == -1) 
	{ 
		printf("\nQueue is Empty"); 
		return; 
	} 
	printf("\nElements in Circular Queue are: "); 
	if (rear >= front) 
	{ 
		for (int i = front; i <= rear; i++) 
			printf("%d ",arr[i]); 
	} 
	else
	{ 
		for (int i = front; i < size; i++) 
			printf("%d ", arr[i]); 

		for (int i = 0; i <= rear; i++) 
			printf("%d ", arr[i]); 
	} 
} 

/* Driver of the program */
int main() 
{ 
	Queue q(5); 

	// Inserting elements in Circular Queue 
	q.add_item(14); 
	q.add_item(22); 
	q.add_item(13); 
	q.add_item(-6); 

	// Display elements present in Circular Queue 
	q.display_item(); 

	// Deleting elements from Circular Queue 
	printf("\nDeleted value = %d", q.remove_itm()); 
	printf("\nDeleted value = %d", q.remove_itm()); 

	q.display_item(); 

	q.add_item(9); 
	q.add_item(20); 
	q.add_item(5); 

	q.display_item(); 

	q.add_item(20); 
	return 0; 
} 
