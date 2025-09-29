// file.c
int fast_add(int a, int b) __attribute__((fastcall)) {
    return a + b; 
}

int main() {
    return fast_add(3, 4); 
}
