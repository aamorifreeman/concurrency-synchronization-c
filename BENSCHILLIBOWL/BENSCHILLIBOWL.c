#include "BENSCHILLIBOWL.h"
#include <assert.h>

// ---------------- MENU DEFINITIONS ----------------
MenuItem BENSCHILLIBOWLMenu[] = {
    "Ben's Chili",
    "Half Smoke",
    "Chili-Cheese Fries",
    "Ben's Chili Burger",
    "Milkshake",
    "Bens Hotdog",
    "Veggie Wrap",
    "Chicken Sandwich",
    "Onion Rings",
    "Wings"
};
int BENSCHILLIBOWLMenuLength = 10;

// ----------------------------------------------------

MenuItem PickRandomMenuItem() {
    int r = rand() % BENSCHILLIBOWLMenuLength;
    return BENSCHILLIBOWLMenu[r];
}

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {

    BENSCHILLIBOWL* mcg = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));
    if (mcg == NULL) {
        printf("Malloc failed\n");
        exit(1);
    }

    mcg->orders = NULL;
    mcg->current_size = 0;
    mcg->max_size = max_size;
    mcg->next_order_number = 1;
    mcg->orders_handled = 0;
    mcg->expected_num_orders = expected_num_orders;

    pthread_mutex_init(&mcg->mutex, NULL);
    pthread_cond_init(&mcg->can_add_orders, NULL);
    pthread_cond_init(&mcg->can_get_orders, NULL);

    printf("Restaurant is open!\n");
    return mcg;
}

void CloseRestaurant(BENSCHILLIBOWL* mcg) {

    if (mcg->orders_handled != mcg->expected_num_orders) {
        printf("WARNING: NOT ALL ORDERS WERE FULFILLED!\n");
    }

    pthread_mutex_destroy(&mcg->mutex);
    pthread_cond_destroy(&mcg->can_add_orders);
    pthread_cond_destroy(&mcg->can_get_orders);

    printf("Restaurant is closed!\n");
    free(mcg);
}

int AddOrder(BENSCHILLIBOWL* mcg, Order* order) {

    pthread_mutex_lock(&mcg->mutex);

    while (mcg->current_size == mcg->max_size) {
        pthread_cond_wait(&mcg->can_add_orders, &mcg->mutex);
    }

    order->order_number = mcg->next_order_number;
    mcg->next_order_number++;

    if (mcg->orders == NULL) {
        mcg->orders = order;
        order->next = NULL;
    } else {
        Order* curr = mcg->orders;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = order;
        order->next = NULL;
    }

    mcg->current_size++;

    pthread_cond_signal(&mcg->can_get_orders);
    pthread_mutex_unlock(&mcg->mutex);

    return order->order_number;
}

Order* GetOrder(BENSCHILLIBOWL* mcg) {

    pthread_mutex_lock(&mcg->mutex);

    if (mcg->orders_handled == mcg->expected_num_orders) {
        pthread_mutex_unlock(&mcg->mutex);
        return NULL;
    }

    while (mcg->current_size == 0) {

        if (mcg->orders_handled == mcg->expected_num_orders) {
            pthread_mutex_unlock(&mcg->mutex);
            return NULL;
        }

        pthread_cond_wait(&mcg->can_get_orders, &mcg->mutex);
    }

    Order* order = mcg->orders;
    mcg->orders = mcg->orders->next;

    mcg->current_size--;
    mcg->orders_handled++;

    pthread_cond_signal(&mcg->can_add_orders);
    pthread_mutex_unlock(&mcg->mutex);

    return order;
}


