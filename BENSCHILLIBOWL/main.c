#include "BENSCHILLIBOWL.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

BENSCHILLIBOWL* bcb;  /* GLOBAL RESTAURANT */

void* BENSCHILLIBOWLCustomer(void* tid) {

    int id = *((int*) tid);
    Order* order = (Order*) malloc(sizeof(Order));
    if (order == NULL) {
        printf("Malloc failed\n");
        pthread_exit(NULL);
    }

    order->menu_item = PickRandomMenuItem();
    order->customer_id = id;
    order->next = NULL;

    printf("Customer %d is placing order\n", id);
    AddOrder(bcb, order);

    pthread_exit(NULL);
}

void* BENSCHILLIBOWLCook(void* tid) {

    int id = *((int*) tid);
    Order* order;

    while (1) {
        order = GetOrder(bcb);

        if (order == NULL) {
            break;
        }

        printf("Cook %d fulfilled order %d from customer %d (%s)\n",
               id,
               order->order_number,
               order->customer_id,
               order->menu_item);

        free(order);
    }

    pthread_exit(NULL);
}

int main() {

    int num_customers = 10;
    int num_cooks = 3;
    int max_size = 5;

    pthread_t customers[num_customers];
    pthread_t cooks[num_cooks];

    int ids_customers[num_customers];
    int ids_cooks[num_cooks];

    bcb = OpenRestaurant(max_size, num_customers);

    int i;

    for (i = 0; i < num_customers; i++) {
        ids_customers[i] = i + 1;
        pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, &ids_customers[i]);
    }

    for (i = 0; i < num_cooks; i++) {
        ids_cooks[i] = i + 1;
        pthread_create(&cooks[i], NULL, BENSCHILLIBOWLCook, &ids_cooks[i]);
    }

    for (i = 0; i < num_customers; i++) {
        pthread_join(customers[i], NULL);
    }

    for (i = 0; i < num_cooks; i++) {
        pthread_join(cooks[i], NULL);
    }

    CloseRestaurant(bcb);

    return 0;
}
