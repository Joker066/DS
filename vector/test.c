#include "vector.c"
int main(){
    vector *v = new_vector();
    for(int i = 0; i < 20; i++)
        v->push_back(v, i);
    v->print(v);
    v->insert(v, 5, 20);
    v->print(v);
    printf("%d\n", v->search(v, 20));
    v->remove(v, 5);
    v->print(v);
    return 0;
}
