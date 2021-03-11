#include "vector.c"
int main(){
    vector *v = new_vector();
    v->push_back(v, 1);
    v->push_front(v, 3);
    v->print(v);
    printf("%d\n", v->peek_back(v));
    printf("%d\n", v->pop_front(v));
    v->print(v);
    printf("%d\n", v->peek_back(v));
    printf("%d\b", v->length(v));
    v->insert(v, 1, 2);
    v->print(v);
    return 0;
}
